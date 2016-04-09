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

#include "GdiIn.hpp"

//#include "Component.hpp"
#include "../../core/Context.hpp"


static const std::basic_string<char> LOG_FACILITY = "GDI_IN";


/*
 * ******************************************************** constructor / destructor
 */

GdiIn::GdiIn(Context &ctx, HWND win) : FrontendIn(ctx) {
	window = win;
/*
	windowContext = GetDC(win);
	font = CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	    NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Courier New");
	if (font == NULL) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "win32 create font error");
		return;
	}
	SelectObject(windowContext, font);
*/
}

GdiIn::~GdiIn() {
/*
	if (!DeleteObject(font))
		getContext()->log(Context::LOG_INFO, LOG_FACILITY, "<free>", "win32 delete object (font) error");
	if (!ReleaseDC(window, windowContext)) 
		getContext()->log(Context::LOG_INFO, LOG_FACILITY, "<free>", "win32 release dc error");
*/
	getContext()->log(Context::LOG_INFO, LOG_FACILITY, "<free>", nullptr);
}


/*
 * ******************************************************** private
 */


/*
 * event handling
 */

void* GdiIn::eventPoll() {
	if (!PeekMessage(&currentEvent, window, 0, 0, PM_REMOVE))
		return nullptr;
	TranslateMessage(&currentEvent);
	return &currentEvent;
}

void* GdiIn::eventWait() {
	if (GetMessage(&currentEvent, window, 0, 0) == -1) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "eventWait", "win32 get message error: %d", GetLastError());
		return nullptr;
	}
	TranslateMessage(&currentEvent);
	return &currentEvent;
}

void GdiIn::gameEventSleep() const {
	Sleep(1);
}

long GdiIn::gameEventTicks() const {
	return GetTickCount64();
}


/*
 * ******************************************************** public
 */


 /*
 * getter
 */

HWND GdiIn::getWindow() const {
	return window;
}


/*
 * event handling
 */

void GdiIn::handleEvent(void *event) const {
	if (event == nullptr)
		return;
	const MSG *e = (const MSG*) event;
	switch (e->message) {
	case WM_KEYDOWN:
		switch (e->wParam) {
		case VK_RETURN:
			break;
		default:
			break;
		}
		getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "key %d %d", e->lParam, e->wParam);
		break;
	default:
		break;
	}
}
