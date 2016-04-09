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

#include <cstdarg>
#include <string>

#include "Context.hpp"

#include "Component.hpp"
#include "Container.hpp"
#include "FrontendIn.hpp"
#include "FrontendOut.hpp"
/*
#ifdef _WINDOWS
#include "DisplayGdi.hpp"
#endif
*/


static const std::basic_string<char> LOG_FACILITY = "CONTEXT";

/*
 * ******************************************************** constructor / destructor
 */

Context::Context() {	
	log(LOG_DEBUG, LOG_FACILITY, "<init>", nullptr);
	frontendIn = nullptr;
	frontendOut = nullptr;
	rootContainer = nullptr;
}

Context::~Context() {
//	for (auto log : logs)
//		delete log
	log(LOG_WARN, LOG_FACILITY, "<free>", nullptr);
}


/*
 * ******************************************************** private
 */


/*
 * drawing
 */

void Context::onDraw(Component *c, void *userData) {
	FrontendOut *out = (FrontendOut*) userData;
	if (out == nullptr) {
		std::printf("%s onDraw() no frontend\n", LOG_FACILITY.c_str());
		return;
	}
	c->onDraw(out);
}


/*
 * ******************************************************** public
 */


/*
 * getter / setter
 */

const FrontendIn* Context::getFrontendIn() {
	if (frontendIn == nullptr)
		log(LOG_WARN, LOG_FACILITY, "getFrontendIn", "no frontendIn");
	return frontendIn;
}

void Context::setFrontendIn(FrontendIn &in) {
	frontendIn = &in;
	rootContainer->invalidatePosition();
}

const FrontendOut* Context::getFrontendOut() {
	if (frontendOut == nullptr)
		log(LOG_WARN, LOG_FACILITY, "getFrontendOut", "no frontendOut");
	return frontendOut;
}

void Context::setFrontendOut(FrontendOut &out) {
	frontendOut = &out;
	rootContainer->invalidatePosition();
}

const Container* Context::getRootContainer() {
	if (rootContainer == nullptr)
		log(LOG_WARN, LOG_FACILITY, "getRootContainer", "no root container");
	return rootContainer;
}

void Context::setRootContainer(Container &r) {
	rootContainer = &r;
}


/*
 * drawing
 */

void Context::draw() {
	Component::traverse((Component*) rootContainer, Context::onDraw, frontendOut);

	if (frontendOut != nullptr) {
		const std::pair<int,int> fontDimension = frontendOut->fontDimension();
		const std::pair<int,int> screenDimension = frontendOut->screenDimension();
		// draw log
		if (logs.size() > 0) {
//			const std::pair<int,int> logDimension { screenDimension.first / 2 - 2 * fontDimension.first,
//			    fontDimension.second };
//			std::pair<int,int> logOffset;
			// 1 char left padding to screen
			const int x = 1 * fontDimension.first;
			// 1 char bottom padding to screen
			const int y = screenDimension.second - (1 + logs.size()) * fontDimension.second;
			Position pos {x, y, screenDimension.first / 2 - 2 * fontDimension.first, fontDimension.second, x, y};
			Style stl {0, 0};
			for (const auto log : logs) {
				//display->draw(logOffset, logDimension, *log);
				frontendOut->draw(pos, stl, *log);
				pos.y += fontDimension.second;
				pos.textY = pos.y;
			}
		}
		// draw fps stats
		const std::pair<int,int> frameStat = frontendOut->getFpsStat();
		if (frameStat.first > 0 && frameStat.second > 0) {
			char buf[100];
			std::snprintf(buf, 100, "%7dcycl %3dms %3dfps", frameStat.second, frameStat.first, 1000 / frameStat.first);
			const int w = std::strlen(buf) * fontDimension.first;
			const int x = screenDimension.first - w - fontDimension.first;
			const int y = screenDimension.second - 2 * fontDimension.second;
//			const std::pair<int,int> statDimension { width, fontDimension.second };
	//		display->draw(statOffset, statDimension, buf);
			const Position pos {x, y, w, fontDimension.second, x, y};
			Style stl {0, 0};
			frontendOut->draw(pos, stl, buf);
		}
	}
}


/*
 * loop
 */

int Context::gameEventLoop(const int targetFps, const bool isSleepy, int (*onEvent)(const bool, void*, void*),
	   void (*onRender)(void*), void (*onDraw)(const bool, void*), void* userData) {
	log(Context::LOG_INFO, LOG_FACILITY, "gameEventLoop", "entering loop");
	for (;;) {
		const long ticks = frontendIn->gameEventTicks();
		const bool isElapsed = frontendOut->fpsIsTicksElapsed(ticks, targetFps);
		if (isElapsed) {
			frontendOut->fpsResetTicks(ticks);
			void *e = frontendIn->eventPoll();
			int exitCode = 0;
			if (e != nullptr) {
				exitCode = onEvent(false, e, userData);
				if (!exitCode) {
					frontendIn->handleEvent(e);
					exitCode = onEvent(true, e, userData);
				}
			}
			frontendIn->eventFree(e);
			if (exitCode)
				return exitCode;
		}
		if (isElapsed || !isSleepy)
			onRender(userData);
		if (isElapsed) {
			onDraw(false, userData);
			draw();
			onDraw(true, userData);
		}
		if (!isElapsed && isSleepy) 
			frontendIn->gameEventSleep();
	}
}

//int Display::applicationEventLoop(bool (*isQuitEvent)(void*, void*), int (*onEvent)(void*, void*), void* userData) {
int Context::applicationEventLoop(int (*onEvent)(const bool, void*, void*), void* userData) {
	void *e;
	for (;;) {
		e = frontendIn->eventWait();
		if (e == nullptr)
			continue;
		int exitCode = 0;
		exitCode = onEvent(false, e, userData);
		if (!exitCode) {
			frontendIn->handleEvent(e);
			exitCode = onEvent(true, e, userData);
		}
		if (exitCode)
			return exitCode;
		draw();
	}
}



/*
 * logging
 */

const static std::size_t bufSize = 1000;
static char buf[bufSize];

void Context::log(const int level, const std::basic_string<char> &facility, const std::basic_string<char> &functionName, const char *format...) {

	std::basic_string<char> *s;
	if (logs.size() > 20) {
		s = logs.front();
		s->clear();
		logs.pop_front();
	} else {
		s = new std::basic_string<char>();
	}

	std::snprintf(buf, bufSize, "%d", level);
	s->append(buf);
	std::snprintf(buf, bufSize, " %-16.16s", facility.c_str());
	s->append(buf);
	std::snprintf(buf, bufSize, " %-24.24s", functionName.c_str());
	s->append(buf);

	if (format != nullptr) {
		va_list arg;
		va_start(arg, format);
		std::vsnprintf(buf, bufSize, format, arg);
		s->append(buf);
		va_end(arg);
	}

	logs.push_back(s);

	if (frontendOut == nullptr || level == Context::LOG_WARN)
		std::printf("%s\n", s->c_str());
#ifdef _WINDOWS
	if (display == nullptr && level == Context::LOG_WARN)
		DisplayGdi::messageBox(0, s->c_str());
#endif
}

