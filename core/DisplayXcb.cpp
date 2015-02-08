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

#include <iostream>
#include <utility>

#include <xcb/xcb.h>

#include "DisplayXcb.hpp"

#include "Component.hpp"
#include "RootContainer.hpp"

xcb_rectangle_t rectBorder;

DisplayXcb::DisplayXcb(const std::pair<int,int> &dimension) {

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
	    screen->root, 0, 0, dimension.first, dimension.second, 0,
	    XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
	    XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, valueListWindow);

	xcb_map_window(connection, window);
	xcb_flush(connection);

	font = xcb_generate_id(connection);
	xcb_open_font(connection, font, 9, "Helvetica");
	
	context = xcb_generate_id(connection);
//	const uint32_t valueListGContext[] { screen->black_pixel, screen->white_pixel, font, 0 };
	const uint32_t valueListGContext[] { screen->black_pixel, screen->white_pixel, 0 };
	xcb_create_gc(connection, context, screen->root, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND
	    | XCB_GC_GRAPHICS_EXPOSURES, valueListGContext);
//	    | XCB_GC_FONT | XCB_GC_GRAPHICS_EXPOSURES, valueListGContext);

//	xcb_close_font(connection, font);

	xcb_map_window(connection, window);
	xcb_flush(connection);
}

DisplayXcb::~DisplayXcb() {
	if (connection != NULL)
		xcb_disconnect(connection);
	std::cout << "displayxcb terminated." << std::endl;
}

void DisplayXcb::drawBorder(const std::pair<int,int> &offset, const std::pair<int,int> &dimension) const {
	rectBorder.x = offset.first;
	rectBorder.y = offset.second;
	rectBorder.width = dimension.first;
	rectBorder.height = dimension.second;
	xcb_poly_rectangle(connection, window, context, 1, &rectBorder);
	xcb_flush(connection);
}

void DisplayXcb::drawText(const std::pair<int,int> &offset, const std::basic_string<char> &text) const {
	xcb_image_text_8(connection, text.size(), window, context, offset.first, offset.second, text.c_str());
	xcb_flush(connection);
}

std::pair<int,int> DisplayXcb::getDimension() const {
	xcb_get_geometry_cookie_t cookie = xcb_get_geometry(connection, window);
	xcb_get_geometry_reply_t *geometry = xcb_get_geometry_reply(connection, cookie, NULL);
	const std::pair<int,int> dimension = { geometry->width, geometry->height };
	free(geometry);
	return dimension;
}

