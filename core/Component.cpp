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
	onInvalidatePosition(this, nullptr);
	((Component*) parent)->addToContents(this);
}


/*
 * ******************************************************** private
 */

void Component::onInvalidatePosition(Component *c, void *ud) {
	c->offset.first = -1;
	c->dimension.first = -1;
}

inline bool Component::isPositionValid() const {
	return offset.first != -1 && dimension.first != -1;
}

// returns position index of component in parent container
int Component::containerPositionIndex() const {
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

std::pair<int,int>* Component::getOffset() {
	if (isPositionValid())
		return &offset;
	if (parent == nullptr) {
		offset.first = 0;
		offset.second = 0;
	} else {
		const std::pair<int,int> *parentOffset = parent->getOffset();
		if (parentOffset == nullptr) {
			getContext()->log(Context::LOG_WARN, LOG_FACILITY, "getOffset", "no parent offset");
			return nullptr;
		}
		const std::pair<int,int> *parentDimension = parent->getDimension();
		if (parentDimension == nullptr) {
			getContext()->log(Context::LOG_WARN, LOG_FACILITY, "getOffset", "no parent dimension");
			return nullptr;
		}
		const int width = parentDimension->first / parent->contents()->size();
		offset.first = parentOffset->first + containerPositionIndex() * width;
		offset.second = 0;
	}
	getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "getOffset", "%d+%d", offset.first, offset.second);
	return &offset;
}

std::pair<int,int>* Component::getDimension() {
	if (isPositionValid())
		return &dimension;
	const Display *display = getContext()->getDisplay();
	if (display == nullptr) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "getDimension", "no display");
		return nullptr;
	}
	if (parent == nullptr) {
		dimension.first = display->screenDimension().first;
		dimension.second = display->screenDimension().second;
	} else {
		const std::pair<int,int> *parentOffset = parent->getOffset();
		if (parentOffset == nullptr) {
			getContext()->log(Context::LOG_WARN, LOG_FACILITY, "getDimension", "no parent offset");
			return nullptr;
		}
		const std::pair<int,int> *parentDimension = parent->getDimension();
		if (parentDimension == nullptr) {
			getContext()->log(Context::LOG_WARN, LOG_FACILITY, "getDimension", "no parent dimension");
			return nullptr;
		}
		const int width = parentDimension->first / parent->contents()->size();
		dimension.first = width;
		dimension.second = display->fontDimension().second;
	}
	getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "getDimension", "%dx%d", dimension.first, dimension.second);
	return &dimension;
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

