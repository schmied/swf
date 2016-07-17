/*
 * Copyright (c) 2013, 2014, 2015, 2016, Michael Schmiedgen
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
#include "FrontendOut.hpp"


static const std::basic_string<char> LOG_FACILITY = "COMPONENT";


/*
 * ******************************************************** constructor / destructor
 */

Component::Component(Context* ctx) {
//	std::printf("%s <init> context\n", LOG_FACILITY.c_str());
	if (ctx == nullptr) {
		std::printf("%s <init> no context\n", LOG_FACILITY.c_str());
		return;
	}
	parent = nullptr;
	context = ctx;
	style = {0, 0};
	onInvalidatePosition(this, nullptr);
	context->setRootContainer((Container*) this);
}

Component::Component(Container* p) {
//	std::printf("%s <init> container\n", LOG_FACILITY.c_str());
	if (p == nullptr) {
		std::printf("%s <init> no parent container\n", LOG_FACILITY.c_str());
		return;
	}
	context = nullptr;
	parent = p;
	style = {0, 0};
	onInvalidatePosition(this, nullptr);
	((Component*) parent)->addToContents(this);
}


/*
 * ******************************************************** private
 */

TraverseCondition Component::onInvalidatePosition(Component *c, void *ud) {
	c->position.x = -1;
	return TraverseCondition::notMatch;
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
 * ******************************************************** protected
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
	const FrontendOut *out = getContext()->getFrontendOut();
	if (out == nullptr) {
		SWFLOG(getContext(), LOG_WARN, "no frontendOut");
		return nullptr;
	}
	if (parent == nullptr) {
		position.x = 0;
		position.y = 0;
		position.w = out->screenDimension().first;
		position.h = out->screenDimension().second;
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
	SWFLOG(getContext(), LOG_DEBUG, "%d+%d %dx%d t%d+%d m%d p%d", position.x, position.y,
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
	SWFLOG(getContext(), LOG_DEBUG, nullptr);
	traverseInclusive(this, onInvalidatePosition, nullptr);
}

void Component::onDraw(const FrontendOut *out) {
	// do not draw root container
	if (parent == nullptr)
		return;
	const Position *p = getPosition();
	const Style *s = getStyle();
	out->draw(*p, *s, "blaau");
}


/*
 * component traversing
 */

bool Component::traverseExclusive(Component *c, TraverseCondition (*cb)(Component*, void*), void *userData, std::vector<Component*> *matches) {
	bool running;
	for (auto current : *c->contents()) {
		switch (cb(current, userData)) {
		case TraverseCondition::match:
			if (matches == nullptr)
				SWFLOG(c->context, LOG_WARN, "cannot add match");
			else
				matches->push_back(current);
			running = traverseExclusive(current, cb, userData, matches);
			if (!running)
				return false;
			break; // not reached
		case TraverseCondition::matchBreak:
			if (matches == nullptr)
				SWFLOG(c->context, LOG_WARN, "cannot add match");
			else
				matches->push_back(current);
			return false;
			break; // not reached
		case TraverseCondition::notMatch:
			running = traverseExclusive(current, cb, userData, matches);
			if (!running)
				return false;
			break;
		case TraverseCondition::notMatchBreak:
			return false;
		}
	}
	return true;
}

bool Component::traverseExclusive(Component *c, TraverseCondition (*cb)(Component*, void*), void *userData) {
	return traverseExclusive(c, cb, userData, nullptr);
}

bool Component::traverseInclusive(Component *c, TraverseCondition (*cb)(Component*, void*), void *userData, std::vector<Component*> *matches) {
	switch (cb(c, userData)) {
	case TraverseCondition::match:
		if (matches == nullptr)
			SWFLOG(c->context, LOG_WARN, "cannot add match");
		else
			matches->push_back(c);
		break;
	case TraverseCondition::matchBreak:
		if (matches == nullptr)
			SWFLOG(c->context, LOG_WARN, "cannot add match");
		else
			matches->push_back(c);
		return false;
		break; // not reached
	case TraverseCondition::notMatch:
		break;
	case TraverseCondition::notMatchBreak:
		return false;
		break; // not reached
	}
	return traverseExclusive(c, cb, userData, matches);
}

bool Component::traverseInclusive(Component *c, TraverseCondition (*cb)(Component*, void*), void *userData) {
	return traverseInclusive(c, cb, userData, nullptr);
}

Component* Component::findComponentExclusive(Component *c, TraverseCondition (*cb)(Component*, void*), void *userData) {
	std::vector<Component*> matches {};
	traverseExclusive(c, cb, userData, &matches);
	if (matches.size() == 1)
		return matches[0];
	SWFLOG(c->context, LOG_DEBUG, "more than one match");
	return nullptr;
}

Component* Component::findComponentInclusive(Component *c, TraverseCondition (*cb)(Component*, void*), void *userData) {
	std::vector<Component*> matches {};
	traverseInclusive(c, cb, userData, &matches);
	if (matches.size() == 1)
		return matches[0];
	SWFLOG(c->context, LOG_DEBUG, "more than one match");
	return nullptr;
}

std::vector<Component*> Component::findComponentsExclusive(Component *c, TraverseCondition (*cb)(Component*, void*), void *userData) {
	std::vector<Component*> matches {};
	traverseExclusive(c, cb, userData, &matches);
	return matches;
}

std::vector<Component*> Component::findComponentsInclusive(Component *c, TraverseCondition (*cb)(Component*, void*), void *userData) {
	std::vector<Component*> matches {};
	traverseInclusive(c, cb, userData, &matches);
	return matches;
}
