/*
 * Copyright (c) 2013, 2014, Michael Schmiedgen
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

#include <vector>

#include "Component.hpp"

/*
unsigned short Component::getDimHeight() const {
	return dimHeight;
}

unsigned short Component::getDimWidth() const {
	return dimWidth;
}

unsigned short Component::getDimX() const {
	return dimX;
}

unsigned short Component::getDimY() const {
	return dimY;
}
*/

Display* Component::getDisplay() const {
	return display;
}

bool Component::isStateActive() const {
	return false;
}

bool Component::isStateFocus() const {
	return false;
}

/*
void Component::onDraw() const {
}
*/

static const std::vector<Component*> emptyVector {};

std::vector<Component*> Component::getContents() const {
	return emptyVector;
}

void Component::traverse(const Component &c, void (*cb)(const Component &c)) {
	cb(c);
	for (auto current : c.getContents()) {
		traverse(*current, cb);
	}
}

