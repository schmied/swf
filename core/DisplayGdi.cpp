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

#include <windows.h>

#include "Component.hpp"
#include "Context.hpp"


static const std::basic_string<char> LOG_FACILITY = "DISPLAY_GDI";


/*
 * ******************************************************** constructor / destructor
 */

DisplayGdi::DisplayGdi(Context &c, HWND win) : Display(c) {
	window = win;
	windowContext = GetDC(win);
	font = CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	    NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Courier New");
	if (font == NULL) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "win32 create font error");
		return;
	}
	SelectObject(windowContext, font);
}

DisplayGdi::~DisplayGdi() {
	if (!DeleteObject(font))
		getContext()->log(Context::LOG_INFO, LOG_FACILITY, "<free>", "win32 delete object (font) error");
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
	if (!PeekMessage(currentEvent, window, 0, 0, 0))
		return nullptr;
	getContext()->log(Context::LOG_WARN, LOG_FACILITY, "eventPoll", "win32 get message error:");
	TranslateMessage(currentEvent);
	return &currentEvent;
}

void* DisplayGdi::eventWait() {
	if (GetMessage(currentEvent, window, 0, 0) == -1) {
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

void DisplayGdi::draw(const Position &pos, const Style &stl, const std::basic_string<char> &text) const {
//	DHC c;
	PAINTSTRUCT ps; 
	BeginPaint(window, &ps); 
	TextOut(windowContext, pos.textX, pos.textY, text.c_str(), text.length()); 
	EndPaint(window, &ps); 
}

std::pair<int,int> DisplayGdi::screenDimension() const {
	RECT r;
	GetWindowRect(window, &r);
	return {r.right - r.left, r.bottom - r.top - 50};
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

HWND DisplayGdi::initWindow(HINSTANCE hInstance, const char *name) {
	WNDCLASSEX wcex;
	wcex.cbSize         = sizeof(WNDCLASSEX);
	wcex.style          = CS_NOCLOSE;
	wcex.lpfnWndProc    = DefWindowProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = 0;//hInstance;
	wcex.hIcon          = NULL;
	wcex.hCursor        = NULL;//LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = NULL;//GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = name;
	wcex.hIconSm        = NULL;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	if (!RegisterClassEx(&wcex)) {
		messageBox(GetLastError(), "%s initWindow() win32 register class ex", LOG_FACILITY.c_str());
		return nullptr;
	}
	HWND w = CreateWindow(name, name, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, 0 /*hinstance*/, NULL);
	if (w == nullptr) {
		messageBox(GetLastError(), "%s initWindow() win32 create window", LOG_FACILITY.c_str());
		return nullptr;
	}
	return w;
}

const static std::size_t bufSize = 1000;
static char buf[bufSize];

int DisplayGdi::messageBox(const int error, const char *format...) {
	std::basic_string<char> s;
	if (format != nullptr) {
		va_list arg;
		va_start(arg, format);
		std::vsnprintf(buf, bufSize, format, arg);
		s.append(buf);
		va_end(arg);
	}
	if (error != 0) {
		char errorBuf[bufSize];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, LANG_NEUTRAL, errorBuf, bufSize, nullptr);
		std::snprintf(buf, bufSize, " error: %s (%d)", errorBuf, error);
		s.append(buf);
	}
	int id = MessageBox(NULL, s.c_str(), "warning", MB_OK);
	switch (id) {
	case IDCANCEL:
		break;
	case IDTRYAGAIN:
		break;
	case IDCONTINUE:
		break;
    }
    return id;
}
