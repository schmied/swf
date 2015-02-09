/*
 * Copyright (c) 2014, 2015, Michael Schmiedgen
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

#include "RootContainer.hpp"


#include <iostream>

#include "Display.hpp"

RootContainer::RootContainer(Display *d) : Container(nullptr) {
	display = d;
//	logs {};
}

Display* RootContainer::getDisplay() const {
	return display;
}

void RootContainer::draw() {
	traverse(*this, Component::cbDraw, display);

	// draw log
	if (logs.size() > 0) {
		const int fontHeight = display->fontDimension().second;
		// 1 char left padding to screen
		const int xOffset = display->fontDimension().first;
		// 1 char bottom padding to screen
		int yOffset = display->screenDimension().second - (1 + logs.size()) * fontHeight;
		for (const auto log : logs) {
			display->drawText({xOffset, yOffset}, log);
			yOffset += fontHeight;
		}
	}
}

void RootContainer::onDraw(const Display &display) const {
//	display.drawBorder(offset, dimension);
}

void RootContainer::log(const std::basic_string<char> &s) {
	logs.push_front(s);
	if (logs.size() > 10)
		logs.pop_back();
}

