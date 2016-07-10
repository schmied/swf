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
#ifdef _WINDOWS
#include "../frontend/out/GdiOut.hpp"
#endif


static const std::basic_string<char> LOG_FACILITY = "CONTEXT";


/*
 * ******************************************************** constructor / destructor
 */

Context::Context() {	
	SWFLOG(this, LOG_DEBUG, nullptr);
	frontendIn = nullptr;
	frontendOut = nullptr;
	rootContainer = nullptr;
}

Context::~Context() {
//	for (auto log : logs)
//		delete log
	SWFLOG(this, LOG_WARN, nullptr);
}


/*
 * ******************************************************** private
 */


/*
 * drawing
 */

 void Context::drawComponents() {
	Component::traverse((Component*) rootContainer, Context::onDrawComponent, frontendOut);

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
		const std::pair<int,int> frameStat = getFpsStat();
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

TraverseCondition Context::onDrawComponent(Component *c, void *userData) {
	FrontendOut *out = (FrontendOut*) userData;
	if (out == nullptr) {
		std::printf("%s onDraw() no frontend\n", LOG_FACILITY.c_str());
		return returnCurrent;
	}
	c->onDraw(out);
	return continueTraverse;
}


/*
 * fps statistics
 */

bool Context::fpsIsTicksElapsed(const long ticksCurrent, const long targetFps) {
	fpsCyclesPerFrameCounter++;
	// over 2/3 of target millis is elapsed
	if (ticksCurrent - fpsTicksPrevious > 2000 / (3 * targetFps))
		return true;
	return false;
}

void Context::fpsResetTicks(const long ticksCurrent) {
	if (fpsTicksPrevious)
		fpsFrameMillis = (int) (ticksCurrent - fpsTicksPrevious);
	fpsTicksPrevious = ticksCurrent;
	fpsCyclesPerFrame = fpsCyclesPerFrameCounter;
	fpsCyclesPerFrameCounter = 0;
}


/*
 * ******************************************************** public
 */


/*
 * getter / setter
 */

std::pair<int,int> Context::getFpsStat() const {
	return { fpsFrameMillis, fpsCyclesPerFrame };
}

const FrontendIn* Context::getFrontendIn() {
	if (frontendIn == nullptr)
		SWFLOG(this, LOG_WARN, "no frontendIn");
	return frontendIn;
}

void Context::setFrontendIn(FrontendIn *in) {
	frontendIn = in;
}

const FrontendOut* Context::getFrontendOut() {
	if (frontendOut == nullptr)
		SWFLOG(this, LOG_WARN, "no frontendOut");
	return frontendOut;
}

void Context::setFrontendOut(FrontendOut *out) {
	frontendOut = out;
	rootContainer->invalidatePosition();
}

const Container* Context::getRootContainer() {
	if (rootContainer == nullptr)
		SWFLOG(this, LOG_WARN, "no root container");
	return rootContainer;
}

void Context::setRootContainer(Container *rc) {
	rootContainer = rc;
}


/*
 * event
 */

void Context::eventClick(const int, const int) {
}

void Context::eventKey(const int) {
}


/*
 * loop
 */

int Context::gameLoop(const int targetFps, const bool isSleepy, int (*onEvent)(void*, void*),
	   void (*onRender)(void*), void (*onDraw)(void*), void* userData) {
	SWFLOG(this, LOG_INFO, "enter loop");

	if (frontendIn == nullptr) {
		SWFLOG(this, LOG_WARN, "no IN-frontend defined");
		return -1;
	}
	if (frontendOut == nullptr) {
		SWFLOG(this, LOG_WARN, "no OUT-frontend defined");
		return -1;
	}

	fpsTicksPrevious = 0;
	fpsFrameMillis = 0;
	fpsCyclesPerFrame = 0;

	for (;;) {
		const long ticks = frontendIn->gameLoopTicks();
		const bool isElapsed = fpsIsTicksElapsed(ticks, targetFps);
		if (isElapsed) {
			fpsResetTicks(ticks);
			void *e = frontendIn->eventPoll();
			int exitCode = 0;
			if (e != nullptr) {
				exitCode = onEvent(e, userData);
				if (!exitCode)
					frontendIn->in(e);
			}
			frontendIn->eventFree(e);
			if (exitCode)
				return exitCode;
		}
		if (isElapsed || !isSleepy)
			onRender(userData);
		if (isElapsed) {
			onDraw(userData);
			drawComponents();
			frontendOut->gameLoopDrawFinish();
		}
		if (!isElapsed && isSleepy) 
			frontendIn->gameLoopSleep();
	}
}

int Context::applicationLoop(int (*onEvent)(const bool, void*, void*), void* userData) {
	void *e;
	for (;;) {
		e = frontendIn->eventWait();
		if (e == nullptr)
			continue;
		int exitCode = 0;
		exitCode = onEvent(false, e, userData);
		if (!exitCode) {
			frontendIn->in(e);
			exitCode = onEvent(true, e, userData);
		}
		if (exitCode)
			return exitCode;
		drawComponents();
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
	if (frontendOut == nullptr && level == Context::LOG_WARN)
		GdiOut::messageBox(0, s->c_str());
#endif
}
