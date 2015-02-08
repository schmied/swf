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

Component::Component(Container* p) {
	if (p == nullptr) // root container?
		init(nullptr);
	else
		p->addComponent(this);
}

void Component::init(Container *p) {
	parent = p;
}

Container* Component::getParent() const {
	return parent;
}

RootContainer* Component::rootContainer() {
	auto current = this;
	while (current->parent != nullptr)
		current = current->parent;
	return (RootContainer*) current;
}

int Component::containerPosition() const {
	if (parent == nullptr)
		return 0;
	const auto contents = parent->getContents();
	return std::distance(contents.begin(), std::find(contents.begin(), contents.end(), this));
}

bool Component::isStateActive() const {
	return false;
}

bool Component::isStateFocus() const {
	return false;
}

static const std::vector<Component*> emptyVector {};

std::vector<Component*> Component::getContents() const {
	return emptyVector;
}

void Component::traverse(Component &c, void (*cb)(Component&, void*), void *userData) {
	cb(c, userData);
	traverseChildren(c, cb, userData);
}

void Component::traverse(const Component &c, void (*cb)(const Component&, void*), void *userData) {
	cb(c, userData);
	traverseChildren(c, cb, userData);
}

void Component::traverseChildren(const Component &c, void (*cb)(Component&, void*), void *userData) {
	for (const auto current : c.getContents()) {
		cb(*current, userData);
		traverseChildren(*current, cb, userData);
	}
}

void Component::traverseChildren(const Component &c, void (*cb)(const Component&, void*), void *userData) {
	for (const auto current : c.getContents()) {
		const Component &cc = static_cast<const Component&>(*current);
		cb(cc, userData);
		traverseChildren(cc, cb, userData);
	}
}

void Component::cbDraw(Component &c, void *userData) {
	const Display *display = (Display*) userData;
	const auto parent = c.parent;
	if (parent == nullptr) {
//		std::cout << " parent null " << std::endl;
		const RootContainer &rc = (RootContainer&) c;
		if (rc.getDisplay() == nullptr) {
			// XXX log
			std::cout << " display is null " << std::endl;
			return;
		}
		c.offset = { 0, 0 };
		c.dimension = display->getDimension();
	} else {
		const int width = parent->dimension.first / parent->getContents().size();
		c.offset.first = parent->offset.first + c.containerPosition() * width;
		c.dimension.first = width;
		c.dimension.second = 10;
	}
	c.rootContainer()->log("bla");
//	std::cout << "cb layout " << c.offset.first << " " << c.offset.second << " " << c.dimension.first << " " << c.dimension.second << std::endl;
	
	c.onDraw(*display);
}

