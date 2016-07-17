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
#include <windowsx.h>

#include "GdiIn.hpp"

#include "../../core/Context.hpp"


static const std::basic_string<char> LOG_FACILITY = "GDI_IN";


/*
 * ******************************************************** constructor / destructor
 */

GdiIn::GdiIn(Context &ctx, HWND win) : FrontendIn(ctx) {
	window = win;
}

GdiIn::~GdiIn() {
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

HWND GdiIn::getWindow() const {
	return window;
}


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
		SWFLOG(getContext(), LOG_WARN, "win32 get message error: %d", GetLastError());
		return nullptr;
	}
	TranslateMessage(&currentEvent);
	return &currentEvent;
}

void GdiIn::in(void *event) const {
	if (event == nullptr)
		return;
	const MSG *e = (const MSG*) event;
	switch (e->message) {
	case WM_KEYDOWN:
		SWFLOG(getContext(), LOG_DEBUG, "key %c %d", e->wParam, e->wParam);
		switch (e->wParam) {
		case VK_RETURN:
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		SWFLOG(getContext(), LOG_DEBUG, "click %dx%d", GET_X_LPARAM(e->lParam), GET_Y_LPARAM(e->lParam));
		break;
	case WM_MOUSEMOVE:
//		SWFLOG(getContext(), LOG_DEBUG, "move %dx%d", GET_X_LPARAM(e->lParam), GET_Y_LPARAM(e->lParam));
		break;
	default:
		break;
	}
}


/*
 * game loop
 */

void GdiIn::gameLoopSleep() const {
	Sleep(1);
}

long GdiIn::gameLoopTicks() const {
	return GetTickCount64();
}
