/*
 * Copyright (c) 2013, Michael Schmiedgen
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

#include "Component.hpp"

Component::Component() {
	dimX = 50;
	dimY = 50;
	dimWidth = 100;
	dimHeight = 100;
}

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

bool Component::isStateActive() const {
	return false;
}

bool Component::isStateFocus() const {
	return false;
}

