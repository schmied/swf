/*
 * Copyright (c) 2013, 2014, 2015, Michael Schmiedgen
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


#include <iostream>
#include <vector>

#include "Container.hpp"
#include "Display.hpp"
#include "RootContainer.hpp"


/*
 * constructor
 */

Component::Component(Container* p) {
	if (p == nullptr) {
		parent = nullptr;
	} else {
		((Component*) p)->addToContents(this);
		parent = p;
	}
}


/*
 * private
 */

// returns position index of component in parent container
int Component::containerPosition() const {
	if (parent == nullptr)
		return 0;
	const auto contents = parent->contents();
	return std::distance(contents.begin(), std::find(contents.begin(), contents.end(), this));
}


/*
 * protected
 */

Container* Component::getParent() const {
	return parent;
}

RootContainer* Component::rootContainer() {
	auto current = this;
	while (current->parent != nullptr)
		current = current->parent;
	return (RootContainer*) current;
}

void Component::cbDraw(Component &c, void *userData) {
	const Display *display = (Display*) userData;
	const auto parent = c.parent;
	if (parent == nullptr) {
		const RootContainer &rc = (RootContainer&) c;
		if (rc.getDisplay() == nullptr) {
			c.rootContainer()->log("display is null");
			return;
		}
		c.offset = { 0, 0 };
		c.dimension = display->screenDimension();
	} else {
		const int width = parent->dimension.first / parent->contents().size();
		c.offset.first = parent->offset.first + c.containerPosition() * width;
		c.dimension.first = width;
		c.dimension.second = display->fontDimension().second;
	}
	c.onDraw(*display);
}


/*
 * protected: component traversing
 */

void Component::traverse(Component &c, void (*cb)(Component&, void*), void *userData) {
	cb(c, userData);
	traverseChildren(c, cb, userData);
}

void Component::traverse(const Component &c, void (*cb)(const Component&, void*), void *userData) {
	cb(c, userData);
	traverseChildren(c, cb, userData);
}

void Component::traverseChildren(const Component &c, void (*cb)(Component&, void*), void *userData) {
	for (const auto current : c.contents()) {
		cb(*current, userData);
		traverseChildren(*current, cb, userData);
	}
}

void Component::traverseChildren(const Component &c, void (*cb)(const Component&, void*), void *userData) {
	for (const auto current : c.contents()) {
		const Component &cc = static_cast<const Component&>(*current);
		cb(cc, userData);
		traverseChildren(cc, cb, userData);
	}
}

/*
 * public
 */

bool Component::isStateActive() const {
	return false;
}

bool Component::isStateFocus() const {
	return false;
}

