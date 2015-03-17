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

#ifndef SWF_CORE_DISPLAY_GDI
#define SWF_CORE_DISPLAY_GDI

#include <windows.h>

#include "Display.hpp"

class Component;

class DisplayGdi : public Display {

private:
	HWND window;
	HDC windowContext;

	// event handling
	LPMSG currentEvent;
	void* eventPoll() override;
	void* eventWait() override;
	void gameEventSleep() const override;
	long gameEventTicks() const override;

public:
	DisplayGdi(Context&, HWND);
	~DisplayGdi();

	// drawing
//	void drawBorder(const std::pair<int,int>&, const std::pair<int,int>&) const override;
//	void drawText(const std::pair<int,int>&, const std::pair<int,int>&, const std::basic_string<char>&) const override;
//	void draw(const std::pair<int,int>&, const std::pair<int,int>&, const std::basic_string<char>&) const override;
	void draw(const Position&, const Style&, const std::basic_string<char>&) const override;
	std::pair<int,int> screenDimension() const override;
	std::pair<int,int> fontDimension() const override;

	// event handling
	void handleEvent(void*) const override;

	// gdi helper
	static HWND initWindow();

};

#endif // SWF_CORE_DISPLAY_CURSES

