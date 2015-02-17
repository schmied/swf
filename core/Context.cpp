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

#include "Component.hpp"
#include "Container.hpp"
#include "Context.hpp"
#include "Display.hpp"


static const std::basic_string<char> LOG_FACILITY = "CONTEXT";

/*
 * constructor / destructor
 */

Context::Context() {	
	logDebug(LOG_FACILITY, "<init>");
	display = nullptr;
	rootContainer = nullptr;
}

Context::~Context() {
//	for (auto log : logs)
//		delete log
	logInfo(LOG_FACILITY, "<free>");
}


/*
 * private
 */

const static std::size_t bufSize = 1000;
char buf[bufSize];

void Context::log(const int level, const std::basic_string<char> &facility, const std::basic_string<char> &functionName, const char *format...) {
	std::basic_string<char> *s = new std::basic_string<char>();
//	std::memset(buf, 0, bufSize);
	std::snprintf(buf, bufSize, "%d", level);
	s->append(buf);
//	std::memset(buf, 0, bufSize);
	std::snprintf(buf, bufSize, " %-16.16s", facility.c_str());
	s->append(buf);
//	std::memset(buf, 0, bufSize);
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
	std::fflush(stdout);

	logs.push_back(s);
	if (logs.size() > 20)
		logs.pop_front();
}

void Context::onDraw(Component *c, void *userData) {
	Display *display = (Display*) userData;
	if (display == nullptr) {
		std::printf("%s onDraw() no display\n", LOG_FACILITY.c_str());
		return;
	}
	display->getContext()->logDebug(LOG_FACILITY, "onDraw");
	c->onDraw(display);
}

/*
 * public
 */

const Display* Context::getDisplay() {
	if (display == nullptr)
		logWarn(LOG_FACILITY, "getDisplay", "no display");
	return display;
}

void Context::setDisplay(Display *d) {
	display = d;
	if (display == nullptr)
		logWarn(LOG_FACILITY, "setDisplay", "no display");
}

const Container* Context::getRootContainer() {
	if (rootContainer == nullptr)
		logWarn(LOG_FACILITY, "getRootContainer", "no root container");
	return rootContainer;
}

void Context::setRootContainer(Container *r) {
	rootContainer = r;
	if (rootContainer == nullptr)
		logWarn(LOG_FACILITY, "setRootContainer", "no root container");
}

void Context::draw() {
	logDebug(LOG_FACILITY, "draw");
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

enum LogLevel { LOG_DEBUG, LOG_INFO, LOG_WARN };

void Context::logDebug(const std::basic_string<char> &facility, const std::basic_string<char> &functionName, const char *format...) {
	log(LOG_DEBUG, facility, functionName, format);
}

void Context::logDebug(const std::basic_string<char> &facility, const std::basic_string<char> &functionName) {
	log(LOG_DEBUG, facility, functionName, nullptr);
}

void Context::logInfo(const std::basic_string<char> &facility, const std::basic_string<char> &functionName, const char *format...) {
	log(LOG_INFO, facility, functionName, format);
}

void Context::logInfo(const std::basic_string<char> &facility, const std::basic_string<char> &functionName) {
	log(LOG_INFO, facility, functionName, nullptr);
}

void Context::logWarn(const std::basic_string<char> &facility, const std::basic_string<char> &functionName, const char *format...) {
	log(LOG_WARN, facility, functionName, format);
}

void Context::logWarn(const std::basic_string<char> &facility, const std::basic_string<char> &functionName) {
	log(LOG_WARN, facility, functionName, nullptr);
}

