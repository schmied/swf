/*
 * Copyright (c) 2014, 2015, 2016, Michael Schmiedgen
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

#include "Container.hpp"

#include "Context.hpp"
//#include "Display.hpp"


static const std::basic_string<char> LOG_FACILITY = "CONTAINER";


/*
 * constructor, destructor
 */

Container::Container(Context *ctx) : Component(ctx) {
};

Container::Container(Container *c) : Component(c) {
};

Container::~Container() {
};


/*
 * private
 */

void Container::addToContents(Component *c) {
	components.push_back(c);
	SWFLOG(getContext(), LOG_DEBUG, "new size %ld", contents()->size());
}


/*
 * public
 */

std::vector<Component*>* Container::contents() {
	return &components;
}

/*
void Container::onDraw(const Display *display) {
//	std::pair<int,int> *offset = getOffset();
//	std::pair<int,int> *dimension = getDimension();
//	display->drawBorder(*offset, *dimension);
//	display->draw(*offset, *dimension);
}
*/

