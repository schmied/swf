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

#ifndef SWT_CORE_COMPONENT
#define SWT_CORE_COMPONENT

#include <vector>

class Context;

class Component {

private:
	Context *context;
	Component *parent;
	unsigned short dimX;
	unsigned short dimY;
	unsigned short dimWidth;
	unsigned short dimHeight;
	std::vector<Component> contents;

public:
	Component();
	unsigned short getDimHeight() const;
	unsigned short getDimWidth() const;
	unsigned short getDimX() const;
	unsigned short getDimY() const;
	bool isStateActive() const;
	bool isStateFocus() const;
};

#endif // SWT_CORE_COMPONENT

