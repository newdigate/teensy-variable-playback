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
 
//#if defined(EventResponder_h)
#if !defined(AudioEventResponder_h)
#define AudioEventResponder_h

class AudioEventResponder : public EventResponder
{
	static uint8_t active_flags_copy;
	static int disableCount;
	static bool forcePolled;
	bool _isPolled;
	AudioEventResponder* _aprev,*_anext; // can't use base ones, protected
	
	struct triggeredList {AudioEventResponder* first, *last;};
	void addToList(void);//, triggeredList& list);
	void removeFromList(void);//, triggeredList& list);
	static triggeredList pollList;
	
  public:
	static void disableResponse(void) 
	{
		if (0 == disableCount)
		{
			active_flags_copy = yield_active_check_flags;
			yield_active_check_flags &= ~YIELD_CHECK_EVENT_RESPONDER;
		}
		disableCount++;
	}
	
	static void enableResponse(void) 
	{ 
		disableCount--;
		if (disableCount <= 0)
		{
			yield_active_check_flags = active_flags_copy;
			disableCount = 0;
		}
	}
	
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
	 * \return true if pending events remain
	 */
	static bool runPolled(void)
	{
		AudioEventResponder* toRun = (AudioEventResponder*) pollList.first;  // keep a pointer to it
		if (toRun != nullptr) // we have something to run
			toRun->_runPolled();
		
		return pollList.first != nullptr;
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


#endif // !defined(AudioEventResponder_h)
//#endif // defined(EventResponder_h)
