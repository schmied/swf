/*
 * Copyright (c) 2014, Michael Schmiedgen
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

//#include "Display.hpp"

/*
RootContainer::RootContainer() : Container(nullptr) {
}
*/

void RootContainer::traverseDraw() {
	traverse(*this, Component::cbDraw);
}

/*
void RootContainer::cbRegisterDisplay(const Component &c) {
	c.display = c.parent->display;
}
*/

void RootContainer::traverseRegisterDisplay(Display *d) {
	display = d;
	traverseChildren(*this, Component::cbRegisterDisplay);
}

/*
void RootContainer::cbUnregisterDisplay(const Component &c) {
	c.display = nullptr;
}
*/

void RootContainer::traverseUnregisterDisplay() {
	traverse(*this, Component::cbUnregisterDisplay);
}

