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

#include "Container.hpp"

//#include "RootContainer.hpp"

Component::Component(Container* p) {
	if (p == nullptr) // root container?
		init(nullptr, nullptr);
	else
		p->addComponent(this);
};

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
	for (auto current : c.getContents()) {
		cb(*current, userData);
		traverseChildren(*current, cb, userData);
	}
}

void Component::traverseChildren(const Component &c, void (*cb)(const Component&, void*), void *userData) {
	for (auto current : c.getContents()) {
		cb(*current, userData);
		traverseChildren(*current, cb, userData);
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

