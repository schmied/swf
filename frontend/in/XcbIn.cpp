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

#include "XcbIn.hpp"

#include "../../core/Context.hpp"


static const std::basic_string<char> LOG_FACILITY = "XCB_IN";


/*
 * ******************************************************** constructor / destructor
 */

XcbIn::XcbIn(Context &ctx, xcb_connection_t* cn) : FrontendIn(ctx) {
	connection = cn;
}

XcbIn::~XcbIn() {
	SWFLOG(getContext(), LOG_INFO, nullptr);
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

xcb_connection_t* XcbIn::getConnection() const {
	return connection;
}


/*
 * event handling
 */

void XcbIn::eventFree(void *event) {
	free(event);
}

void* XcbIn::eventPoll() {
	return xcb_poll_for_event(connection);
}

void* XcbIn::eventWait() {
	return xcb_poll_for_event(connection);
}

void XcbIn::in(void *event) const {
	if (event == nullptr)
		return;
	const xcb_generic_event_t *e = (const xcb_generic_event_t*) event;
	switch (e->response_type & ~0x80) {
	case XCB_EXPOSE: {
		xcb_expose_event_t *ee = (xcb_expose_event_t *) event;
		SWFLOG(getContext(), LOG_DEBUG, "expose");
		((Component*)getContext()->getRootContainer())->invalidatePosition();
		break;
	}
	case XCB_BUTTON_PRESS: {
		xcb_button_press_event_t *bpe = (xcb_button_press_event_t*) event;
		SWFLOG(getContext(), LOG_DEBUG, "button press %dx%d", bpe->event_x, bpe->event_y);
		break;
	}
	case XCB_KEY_PRESS: {
		xcb_key_press_event_t *kpe = (xcb_key_press_event_t*) event;
		xcb_keysym_t sym = keysym(kpe->detail);
//		SWFLOG(getContext(), LOG_DEBUG, "key press %c %d", sym, sym);
		switch (sym) {
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
}


/*
 * game loop
 */

void XcbIn::gameLoopSleep() const {
	timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 1000 * 1000;
	nanosleep(&ts, NULL);
}

long XcbIn::gameLoopTicks() const {
	timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
		SWFLOG(getContext(), LOG_WARN, "clock gettime error");
	return 1000L * ts.tv_sec + ts.tv_nsec / 1000L / 1000L;
}


/*
 * xcb helper
 */

xcb_keysym_t XcbIn::keysym(xcb_keycode_t code) const {
	xcb_key_symbols_t *symbols = xcb_key_symbols_alloc(connection);
//	xcb_keysym_t sym = xcb_key_press_lookup_keysym(symbols, kpe, kpe->state);
	xcb_keysym_t sym = xcb_key_symbols_get_keysym(symbols, code, 0);
	xcb_key_symbols_free(symbols);
	return sym;
}

xcb_connection_t* XcbIn::initConnection() {
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

/*
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

xcb_font_t DisplayXcb::initFont(xcb_connection_t *cn) {
	xcb_font_t fn = xcb_generate_id(cn);
	xcb_open_font(cn, fn, 9, "Helvetica");
	return fn;
}
*/

