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

#include "Widget.hpp"


#include <iostream>

#include "Context.hpp"
#include "Display.hpp"


static const std::basic_string<char> LOG_FACILITY = "WIDGET";

// empty vector for contents()
static std::vector<Component*> noContents {};


/*
 * constructor / destructor
 */

Widget::Widget(Container *c) : Component(c) {
	getContext()->logDebug(LOG_FACILITY, "<init>");
}

Widget::~Widget() {
	getContext()->logInfo(LOG_FACILITY, "<free>");
}


/*
 * private
 */

void Widget::addToContents(Component *c) {
	getContext()->logWarn(LOG_FACILITY, "addToContents", "cannot add contents to a widget");
}

void Widget::onDraw(const Display *display) {
	std::pair<int,int> *offset = getOffset();
	std::pair<int,int> *dimension = getDimension();
	display->drawBorder(*offset, *dimension);
	display->drawText(*offset, "blaa");
}


/*
 * public
 */

// no contents by default
std::vector<Component*>* Widget::contents() {
	return &noContents;
}

