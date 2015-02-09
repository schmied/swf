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

#ifndef SWT_CORE_COMPONENT
#define SWT_CORE_COMPONENT

#include <utility>
#include <vector>

class Container;
class Display;
class RootContainer;


class Component {

private:
	Container *parent;

	int containerPosition() const;

	virtual void addToContents(Component*) = 0;
	virtual void onDraw(const Display&) const = 0;

protected:
	// XXX put this in separate structure
	std::pair<int,int> offset;
	std::pair<int,int> dimension;

	Container* getParent() const;

	RootContainer* rootContainer();

	static void cbDraw(Component&, void*);

	/* component traversing */
	static void traverse(Component&, void (*)(Component&, void*), void*);
	static void traverse(const Component&, void (*)(const Component&, void*), void*);
	static void traverseChildren(const Component&, void (*)(Component&, void*), void*);
	static void traverseChildren(const Component&, void (*)(const Component&, void*), void*);

public:
	Component(Container*);

	bool isStateActive() const;
	bool isStateFocus() const;

	virtual std::vector<Component*> contents() const = 0;

};

#endif // SWT_CORE_COMPONENT

