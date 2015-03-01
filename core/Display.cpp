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

#include "Display.hpp"


#include "Context.hpp"

static const std::basic_string<char> LOG_FACILITY = "DISPLAY";


/*
 * ******************************************************** constructor / destructor
 */

Display::Display(Context *c) {
	if (c == nullptr) {
		std::printf("%s <init> no context\n", LOG_FACILITY.c_str());
		return;
	}
	context = c;
	context->setDisplay(this);

	fpsTicksPrevious = 0;
	fpsFrameMillis = 0;
	fpsCyclesPerFrame = 0;
}

Display::~Display() {
}


/*
 * ******************************************************** private
 */


/*
 * fps statistics
 */

bool Display::fpsIsTicksElapsed(const long ticksCurrent, const long targetFps) {
	fpsCyclesPerFrameCounter++;
	// over 2/3 of target millis is elapsed
	if (ticksCurrent - fpsTicksPrevious > 2000 / (3 * targetFps))
		return true;
	return false;
}

void Display::fpsResetTicks(const long ticksCurrent) {
	if (fpsTicksPrevious)
		fpsFrameMillis = (int) (ticksCurrent - fpsTicksPrevious);
	fpsTicksPrevious = ticksCurrent;
	fpsCyclesPerFrame = fpsCyclesPerFrameCounter;
	fpsCyclesPerFrameCounter = 0;
}


/*
 * event handling
 */

// override e.g. to implement freeing allocated events, defaults to do nothing
void Display::eventFree(void *event) {
}


/*
 * ******************************************************** public
 */


/*
 * getter
 */

Context* Display::getContext() const {
	if (context == nullptr)
		std::printf("%s getContext() no context\n", LOG_FACILITY.c_str());
	return context;
}


std::pair<int,int> Display::getFpsStat() const {
	return { fpsFrameMillis, fpsCyclesPerFrame };
}


/*
 * event handling
 */

void* Display::gameEventLoop(const int targetFps, const bool isSleepy, bool (*onEvent)(const bool, void*, void*),
	   void (*onRender)(void*), void (*onDraw)(const bool, void*), void* userData) {
	void *e;
	for (;;) {
		const long ticks = gameEventTicks();
		const bool isElapsed = fpsIsTicksElapsed(ticks, targetFps);
		if (isElapsed) {
			fpsResetTicks(ticks);
			e = eventPoll();
			bool leave = false;
			if (e != nullptr) {
				if (onEvent(false, e, userData)) {
					if (handleEvent(e)) {
						if (!onEvent(true, e, userData))
							leave = true;
					}
				}
			}
			eventFree(e);
			if (leave)
				return e;
		}
		if (isElapsed || !isSleepy)
			onRender(userData);
		if (isElapsed) {
			onDraw(false, userData);
			getContext()->draw();
			onDraw(true, userData);
		}
		if (!isElapsed && isSleepy) 
			gameEventSleep();
	}
}

void* Display::applicationEventLoop(bool (*isQuitEvent)(void*, void*), void (*onEvent)(void*, void*), void* userData) {
	void *e;
	for (;;) {
		e = eventWait();
		if (e == nullptr)
			continue;
		if (isQuitEvent(e, userData))
			return e;
		if (!handleEvent(e))
			onEvent(e, userData);
		getContext()->draw();
	}
}

