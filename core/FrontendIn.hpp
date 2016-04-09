/*
 * Copyright (c) 2013, 2014, 2015, Michael Schmiedgen
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

#ifndef SWF_CORE_DISPLAY
#define SWF_CORE_DISPLAY

#include <string>
#include <utility>

#include "Component.hpp"

class Context;

class Display {

private:
	Context *context;

	// fps statistics
	long fpsTicksPrevious;				// need to remember for elapsed ticks calculation
	int fpsCyclesPerFrame;				// number of event loop cycles per frame
	int fpsCyclesPerFrameCounter;
	int fpsFrameMillis;				// duration of a frame
	bool fpsIsTicksElapsed(const long, const long);
	void fpsResetTicks(const long);

	// event handling
	virtual void* eventPoll() = 0;
	virtual void* eventWait() = 0;
	virtual void gameEventSleep() const = 0;		// gives cpu voluntary
	virtual long gameEventTicks() const = 0;		// must return ticks in milliseconds
	virtual void eventFree(void*);				// some stupid apis allocate events and leave to the user

protected:

public:
	Display(Context&);
	~Display();

	// getter
	Context* getContext() const;
	std::pair<int,int> getFpsStat() const;

	// drawing
//	virtual void drawBorder(const std::pair<int,int>&, const std::pair<int,int>&) const = 0;
//	virtual void drawText(const std::pair<int,int>&, const std::pair<int,int>&, const std::basic_string<char>&) const = 0;
//	virtual void draw(const std::pair<int,int>&, const std::pair<int,int>&, const std::basic_string<char>&) const = 0;
	virtual void draw(const Position&, const Style&, const std::basic_string<char>&) const = 0;
	virtual std::pair<int,int> screenDimension() const = 0;
	virtual std::pair<int,int> fontDimension() const = 0;

	// event handling
	virtual void handleEvent(void*) const = 0;
	int gameEventLoop(const int, const bool, int (*)(const bool, void*, void*), void (*)(void*), void (*)(const bool, void*), void*);
	int applicationEventLoop(int (*)(const bool, void*, void*), void*);
};

#endif // SWF_CORE_DISPLAY
