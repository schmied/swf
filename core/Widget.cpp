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

#include "Display.hpp"
#include "RootContainer.hpp"


// empty vector for contents()
static const std::vector<Component*> noContents {};


/*
 * private
 */

void Widget::addToContents(Component *c) {
	rootContainer()->log("cannot add contents");
}

void Widget::onDraw(const Display &display) const {
	display.drawBorder(offset, dimension);
	display.drawText(offset, "1234abcd");
}


/*
 * public
 */

// no contents by default
std::vector<Component*> Widget::contents() const {
	return noContents;
}


