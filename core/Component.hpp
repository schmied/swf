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

#ifndef SWF_CORE_COMPONENT
#define SWF_CORE_COMPONENT

#include <utility>
#include <vector>

class Container;
class Context;
class Display;


class Component {

private:
	Container *parent;

	Context *context; // cache to context
	std::pair<int,int> offset; // cache to offset;
	std::pair<int,int> dimension; // cache to dimension;

	static void onFlushPositionCache(Component*, void*);
	inline bool isPositionCacheValid() const;

	int containerPositionIndex() const;

	virtual void addToContents(Component*) = 0;

protected:
	inline const Container* getParent() const;
	std::pair<int,int>* getOffset();
	std::pair<int,int>* getDimension();

public:
	Component(Context*);
	Component(Container*);

	Context* getContext();

	bool isStateActive() const;
	bool isStateFocus() const;

	void flushPositionCache();
	virtual std::vector<Component*>* contents() = 0;
	virtual void onDraw(const Display*) = 0;

	/* component traversing */
	static void traverse(Component*, void (*)(Component*, void*), void*);
//	static void traverse(const Component*, void (*)(const Component*, void*), void*);
	static void traverseChildren(Component*, void (*)(Component*, void*), void*);
//	static void traverseChildren(const Component*, void (*)(const Component*, void*), void*);
};

#endif // SWF_CORE_COMPONENT

