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

#ifndef SWF_CORE_COMPONENT
#define SWF_CORE_COMPONENT

#include <utility>
#include <vector>

class Container;
class Context;
class FrontendOut;

struct Position {
	int x, y, w, h, textX, textY;
};

struct Style {
	int margin, padding;
};

//enum TraverseCondition { continueTraverse, returnCurrent, skipChildren };
enum class TraverseCondition { match, matchBreak, notMatch, notMatchBreak };

class Component {

private:
	Container *parent;
	Context *context; // cache to context

	// position
	Position position;
	static TraverseCondition onInvalidatePosition(Component*, void*);
	inline bool isPositionValid() const;
	int positionIndex() const;

	virtual void addToContents(Component*) = 0;

protected:
	Style style;

	inline const Container* getParent() const;
	const Position* getPosition();
	const Style* getStyle() const;

public:
	Component(Context*);
	Component(Container*);

	Context* getContext();

	bool isStateActive() const;
	bool isStateFocus() const;

	void invalidatePosition();
	virtual std::vector<Component*>* contents() = 0;
//	virtual void onDraw(const Display*) = 0;
	void onDraw(const FrontendOut*);

	/* component traversing */
	static bool traverseExclusive(Component*, TraverseCondition (*)(Component*, void*), void*, std::vector<Component*>*);
	static bool traverseExclusive(Component*, TraverseCondition (*)(Component*, void*), void*);
	static bool traverseInclusive(Component*, TraverseCondition (*)(Component*, void*), void*, std::vector<Component*>*);
	static bool traverseInclusive(Component*, TraverseCondition (*)(Component*, void*), void*);
	static Component* findComponentExclusive(Component*, TraverseCondition (*)(Component*, void*), void*);
	static Component* findComponentInclusive(Component*, TraverseCondition (*)(Component*, void*), void*);
	static std::vector<Component*> findComponentsExclusive(Component*, TraverseCondition (*)(Component*, void*), void*);
	static std::vector<Component*> findComponentsInclusive(Component*, TraverseCondition (*)(Component*, void*), void*);

};

#endif // SWF_CORE_COMPONENT
