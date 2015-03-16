/*
 * Copyright (c) 2015, Michael Schmiedgen
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

#include "DisplayGdi.hpp"

#include <iostream>

#include "Component.hpp"
#include "Context.hpp"


static const std::basic_string<char> LOG_FACILITY = "DISPLAY_GDI";


/*
 * ******************************************************** constructor / destructor
 */

DisplayGdi::DisplayGdi(Context &c, HWND win) : Display(c) {
	window = win;
	windowContext = GetDC(win);
}

DisplayGdi::~DisplayGdi() {
	if (!ReleaseDC(window, windowContext)) 
		getContext()->log(Context::LOG_INFO, LOG_FACILITY, "<free>", "win32 release dc error");
	getContext()->log(Context::LOG_INFO, LOG_FACILITY, "<free>", nullptr);
}


/*
 * ******************************************************** private
 */


/*
 * event handling
 */

void* DisplayGdi::eventPoll() {
	if (!PeekMessage(currentEvent, NULL, 0, 0, 0))
		return nullptr;
	TranslateMessage(currentEvent);
	return &currentEvent;
}

void* DisplayGdi::eventWait() {
	if (GetMessage(currentEvent, NULL, 0, 0) == -1) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "eventWait", "win32 get message error: %d", GetLastError());
		return nullptr;
	}
	TranslateMessage(currentEvent);
	return &currentEvent;
}

void DisplayGdi::gameEventSleep() const {
	Sleep(1);
}

long DisplayGdi::gameEventTicks() const {
	return GetTickCount();
}


/*
 * ******************************************************** public
 */


/*
 * drawing
 */

void DisplayGdi::draw(const Position *pos, const std::basic_string<char> &text) const {
//	DHC c;
	PAINTSTRUCT ps; 
	BeginPaint(window, &ps); 
	TextOut(windowContext, 10, 10, text.c_str(), text.length()); 
	EndPaint(window, &ps); 
}

std::pair<int,int> DisplayGdi::screenDimension() const {
	//return {GetSystemMetrics(), GetSystemMetrics()};
	return {500, 500};
}

std::pair<int,int> DisplayGdi::fontDimension() const {
	return {10, 14};
}


/*
 * event handling
 */

void DisplayGdi::handleEvent(void *event) const {
	if (event == nullptr)
		return;
	const LPMSG e = *(const LPMSG*) event;
//	getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "char %d", c);
	switch (e->lParam) {
	default:
		break;
	}
}


/*
 * gdi helper
 */

HWND DisplayGdi::initWindow() {
	HWND w = CreateWindow(NULL, NULL, 0, 100, 100, 600, 600, 0, 0, 0, 0);
	if (w == nullptr) {
		std::printf("%s initWindow() win32 create window error: %s\n", LOG_FACILITY.c_str(), GetLastError());
		return nullptr;
	}
	return w;
}
