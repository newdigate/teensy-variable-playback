/* Extension to EventResponder - Simple event-based programming for Arduino
 * Copyright 2024 Jonathan Oakley
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#include <EventResponder.h>
	
#if !defined(AudioEventResponder_h)
#define AudioEventResponder_h

namespace newdigate {

class AudioEventResponder : public EventResponder
{
	static uint8_t active_flags_copy;
	static int disableCount;
	static bool forceResponse; // if true, we don't change yield_active_check_flags
	bool _isPolled;
	AudioEventResponder* _aprev,*_anext; // can't use base ones, protected
	
	struct triggeredList {AudioEventResponder* first, *last;};
	void addToList(void);//, triggeredList& list);
	void removeFromList(void);//, triggeredList& list);
	static triggeredList pollList;
	
  public:
	// Stash EventResponder yield flag, then disable, 
	// unless we're forcing the response not to be masked
	static void disableResponse(void) 
	{
		if (0 == disableCount)
		{
			active_flags_copy = yield_active_check_flags;
			if (!forceResponse)
				yield_active_check_flags &= ~YIELD_CHECK_EVENT_RESPONDER;
		}
		disableCount++;
	}
	
	// Restore the yield enable flag
	static void enableResponse(void) 
	{ 
		disableCount--;
		if (disableCount <= 0)
		{
			if (!forceResponse)
				yield_active_check_flags |= active_flags_copy;
			disableCount = 0;
		}
	}

	// Update the stashed enable flag after it might have changed,
	// and re-disable yield response if we're expecting that
	static void updateResponse(void) 
	{
		active_flags_copy |= yield_active_check_flags & YIELD_CHECK_EVENT_RESPONDER;
		if (disableCount > 0 && !forceResponse)
			yield_active_check_flags &= ~YIELD_CHECK_EVENT_RESPONDER;
			
	}

	
	static void setForceResponse(bool force) { forceResponse = force; }
	static bool getForceResponse(void) { return forceResponse; }
	
	// Attach a function to be executed when polled from user code
	void attachPolled(EventResponderFunction function) 
	{
		detach();
		bool irq = disableInterrupts();
		_function = function;
		_type = EventTypeImmediate; // not used
		_isPolled = true;
		enableInterrupts(irq);		
	}


	void detach(void)
	{
		if (_isPolled)
		{
			_isPolled = false;
			_type = EventTypeDetached;
			bool irq = disableInterrupts();
			if (_triggered)
				removeFromList();
			enableInterrupts(irq);		
		}
		else
			EventResponder::detach();
	}
	
	
	void triggerEvent(int status=0, void *data=nullptr) 
	{
		if (_isPolled)
		{
			bool irq = disableInterrupts();
			_status = status;
			_data = data;
			if (!_triggered)
			{
				addToList();
				_triggered = true;
			}
			enableInterrupts(irq);
		}
		else
			EventResponder::triggerEvent(status, data);
		
		// fix bug in EventResponder
		if (EventTypeDetached == _type)
			_triggered = false;
	}

	
	void _runPolled(void)
	{
		bool irq = disableInterrupts();
		removeFromList();			// remove from pending list
		_triggered = false;		// no longer triggered
		enableInterrupts(irq);
		if (_function)			// if function is attached...
			(*(_function))(*((EventResponder*) this)); // ...run it
	}	

	
	/*
	 * Run event from polled list.
	 * Return value is usually 0 or 1, because as things stand update() generates
	 * an event on every execution - it would be more efficient if it only 
	 * triggered a reload request if it knew there was an empty buffer. 
	 *
	 * \return -1 if nothing to do, 0 for event run but none remain, or 1 if pending events remain
	 */
	static int runPolled(void)
	{
		int result = -1;
		
		AudioEventResponder* toRun = (AudioEventResponder*) pollList.first;  // keep a pointer to it
		if (toRun != nullptr) // we have something to run
		{
			toRun->_runPolled();
			result = 0;
		}
		
		return pollList.first == nullptr?result:1;
	}
	
  protected:
	static bool disableInterrupts() {
		uint32_t primask;
		__asm__ volatile("mrs %0, primask\n" : "=r" (primask)::);
		__disable_irq();
		return (primask == 0) ? true : false;
	}
	static void enableInterrupts(bool doit) {
		if (doit) __enable_irq();
	}
	
  
};

} // namespace

#endif // !defined(AudioEventResponder_h)
//#endif // defined(EventResponder_h)
