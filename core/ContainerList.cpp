/*
 * Copyright (c) 2015, 2016, Michael Schmiedgen
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

#include "ContainerList.hpp"

//#include "Container.hpp"
#include "Context.hpp"
#include "FrontendOut.hpp"


static const std::basic_string<char> LOG_FACILITY = "CONTAINER_LIST";


/*
 * constructor, destructor
 */

ContainerList::ContainerList(Context *p) : Container(p) {
	advance = 0;
};

ContainerList::ContainerList(Container *c) : Container(c) {
	advance = 0;
};

ContainerList::~ContainerList() {
};


/*
 * private
 */

void ContainerList::calculatePosition(const int childIndex, const Style &childStyle, Position *childPos) {
	Context *ctx = getContext();
	const Position *pos = getPosition();
	const Style *stl = getStyle();
	const int space = stl->margin + stl->padding;
	const int fontHeight = ctx->getFrontendOut()->fontDimension().second;
	childPos->w = pos->w - 2 * space;
	if (childPos->w < 1)
		childPos->w = 1;
	childPos->h = fontHeight + 2 * space;
	childPos->x = pos->x + stl->margin + stl->padding;
	childPos->y = pos->y + stl->margin + stl->padding + childIndex * (childPos->h + advance);
/*
	// columns
	const int size = contents()->size();
	childPos->x = pos->x + index * pos->w / size;
	childPos->y = 0;
	childPos->w = pos->w / size;
	childPos->h = getContext()->getDisplay()->fontDimension().second;
*/
	SWFLOG(getContext(), LOG_DEBUG, "%d+%d %dx%d", childPos->x, childPos->y, childPos->w, childPos->h);
}


/*
 * public
 */


/*
void Container::onDraw(const Display *display) {
//	std::pair<int,int> *offset = getOffset();
//	std::pair<int,int> *dimension = getDimension();
//	display->drawBorder(*offset, *dimension);
//	display->draw(*offset, *dimension);
}
*/
