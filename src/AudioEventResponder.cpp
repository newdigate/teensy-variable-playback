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

#include "AudioEventResponder.h"

using namespace newdigate;
 
uint8_t AudioEventResponder::active_flags_copy;
int AudioEventResponder::disableCount;
AudioEventResponder::triggeredList AudioEventResponder::pollList;
bool AudioEventResponder::forceResponse;

void AudioEventResponder::addToList(void)
{
	triggeredList& list = pollList;

	if (list.first == nullptr) // list is empty...
	{
		list.first = this;		// ...event is the first...
		_aprev = nullptr; 	// ...nothing before it
	} 
	else // non-empty, put after last item...
	{
		_aprev = list.last;
		_aprev->_anext = this;
	}
	list.last = this; // ...and at end of list
	_anext = nullptr;
}


void AudioEventResponder::removeFromList(void)
{
	triggeredList& list = pollList;

	if (list.first == this) 	// first list element?
		list.first = _anext;	// new first
		
	if (list.last == this) 		// last list element?
		list.last = _aprev;	// new last

	if (nullptr != _anext)	// not last
		_anext->_aprev = _aprev; // following item has new previous
		
	if (nullptr != _prev)	// not first
		_aprev->_anext = _anext; // previous item has new following
}

