/*
 * Copyright (c) 2013, Michael Schmiedgen
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

//#include <cstddef>
#include <cstdarg>
#include <string>

#include "Component.hpp"
#include "Container.hpp"
#include "Context.hpp"
#include "Display.hpp"


static const std::basic_string<char> LOG_FACILITY = "CONTEXT";

/*
 * constructor / destructor
 */

Context::Context() {	
	log(LOG_DEBUG, LOG_FACILITY, "<init>", nullptr);
	display = nullptr;
	rootContainer = nullptr;
}

Context::~Context() {
//	for (auto log : logs)
//		delete log
	log(LOG_DEBUG, LOG_FACILITY, "<free>", nullptr);
}


/*
 * private
 */

void Context::onDraw(Component *c, void *userData) {
	Display *display = (Display*) userData;
	if (display == nullptr) {
		std::printf("%s onDraw() no display\n", LOG_FACILITY.c_str());
		return;
	}
//	display->getContext()->logDebug(LOG_FACILITY, "onDraw");
	c->onDraw(display);
}


/*
 * public
 */

const Display* Context::getDisplay() {
	if (display == nullptr)
		log(LOG_WARN, LOG_FACILITY, "getDisplay", "no display");
	return display;
}

void Context::setDisplay(Display *d) {
	display = d;
	if (display == nullptr)
		log(LOG_WARN, LOG_FACILITY, "setDisplay", "no display");
}

const Container* Context::getRootContainer() {
	if (rootContainer == nullptr)
		log(LOG_WARN, LOG_FACILITY, "getRootContainer", "no root container");
	return rootContainer;
}

void Context::setRootContainer(Container *r) {
	rootContainer = r;
	if (rootContainer == nullptr)
		log(LOG_WARN, LOG_FACILITY, "setRootContainer", "no root container");
}

void Context::draw() {
//	logDebug(LOG_FACILITY, "draw");
	Component::traverse((Component*) rootContainer, Context::onDraw, display);

	// draw log
	if (logs.size() > 0) {
		const int fontHeight = display->fontDimension().second;
		// 1 char left padding to screen
		const int xOffset = display->fontDimension().first;
		// 1 char bottom padding to screen
		int yOffset = display->screenDimension().second - (1 + logs.size()) * fontHeight;
		for (const auto log : logs) {
			display->drawText({xOffset, yOffset}, *log);
			yOffset += fontHeight;
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

	std::printf("%s\n", s->c_str());

	logs.push_back(s);
}

