/*
 * Copyright (c) 2013, Michael Schmiedgen
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

#include <iostream>

#include <xcb/xcb.h>

#include "DisplayXcb.hpp"

#include "Component.hpp"
#include "Display.hpp"

//DisplayXcb::DisplayXcb() {
//}

xcb_rectangle_t rectBorder;

DisplayXcb::DisplayXcb(const unsigned short w, const unsigned short h) :
    Display(w, h) {
	int n;
	connection = xcb_connect(NULL, &n);
	if (connection == NULL) {
		// log
		return;
	}
	screen = NULL;
	xcb_screen_iterator_t i = xcb_setup_roots_iterator(xcb_get_setup(
	    connection));
	for (; i.rem; n--, xcb_screen_next(&i)) {
		std::cout << "Looking for screen #" << n << '.' << std::endl;
		if (n == 0) {
			screen = i.data;
			break;
		}
	}
	if (screen == NULL) {
		// log
		return;
	}

	std::cout << "Found screen with " << screen->width_in_pixels << 'x' <<
	    screen->height_in_pixels << '.' << std::endl;

	window = xcb_generate_id(connection);
	const uint32_t valueListWindow[] { screen->white_pixel,
	    XCB_EVENT_MASK_EXPOSURE };
	xcb_create_window(connection, XCB_COPY_FROM_PARENT, window,
	    screen->root, 0, 0, getWidth(), getHeight(), 0,
	    XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
	    XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, valueListWindow);

	xcb_map_window(connection, window);
	xcb_flush(connection);

	gContext = xcb_generate_id(connection);
	const uint32_t valueListGContext[] { screen->black_pixel, 0 };
	xcb_create_gc(connection, gContext, screen->root, XCB_GC_FOREGROUND |
	    XCB_GC_GRAPHICS_EXPOSURES, valueListGContext);

	xcb_map_window(connection, window);
	xcb_flush(connection);
}

DisplayXcb::~DisplayXcb() {

	if (connection != NULL)
		xcb_disconnect(connection);
	std::cout << "displayxcb terminated." << std::endl;
}

void DisplayXcb::drawBegin() const {
}

void DisplayXcb::drawComponent(const Component& c) const {
	rectBorder.x = c.getDimX();
	rectBorder.y = c.getDimY();
	rectBorder.width = c.getDimWidth();
	rectBorder.height = c.getDimHeight();
	xcb_poly_rectangle(connection, window, gContext, 1, &rectBorder);
}

void DisplayXcb::drawEnd() const {
	xcb_flush(connection);
}

