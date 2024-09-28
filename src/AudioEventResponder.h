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
 
#if defined(EventResponder_h)
#if !defined(AudioEventResponder_h)
#define AudioEventResponder_h

class AudioEventResponder : public EventResponder
{
	static uint8_t active_flags_copy;
	static int disableCount;
  public:
	static void disableResponse(void) 
	{
		if (0 == disableCount)
		{
			active_flags_copy = yield_active_check_flags;
			yield_active_check_flags &= ~YIELD_CHECK_EVENT_RESPONDER;
			disableCount++;
		}
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
};


#endif // !defined(AudioEventResponder_h)
#endif // defined(EventResponder_h)
