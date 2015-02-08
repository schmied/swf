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

#include <iostream>
#include <vector>

#include "Component.hpp"

#include "Container.hpp"
#include "Display.hpp"

//#include "RootContainer.hpp"

Component::Component(Container* p) {
	if (p == nullptr) // root container?
		init(nullptr, nullptr);
	else
		p->addComponent(this);
}

void Component::init(Container *p, Display *d) {
	parent = p;
	display = d;
}

Container* Component::getParent() const {
	return parent;
}

Display* Component::getDisplay() const {
	return display;
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

void Component::cbDisplayRegister(Component &c, void *userData) {
	c.display = static_cast<Display*>(userData);
}

void Component::cbDisplayUnregister(Component &c, void *userData) {
	c.display = nullptr;
}

void Component::cbDraw(const Component &c, void *userData) {
	c.onDraw();
}

void Component::cbLayout(Component &c, void *userData) {
	const auto parent = c.parent;
	if (parent == nullptr) {
		// XXX log if no display connected
		c.offset = { 0, 0 };
		c.dimension = c.display->getDimension();
	} else {
		std::cout << "cb layout pos " << c.containerPosition() << std::endl;
		const int width = parent->dimension.first / parent->getContents().size();
		c.offset.first = parent->offset.first + c.containerPosition() * width;
		c.dimension.first = width;
		c.dimension.second = 10;
	}
	std::cout << "cb layout " << c.offset.first << " " << c.offset.second << " " << c.dimension.first << " " << c.dimension.second << std::endl;
	
}

/*
const RootContainer* Component::getRootContainer() const {
	if (parent == nullptr)
		return nullptr;
	const Component *c = this;
	while (c->parent != nullptr)
		c = c->parent;
	return static_cast<const RootContainer*>(c);
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
*/

