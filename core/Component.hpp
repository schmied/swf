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

#ifndef SWT_CORE_COMPONENT
#define SWT_CORE_COMPONENT

#include <vector>

class Container;
class Display;
class RootContainer;

class Component {

private:
	Container *parent;
	Display *display;	/* connected to a display? */

protected:
	Container* getParent() const;
	Display* getDisplay() const;
//	Display* getDisplay() const;
//	void unregisterDisplay();
//	void setParent(Container*);
	int dimX, dimY, dimWidth, dimHeight;
//	const RootContainer* getRootContainer() const;

	/* component traversing */
	virtual std::vector<Component*> getContents() const;
	static void traverse(Component &c, void (*)(Component&, void*), void*);
	static void traverse(const Component &c, void (*)(const Component&, void*), void*);
	static void traverseChildren(const Component &c, void (*)(Component&, void*), void*);
	static void traverseChildren(const Component &c, void (*)(const Component&, void*), void*);
	static void cbDisplayRegister(Component &c, void*);
	static void cbDisplayUnregister(Component &c, void*);
	static void cbDraw(const Component &c, void*);

public:
	Component(Container*);
	void init(Container*, Display*);
//	virtual Display* getDisplay() const;
	bool isStateActive() const;
	bool isStateFocus() const;
	virtual void onDraw() const = 0;

};

#endif // SWT_CORE_COMPONENT


// XXX delete
//	unsigned short getDimHeight() const;
//	unsigned short getDimWidth() const;
//	unsigned short getDimX() const;
//	unsigned short getDimY() const;


