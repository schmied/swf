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
#include "Context.hpp"
#include "Display.hpp"


static const std::basic_string<char> LOG_FACILITY = "COMPONENT";


/*
 * ******************************************************** constructor / destructor
 */

Component::Component(Context* c) {
//	std::printf("%s <init> context\n", LOG_FACILITY.c_str());
	if (c == nullptr) {
		std::printf("%s <init> no context\n", LOG_FACILITY.c_str());
		return;
	}
	parent = nullptr;
	context = c;
	style = {4, 7};
	onInvalidatePosition(this, nullptr);
	context->setRootContainer(*(Container*) this);
}

Component::Component(Container* p) {
//	std::printf("%s <init> container\n", LOG_FACILITY.c_str());
	if (p == nullptr) {
		std::printf("%s <init> no parent container\n", LOG_FACILITY.c_str());
		return;
	}
	context = nullptr;
	parent = p;
	style = {4, 7};
	onInvalidatePosition(this, nullptr);
	((Component*) parent)->addToContents(this);
}


/*
 * ******************************************************** private
 */

void Component::onInvalidatePosition(Component *c, void *ud) {
	c->position.x = -1;
}

inline bool Component::isPositionValid() const {
	return position.x != -1;
}

// returns position index of component in parent container
int Component::positionIndex() const {
	if (parent == nullptr)
		return 0;
	const auto contents = parent->contents();
	return std::distance(contents->begin(), std::find(contents->begin(), contents->end(), this));
}


/*
 * protected
 */

/*
Container* Component::getParent() const {
	return parent;
}
*/

const Container* Component::getParent() const {
	return parent;
}

Context* Component::getContext() {
	if (context != nullptr)
		return context;
	if (parent == nullptr) {
		std::printf("%s getContext() no context\n", LOG_FACILITY.c_str());
		return nullptr;
	}
	context = parent->getContext();
	if (context == nullptr) {
		std::printf("%s getContext() no parent context\n", LOG_FACILITY.c_str());
		return nullptr;
	}
	return context;
}

const Position* Component::getPosition() {
	if (isPositionValid())
		return &position;
	const Display *display = getContext()->getDisplay();
	if (display == nullptr) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "getPosition", "no display");
		return nullptr;
	}
	if (parent == nullptr) {
		position.x = 0;
		position.y = 0;
		position.w = display->screenDimension().first;
		position.h = display->screenDimension().second;
		position.textX = 0;
		position.textY = 0;
		return &position;
	}
	parent->calculatePosition(positionIndex(), style, &position);
	position.w = position.w - 2 * style.margin;
	if (position.w < 1)
		position.w = 1;
	position.h = position.h - 2 * style.margin;
	if (position.h < 1)
		position.h = 1;
	if (position.w > 2 * (style.margin + style.padding)) {
		position.x = position.x + style.margin;
		position.textX = position.x + style.padding;
	} else {
		position.textX = position.x;
	}
	if (position.h > 2 * (style.margin + style.padding)) {
		position.y = position.y + style.margin;
		position.textY = position.y + style.padding;
	} else {
		position.textY = position.y;
	}
	getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "getPosition", "%d+%d %dx%d t%d+%d m%d p%d", position.x, position.y,
	    position.w, position.h, position.textX, position.textY, style.margin, style.padding);
	return &position;
}

const Style* Component::getStyle() const {
	return &style;
}


/*
 * ******************************************************** public
 */


bool Component::isStateActive() const {
	return false;
}

bool Component::isStateFocus() const {
	return false;
}

void Component::invalidatePosition() {
	getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "flushPositionCache", nullptr);
	traverse(this, onInvalidatePosition, nullptr);
}

void Component::onDraw(const Display *display) {
	// do not draw root container
	if (parent == nullptr)
		return;
	const Position *p = getPosition();
	display->draw(p, "blaau");
}

/*
 * component traversing
 */

void Component::traverse(Component *c, void (*cb)(Component*, void*), void *userData) {
	cb(c, userData);
	traverseChildren(c, cb, userData);
}

/*
void Component::traverse(const Component *c, void (*cb)(const Component*, void*), void *userData) {
	cb(c, userData);
	traverseChildren(c, cb, userData);
}
*/

void Component::traverseChildren(Component *c, void (*cb)(Component*, void*), void *userData) {
	for (auto current : *c->contents()) {
		cb(current, userData);
		traverseChildren(current, cb, userData);
	}
}

/*
void Component::traverseChildren(const Component *c, void (*cb)(const Component*, void*), void *userData) {
	for (const auto current : *c->contents()) {
		cb(current, userData);
		traverseChildren(current, cb, userData);
	}
}
*/

