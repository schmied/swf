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
#include <xcb/xcb_keysyms.h>

#include "DisplayXcb.hpp"

#include "Component.hpp"
#include "Context.hpp"


static const std::basic_string<char> LOG_FACILITY = "DISPLAY_SDL";

static xcb_rectangle_t rectBorder;


/*
 * ******************************************************** constructor / destructor
 */

DisplayXcb::DisplayXcb(Context *c, xcb_connection_t* cn, xcb_screen_t *scr, const xcb_window_t win, const xcb_font_t fn) : Display(c) {

	connection = cn;
	screen = scr;
	window = win;
	font = fn;

	context = xcb_generate_id(connection);
//	const uint32_t valueListGContext[] { screen->black_pixel, screen->white_pixel, font, 0 };
	const uint32_t valueListGContext[] { screen->black_pixel, screen->white_pixel, 0 };
	xcb_create_gc(connection, context, screen->root, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND
	    | XCB_GC_GRAPHICS_EXPOSURES, valueListGContext);
//	    | XCB_GC_FONT | XCB_GC_GRAPHICS_EXPOSURES, valueListGContext);

}

DisplayXcb::~DisplayXcb() {
	if (connection != nullptr) {
		xcb_close_font(connection, font);
		xcb_disconnect(connection);
	}
	getContext()->log(Context::LOG_INFO, LOG_FACILITY, "<free>", nullptr);
}


/*
 * ******************************************************** private
 */


/*
 * event handling
 */

void* DisplayXcb::eventPoll() {
	return xcb_poll_for_event(connection);
}

void* DisplayXcb::eventWait() {
	return xcb_poll_for_event(connection);
}

void DisplayXcb::gameEventSleep() const {
	timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 1000 * 1000;
	nanosleep(&ts, NULL);
}

long DisplayXcb::gameEventTicks() const {
	timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "fpsTicks", "clock gettime error");
	return 1000L * ts.tv_sec + ts.tv_nsec / 1000L / 1000L;
}


/*
 * ******************************************************** public
 */


/*
 * getter
 */

xcb_connection_t* DisplayXcb::getConnection() const {
	return connection;
}


/*
 * drawing
 */

void DisplayXcb::drawBorder(const std::pair<int,int> &offset, const std::pair<int,int> &dimension) const {
	rectBorder.x = offset.first;
	rectBorder.y = offset.second;
	rectBorder.width = dimension.first;
	rectBorder.height = dimension.second;
	xcb_poly_rectangle(connection, window, context, 1, &rectBorder);
	xcb_flush(connection);
}

void DisplayXcb::drawText(const std::pair<int,int> &offset, const std::pair<int,int> &dimension,
	    const std::basic_string<char> &text) const {
	xcb_image_text_8(connection, text.size(), window, context, offset.first + 1, offset.second + 12, text.c_str());
	xcb_flush(connection);
}

std::pair<int,int> DisplayXcb::screenDimension() const {
	xcb_get_geometry_cookie_t cookie = xcb_get_geometry(connection, window);
	xcb_get_geometry_reply_t *geometry = xcb_get_geometry_reply(connection, cookie, NULL);
	const std::pair<int,int> dimension = { geometry->width, geometry->height };
	free(geometry);
	return dimension;
}

std::pair<int,int> DisplayXcb::fontDimension() const {
	return { 10, 14 };
}


/*
 * event handling
 */

bool DisplayXcb::handleEvent(void *event) const {
	if (event == nullptr)
		return false;
	const xcb_generic_event_t *e = (const xcb_generic_event_t*) event;
	switch (e->response_type & ~0x80) {
	case XCB_EXPOSE: {
		xcb_expose_event_t *ee = (xcb_expose_event_t *) event;
		getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "expose");
		break;
	}
	case XCB_BUTTON_PRESS: {
		xcb_button_press_event_t *bpe = (xcb_button_press_event_t*) event;
		getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "button press %dx%d", bpe->event_x, bpe->event_y);
		break;
	}
	case XCB_KEY_PRESS: {
		xcb_key_press_event_t *kpe = (xcb_key_press_event_t*) event;
		xcb_keysym_t sym = keysym(kpe->detail);
		switch (sym) {
		default:
			return false;
			break;
		}
		getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "key press %c %d", sym, sym);
		break;
	}
	default:
		return false;
		break;
	}
	return true;
}


/*
 * xcb helper
 */

xcb_keysym_t DisplayXcb::keysym(xcb_keycode_t code) const {
	xcb_key_symbols_t *symbols = xcb_key_symbols_alloc(connection);
//	xcb_keysym_t sym = xcb_key_press_lookup_keysym(symbols, kpe, kpe->state);
	xcb_keysym_t sym = xcb_key_symbols_get_keysym(symbols, code, 0);
	xcb_key_symbols_free(symbols);
	return sym;
}

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

xcb_screen_t* DisplayXcb::initScreen(xcb_connection_t *cn) {
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

xcb_window_t DisplayXcb::initWindow(xcb_connection_t *cn, xcb_screen_t *scr, const std::pair<int,int> *offset,
	    const std::pair<int,int> *dimension) {

	std::pair<int,int> off { 0, 0 };
	if (offset != nullptr)
		off = *offset;
	std::pair<int,int> dim = { scr->white_pixel, scr->height_in_pixels };
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
	xcb_flush(cn);
	return win;
}

xcb_font_t DisplayXcb::initFont(xcb_connection_t *cn) {
	xcb_font_t fn = xcb_generate_id(cn);
	xcb_open_font(cn, fn, 9, "Helvetica");
	return fn;
}

