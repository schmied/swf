/*
 * Copyright (c) 2015, 2016, Michael Schmiedgen
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

#include <iostream>

#include <windows.h>

#include "GdiOut.hpp"

#include "../../core/Context.hpp"


static const std::basic_string<char> LOG_FACILITY = "GDI_OUT";


/*
 * ******************************************************** constructor / destructor
 */

GdiOut::GdiOut(Context &ctx, HWND win) : FrontendOut(ctx) {
	window = win;
	windowContext = GetDC(win);
	font = CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	    NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Courier New");
	if (font == NULL) {
		SWFLOG(getContext(), LOG_WARN, "win32 create font error");
		return;
	}
	SelectObject(windowContext, font);
}

GdiOut::~GdiOut() {
	if (!DeleteObject(font))
		SWFLOG(getContext(), LOG_INFO, "win32 delete object (font) error");
	if (!ReleaseDC(window, windowContext)) 
		SWFLOG(getContext(), LOG_INFO, "win32 release dc error");
	SWFLOG(getContext(), LOG_INFO, nullptr);
}


/*
 * ******************************************************** private
 */


/*
 * ******************************************************** public
 */


 /*
 * getter
 */

HWND GdiOut::getWindow() const {
	return window;
}

HDC GdiOut::getWindowContext() const {
	return windowContext;
}


/*
 * drawing
 */

void GdiOut::draw(const Position &pos, const Style &stl, const std::basic_string<char> &text) const {
//	DHC c;
	PAINTSTRUCT ps; 
//	BeginPaint(window, &ps); 
	TextOut(windowContext, pos.textX, pos.textY, text.c_str(), text.length()); 
//	EndPaint(window, &ps); 
}

std::pair<int,int> GdiOut::screenDimension() const {
	RECT r;
	//GetWindowRect(window, &r);
	GetClientRect(window, &r);
	return {r.right - r.left, r.bottom - r.top};
}

std::pair<int,int> GdiOut::fontDimension() const {
	return {10, 14};
}


/*
 * gdi helper
 */

HWND GdiOut::initWindow(const char *name) {
	//HINSTANCE hInstance = GetModuleHandle(NULL);
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
	HWND w = CreateWindow(name, name, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, 0 /*hInstance*/, NULL);
	if (w == nullptr) {
		messageBox(GetLastError(), "%s initWindow() win32 create window", LOG_FACILITY.c_str());
		return nullptr;
	}
	return w;
}

const static std::size_t bufSize = 1000;
static char buf[bufSize];

int GdiOut::messageBox(const int error, const char *format...) {
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
