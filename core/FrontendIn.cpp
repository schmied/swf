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

#include "FrontendIn.hpp"

#include "Component.hpp"
#include "Context.hpp"

static const std::basic_string<char> LOG_FACILITY = "FRONTEND_IN";


/*
 * ******************************************************** constructor / destructor
 */

FrontendIn::FrontendIn(Context &ctx) {
	context = &ctx;
	context->setFrontendIn(*this);

/*
	fpsTicksPrevious = 0;
	fpsFrameMillis = 0;
	fpsCyclesPerFrame = 0;
*/
}

FrontendIn::~FrontendIn() {
}


/*
 * ******************************************************** private
 */


/*
 * fps statistics
 */

/*
bool FrontendIn::fpsIsTicksElapsed(const long ticksCurrent, const long targetFps) {
	fpsCyclesPerFrameCounter++;
	// over 2/3 of target millis is elapsed
	if (ticksCurrent - fpsTicksPrevious > 2000 / (3 * targetFps))
		return true;
	return false;
}

void FrontendIn::fpsResetTicks(const long ticksCurrent) {
	if (fpsTicksPrevious)
		fpsFrameMillis = (int) (ticksCurrent - fpsTicksPrevious);
	fpsTicksPrevious = ticksCurrent;
	fpsCyclesPerFrame = fpsCyclesPerFrameCounter;
	fpsCyclesPerFrameCounter = 0;
}
*/

/*
 * event handling
 */

// override e.g. to implement freeing allocated events, defaults to do nothing
void FrontendIn::eventFree(void *event) {
}


/*
 * ******************************************************** public
 */


/*
 * getter
 */

Context* FrontendIn::getContext() const {
	if (context == nullptr)
		std::printf("%s getContext() no context\n", LOG_FACILITY.c_str());
	return context;
}

/*
std::pair<int,int> Display::getFpsStat() const {
	return {fpsFrameMillis, fpsCyclesPerFrame};
}
*/


/*
 * event handling
 */
/*
int FrontendIn::gameEventLoop(const int targetFps, const bool isSleepy, int (*onEvent)(const bool, void*, void*),
	   void (*onRender)(void*), void (*onDraw)(const bool, void*), void* userData) {
	getContext()->log(Context::LOG_INFO, LOG_FACILITY, "gameEventLoop", "entering loop");
	for (;;) {
		const long ticks = gameEventTicks();
		const bool isElapsed = fpsIsTicksElapsed(ticks, targetFps);
		if (isElapsed) {
			fpsResetTicks(ticks);
			void *e = eventPoll();
			int exitCode = 0;
			if (e != nullptr) {
				exitCode = onEvent(false, e, userData);
				if (!exitCode) {
					handleEvent(e);
					exitCode = onEvent(true, e, userData);
				}
			}
			eventFree(e);
			if (exitCode)
				return exitCode;
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

//int Display::applicationEventLoop(bool (*isQuitEvent)(void*, void*), int (*onEvent)(void*, void*), void* userData) {
int FrontendIn::applicationEventLoop(int (*onEvent)(const bool, void*, void*), void* userData) {
	void *e;
	for (;;) {
		e = eventWait();
		if (e == nullptr)
			continue;
		int exitCode = 0;
		exitCode = onEvent(false, e, userData);
		if (!exitCode) {
			handleEvent(e);
			exitCode = onEvent(true, e, userData);
		}
		if (exitCode)
			return exitCode;
		getContext()->draw();
	}
}
*/
