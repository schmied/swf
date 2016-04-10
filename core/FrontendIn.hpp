/*
 * Copyright (c) 2013, 2014, 2015, 2016, Michael Schmiedgen
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef SWF_CORE_FRONTEND_IN
#define SWF_CORE_FRONTEND_IN

#include <string>
#include <utility>

#include "Component.hpp"

class Context;


class FrontendIn {

private:
	Context *context;

protected:

public:
	FrontendIn(Context&);
	~FrontendIn();

	// getter
	Context* getContext() const;

	// game loop
	virtual void gameLoopSleep() const = 0;		// gives cpu voluntary
	virtual long gameLoopTicks() const = 0;		// must return ticks in milliseconds

	// event handling
	virtual void* eventPoll() = 0;
	virtual void* eventWait() = 0;
	virtual void eventFree(void*);				// some stupid apis allocate events and leave to the user

	// event handling
	virtual void handleEvent(void*) const = 0;

};

#endif // SWF_CORE_FRONTEND_IN

