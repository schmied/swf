/*
 * Copyright (c) 2014, 2015, Michael Schmiedgen
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

#ifndef SWF_CORE_CONTAINER
#define SWF_CORE_CONTAINER

#include <vector>

#include "Component.hpp"

//enum layout_t { INLINE, 

class Container : public Component {

private:
	std::vector<Component*> components;// {};
//	int layout_columns;
//	int layout_rows;

	void addToContents(Component*) override;
	void onDraw(const Display*) override;

protected:

public:
	Container(Context*);
	Container(Container*);
	~Container();

	std::vector<Component*>* contents() override;

};

#endif // SWF_CORE_CONTAINER

