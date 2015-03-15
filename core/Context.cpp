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

#include <cstdarg>
#include <string>

#include "Component.hpp"
#include "Container.hpp"
#include "Context.hpp"
#include "Display.hpp"


static const std::basic_string<char> LOG_FACILITY = "CONTEXT";

/*
 * ******************************************************** constructor / destructor
 */

Context::Context() {	
	log(LOG_DEBUG, LOG_FACILITY, "<init>", nullptr);
	display = nullptr;
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
	Display *display = (Display*) userData;
	if (display == nullptr) {
		std::printf("%s onDraw() no display\n", LOG_FACILITY.c_str());
		return;
	}
	c->onDraw(display);
}


/*
 * ******************************************************** public
 */


/*
 * getter / setter
 */

const Display* Context::getDisplay() {
	if (display == nullptr)
		log(LOG_WARN, LOG_FACILITY, "getDisplay", "no display");
	return display;
}

void Context::setDisplay(Display &d) {
	display = &d;
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
	Component::traverse((Component*) rootContainer, Context::onDraw, display);

	if (display != nullptr) {
		const std::pair<int,int> fontDimension = display->fontDimension();
		const std::pair<int,int> screenDimension = display->screenDimension();
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
			for (const auto log : logs) {
				//display->draw(logOffset, logDimension, *log);
				display->draw(&pos, *log);
				pos.y += fontDimension.second;
				pos.textY = pos.y;
			}
		}
		// draw fps stats
		const std::pair<int,int> frameStat = display->getFpsStat();
		if (frameStat.first > 0 && frameStat.second > 0) {
			char buf[100];
			std::snprintf(buf, 100, "%7dcycl %3dms %3dfps", frameStat.second, frameStat.first, 1000 / frameStat.first);
			const int w = std::strlen(buf) * fontDimension.first;
			const int x = screenDimension.first - w - fontDimension.first;
			const int y = screenDimension.second - 2 * fontDimension.second;
//			const std::pair<int,int> statDimension { width, fontDimension.second };
	//		display->draw(statOffset, statDimension, buf);
			const Position pos {x, y, w, fontDimension.second, x, y};
			display->draw(&pos, buf);
		}
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

	if (display == nullptr || level == Context::LOG_WARN)
		std::printf("%s\n", s->c_str());
}

