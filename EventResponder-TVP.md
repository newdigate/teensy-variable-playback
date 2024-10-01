# EventResponder-based Teensy Variable Playback
This document gives a brief description of the use of the EventResponder-based branch of the Teensy Variable Playback library, plus a little technical detail.
## In use
### Basic functionality
The functionality provided by this branch is intended to be as close as possible to the original: please see the ReadMe.md file and examples for details.
### Extended functionality
In order to avoid crashes if your application tries to access the same filesystem that's being used for playback, a number of options are available:

 * Don't: wait until playback is finished, then use the filesystem. Not satisfactory for most people...
 * Mask EventResponder responses while you're accessing the filesystem
 * Perform EventResponder responses directly from your code, when you know it's safe
  

All the methods described below *must only* be called from normal foreground code, i.e. `setup()` or `loop()`. **DO NOT** call them from interrupt, it will almost certainly result in a crash or other misbehaviour. Eventually, if not sooner. Trust me on this.

### EventResponder masking
This technique temporarily masks execution of *all* EventResponder responses from within the `yield()` function, not just those relevant to the Teensy Variable Playback and/or filesystem access. Hence you should try to minimise the time spent with responses masked. 
#### `AudioEventResponder::disableResponse()`
Call this function before your code accesses the filesystem: this includes `open()`, `write()`, `read()`, `mkdir()` etc.  **EVERYTHING**
#### `AudioEventResponder::enableResponse()`
Call this function after you've finished accessing the filesystem. It's OK to leave a file open, so long as you don't access it...

### EventResponder polled mode
This is *not* part of the Teensyduino library, but has been "grafted on" by the `AudioEventResponder` derived class.
#### `AudioEventResponder::setForceResponse(bool)`
This function **must be called once, with its parameter set to `true`**, in `setup()`, to prevent `disableResponse()` and `enableResponse()` from being effective (they are called internally in the library, so this is important!). The library will now use polled events only, and *not* mask yield-based events triggered by other libraries. If *those* cause crashes, it's not my fault.

You can call it with a `false` parameter as well, to revert to needing to use the masking functions. Dunno why you would do that, though.
#### `AudioEventResponder::runPolled()`
Call this to execute *one* of the pending buffer-filling events. If it returns -1, no events were pending; 0 means one *was* pending and has been executed, and no more remain pending; 1 means one was executed, and *at least* one more remains pending. It's up to you to decide how often to call this method; it will depend on how many playback objects are active, how big their buffers are, and other factors.

## Technical stuff
Rather than directly accessing the filesystem from within the interrupt-based audio `update()` code, this library triggers events which are responded to by the EventResponder "experimental API". Originally this was done in `yield()`, but it turns out that *lots* of libraries call `yield()` internally, including filesystems, and there is no mechanism provided to defer execution of a response if it "would be unsafe" - you simply can't tell.

The EventResponder library provides neither a "polled" response mechanism, nor a way of masking responses while the system is in a state where it is unsafe to respond. These have been "grafted on" using the AudioEventResponder derived class. This is even more experimental, and it's entirely possible that its name and the names of its methods will change, at least in the short term. 

In the longer term, this functionality, or some variant thereof, may be added into the EventResponder library itself. This is technically straightforward, but unlikely to happen any time soon.