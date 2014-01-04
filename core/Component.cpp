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

Component::Component(Container* p) : parent { p } {
	if (parent == nullptr) // root container?
		return;
	parent->addComponent(this);
	display = parent->display;
};

/*
Component::Component(Container *p) {
	parent = p;
}
*/

Container* Component::getParent() const {
	return parent;
}

/*
Display* Component::getDisplay() const {
	return display;
}
*/

/*
void Component::setParent(Container *p) {
	parent = p;
}
*/

/*
const RootContainer* Component::getRootContainer() const {
	if (parent == nullptr)
		return nullptr;
	const Component *c = this;
	while (c->parent != nullptr)
		c = c->parent;
	return static_cast<const RootContainer*>(c);
}

Display* Component::getDisplay() const {
	const RootContainer *r = getRootContainer();
	if (r == nullptr)
		return nullptr;
	return r->getDisplay();
}
*/

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

void Component::traverse(Component &c, void (*cb)(Component&)) {
	cb(c);
	traverseChildren(c, cb);
}

void Component::traverseChildren(const Component &c, void (*cb)(Component&)) {
	for (auto current : c.getContents()) {
		cb(*current);
		traverseChildren(*current, cb);
	}
}

void Component::cbDraw(Component &c) {
	c.onDraw();
}

void Component::cbRegisterDisplay(Component &c) {
	c.display = c.parent->display;
}

void Component::cbUnregisterDisplay(Component &c) {
	c.display = nullptr;
}

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

