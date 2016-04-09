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

#ifndef SWF_FRONTEND_IN_XCB
#define SWF_FRONTEND_IN_XCB

#include <string>
#include <utility>

#include <xcb/xcb.h>

#include "../../core/FrontendIn.hpp"


class XcbIn : public FrontendIn {

private:
	xcb_connection_t *connection;
/*
	xcb_screen_t *screen;
	xcb_window_t window;
	xcb_font_t font;
	xcb_gcontext_t gcontext;
	xcb_gcontext_t gcontextInverse;		// for background fill
*/

	// event handling
	void* eventPoll() override;
	void* eventWait() override;
	void gameEventSleep() const override;
	long gameEventTicks() const override;
	void eventFree(void*) override;

public:
//	XcbIn(Context&, xcb_connection_t*, xcb_screen_t*, const xcb_window_t, const xcb_font_t);
	XcbIn(Context&, xcb_connection_t*);
	~XcbIn();

	// getter
	xcb_connection_t* getConnection() const;
/*
	xcb_screen_t* getScreen() const;
	xcb_window_t getWindow() const;
	xcb_gcontext_t getGContext() const;
	xcb_gcontext_t getGContextInverse() const;
*/

	// drawing
//	void drawBorder(const std::pair<int,int>&, const std::pair<int,int>&) const override;
//	void drawText(const std::pair<int,int>&, const std::pair<int,int>&, const std::basic_string<char>&) const override;
//	void draw(const std::pair<int,int>&, const std::pair<int,int>&, const std::basic_string<char>&) const override;
/*
	void draw(const Position&, const Style&, const std::basic_string<char>&) const override;
	std::pair<int,int> screenDimension() const override;
	std::pair<int,int> fontDimension() const override;
*/

	// event handling
	void handleEvent(void*) const override;

	// xcb helper
	xcb_keysym_t keysym(xcb_keycode_t) const;
	static xcb_connection_t* initConnection();
/*
	static xcb_screen_t* initScreen(xcb_connection_t*);
	static xcb_window_t initWindow(xcb_connection_t*, xcb_screen_t*, const std::pair<int,int>*, const std::pair<int,int>*);
	static xcb_font_t initFont(xcb_connection_t*);
*/

};

#endif // SWF_FRONTEND_IN_XCB

