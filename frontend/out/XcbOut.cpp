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

#include <iostream>
#include <utility>

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

#include "XcbOut.hpp"

//#include "Component.hpp"
#include "../../core/Context.hpp"


static const std::basic_string<char> LOG_FACILITY = "XCB_OUT";


/*
 * ******************************************************** constructor / destructor
 */

XcbOut::XcbOut(Context &ctx, xcb_connection_t* cn, xcb_screen_t *scr, const xcb_window_t win, const xcb_font_t fn) : FrontendOut(ctx) {

	connection = cn;
	screen = scr;
	window = win;
	font = fn;

	gcontext = xcb_generate_id(connection);
//	const uint32_t valueListGContext[] { screen->black_pixel, screen->white_pixel, font, 0 };
	const uint32_t valueListGContext[] { screen->black_pixel, screen->white_pixel, 0 };
	xcb_create_gc(connection, gcontext, screen->root, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND
	    | XCB_GC_GRAPHICS_EXPOSURES, valueListGContext);
//	    | XCB_GC_FONT | XCB_GC_GRAPHICS_EXPOSURES, valueListGContext);

	gcontextInverse = xcb_generate_id(connection);
	const uint32_t valueListGContextInverse[] { screen->white_pixel, screen->black_pixel, 0 };
	xcb_create_gc(connection, gcontextInverse, screen->root, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND
	    | XCB_GC_GRAPHICS_EXPOSURES, valueListGContextInverse);

}

XcbOut::~XcbOut() {
	// XXX close this in application
/*
	if (connection != nullptr) {
		xcb_close_font(connection, font);
		xcb_disconnect(connection);
	}
	getContext()->log(Context::LOG_INFO, LOG_FACILITY, "<free>", nullptr);
*/
}


/*
 * ******************************************************** private
 */


/*
 * ******************************************************** public
 */


/*
 * getter
 */

xcb_connection_t* XcbOut::getConnection() const {
	return connection;
}

xcb_screen_t* XcbOut::getScreen() const {
	return screen;
}

xcb_window_t XcbOut::getWindow() const {
	return window;
}

xcb_font_t XcbOut::getFont() const {
	return font;
}

xcb_gcontext_t XcbOut::getGContext() const {
	return gcontext;
}

xcb_gcontext_t XcbOut::getGContextInverse() const {
	return gcontextInverse;
}


/*
 * drawing
 */

static xcb_rectangle_t rectBorder;

/*
void DisplayXcb::drawBorder(const std::pair<int,int> &offset, const std::pair<int,int> &dimension) const {
	rectBorder.x = offset.first;
	rectBorder.y = offset.second;
	rectBorder.width = dimension.first;
	rectBorder.height = dimension.second;
	xcb_poly_rectangle(connection, window, gcontext, 1, &rectBorder);
//	xcb_flush(connection);
}
*/

//void DisplayXcb::drawText(const std::pair<int,int> &offset, const std::pair<int,int> &dimension,
//	    const std::basic_string<char> &text) const {
void XcbOut::draw(const Position &pos, const Style &stl, const std::basic_string<char> &text) const {
	xcb_image_text_8(connection, text.size(), window, gcontext, pos.textX + 1, pos.textY + 12, text.c_str());
//	xcb_flush(connection);
}

std::pair<int,int> XcbOut::screenDimension() const {
	xcb_get_geometry_cookie_t cookie = xcb_get_geometry(connection, window);
	xcb_get_geometry_reply_t *geometry = xcb_get_geometry_reply(connection, cookie, NULL);
	const std::pair<int,int> dimension = { geometry->width, geometry->height };
	free(geometry);
	return dimension;
}

std::pair<int,int> XcbOut::fontDimension() const {
	return { 10, 14 };
}


/*
 * xcb helper
 */

/*
xcb_connection_t* DisplayXcb::initConnection() {
	int n;
	xcb_connection_t *cn = xcb_connect(NULL, &n);
	if (cn == NULL) {
		// log
		return nullptr;
	}
	const int err = xcb_connection_has_error(cn);
	if (err > 0) {
		std::printf("%s initConnection() xcb connect error %d\n", LOG_FACILITY.c_str(), err);
		return nullptr;
	}
	return cn;
}
*/

xcb_screen_t* XcbOut::initScreen(xcb_connection_t *cn) {
	xcb_screen_t *scr = NULL;
	xcb_screen_iterator_t it = xcb_setup_roots_iterator(xcb_get_setup(cn));
	for (; it.rem; xcb_screen_next(&it)) {
		scr = it.data;
		if (scr != NULL)
			break;
	}
	if (scr == NULL) {
		std::printf("%s initScreen() no screen found\n", LOG_FACILITY.c_str());
		return nullptr;
	}
	std::printf("%s initScreen() screen found %dx%d\n", LOG_FACILITY.c_str(), scr->width_in_pixels, scr->height_in_pixels);
	return scr;
}

xcb_window_t XcbOut::initWindow(xcb_connection_t *cn, xcb_screen_t *scr, const std::pair<int,int> *offset,
	    const std::pair<int,int> *dimension) {

	std::pair<int,int> off { 0, 0 };
	if (offset != nullptr)
		off = *offset;
	std::pair<int,int> dim = { scr->width_in_pixels, scr->height_in_pixels };
	if (dimension != nullptr)
		dim = *dimension;
	if (off.first >= scr->width_in_pixels - 1)
		off.first = 0;
	if (off.second >= scr->height_in_pixels - 1)
		off.second = 0;
	if (off.first + dim.first > scr->width_in_pixels)
		dim.first = scr->width_in_pixels - off.first;
	if (off.second + dim.second > scr->width_in_pixels)
		dim.first = scr->width_in_pixels - off.first;

	xcb_window_t win = xcb_generate_id(cn);
	const uint32_t valueListWindow[] { scr->white_pixel,
	    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS };
	xcb_create_window(cn, XCB_COPY_FROM_PARENT, win,
	    scr->root, off.first, off.second, dim.first, dim.second, 0,
	    XCB_WINDOW_CLASS_INPUT_OUTPUT, scr->root_visual,
	    XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, valueListWindow);

	xcb_map_window(cn, win);
//	xcb_flush(cn);
	return win;
}

xcb_font_t XcbOut::initFont(xcb_connection_t *cn) {
	xcb_font_t fn = xcb_generate_id(cn);
	xcb_open_font(cn, fn, 9, "Helvetica");
	return fn;
}

