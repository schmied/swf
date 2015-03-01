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

#include <curses.h>

#include <SDL/SDL.h>

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

//#include "../core/Component.hpp"
#include "../core/Button.hpp"
#include "../core/Container.hpp"
#include "../core/Context.hpp"
#include "../core/DisplayCurses.hpp"
#include "../core/DisplayXcb.hpp"
#include "../core/DisplaySdl.hpp"
#include "../core/Widget.hpp"


static const std::basic_string<char> LOG_FACILITY = "EXAMPLE";

static bool isGameMode = false;
static bool isGameModeMaxSpeed = false;


static bool isQuitEventXcb(void *event, void *data) {
	if (event == nullptr)
		return false;
	if (data == nullptr) {
		std::printf("%s isQuitEventXcb() display is null\n", LOG_FACILITY.c_str());
		return false;
	}
	const xcb_generic_event_t *e = (const xcb_generic_event_t*) event;
	const DisplayXcb *display = (const DisplayXcb*) data;
	if ((e->response_type & ~0x80) == XCB_KEY_PRESS) {
		xcb_key_press_event_t *kpe = (xcb_key_press_event_t*) event;
		if (display->keysym(kpe->detail) == 65307) // esc key
			return true;
	}
	return false;
}

static void onEventXcb(void *event, void *data) {
	if (event == nullptr)
		return;
	if (data == nullptr) {
		std::printf("%s onEventXcb() display is null\n", LOG_FACILITY.c_str());
		return;
	}
	const xcb_generic_event_t *e = (const xcb_generic_event_t*) event;
	const DisplayXcb *display = (const DisplayXcb*) data;
	Context *context = display->getContext();
	switch (e->response_type & ~0x80) {
	case XCB_EXPOSE: {
		xcb_expose_event_t *ee = (xcb_expose_event_t *) event;
		context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventXcb", "expose");
		break;
	}
	case XCB_BUTTON_PRESS: {
		xcb_button_press_event_t *bpe = (xcb_button_press_event_t*) event;
		context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventXcb", "button press %dx%d", bpe->event_x, bpe->event_y);
		break;
	}
	case XCB_KEY_PRESS: {
		xcb_key_press_event_t *kpe = (xcb_key_press_event_t*) event;
		xcb_keysym_t sym = display->keysym(kpe->detail);
		context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventXcb", "key press > %c %d", sym, sym);
		switch (sym) {
		case 'g':
			isGameMode = !isGameMode;
			context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventXcb", "game mode is %d",
			    isGameMode);
			break;
		case 's':
			isGameModeMaxSpeed = !isGameModeMaxSpeed;
			context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventXcb", "max speed is %d",
			    isGameModeMaxSpeed);
			break;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
}

static bool isQuitEventCurses(void *event, void *data) {
	if (event == nullptr)
		return false;
	const int c = *(const int*) event;
	if (c == 27) // esc key
		return true;
	return false;
}

static void onEventCurses(void *event, void *data) {
	if (event == nullptr)
		return;
	if (data == nullptr) {
		std::printf("%s onEventCurses() display is null\n", LOG_FACILITY.c_str());
		return;
	}
	const int c = *(const int*) event;
	if (c == ERR)
		return;
	const DisplayXcb *display = (const DisplayXcb*) data;
	Context *context = display->getContext();
	switch (c) {
	case 'g':
		isGameMode = !isGameMode;
		context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventCurses", "game mode is %d", isGameMode);
		break;
	case 's':
		isGameModeMaxSpeed = !isGameModeMaxSpeed;
		context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventCurses", "max speed is %d", isGameModeMaxSpeed);
		break;
	default:
		break;
	}
	context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventCurses", "char %d", c);
}

static bool isQuitEventSdl(void *event, void *data) {
	if (event == nullptr)
		return false;
	SDL_Event *e = (SDL_Event*) event;
	if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)
		return true;
	return false;
}

static void onEventSdl(void *event, void *data) {
	if (event == nullptr)
		return;
	if (data == nullptr) {
		std::printf("%s onEventSdl() display is null\n", LOG_FACILITY.c_str());
		return;
	}
	const DisplayXcb *display = (const DisplayXcb*) data;
	Context *context = display->getContext();
	const SDL_Event *e = (const SDL_Event*) event;
	switch (e->type) {
	case SDL_KEYDOWN:
		switch (e->key.keysym.sym) {
		case 'g':
			isGameMode = !isGameMode;
			context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventSdl", "game mode is %d", isGameMode);
			break;
		case 's':
			isGameModeMaxSpeed = !isGameModeMaxSpeed;
			context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventSdl", "max speed is %d", isGameMode);
			break;
		default:
			break;
		}
		context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventSdl", "typ %d sym %d", e->type, e->key.keysym.sym);
		break;
	default:
		break;
	}
}

int main(int argc, char **argv) {

	Context context;

	Container root { &context };
	Button button1 { &root };
	Button button2 { &root };
	Button button3 { &root };

/*
	xcb_connection_t *cn = DisplayXcb::initConnection();
	xcb_screen_t *scr = DisplayXcb::initScreen(cn);
	xcb_window_t win = DisplayXcb::initWindow(cn, scr, nullptr, nullptr);
	xcb_font_t fn = DisplayXcb::initFont(cn);
	DisplayXcb display { &context, cn, scr, win, fn };
	display.gameEventLoop(60, true, isQuitEventXcb, onEventXcb, &display);
//	display.applicationEventLoop(isQuitEventXcb, onEventXcb, &display);
*/

/*
	WINDOW *w = DisplayCurses::initWindow();
	DisplayCurses display = { &context, w };
//	display.gameEventLoop(60, true, isQuitEventCurses, onEventCurses, &display);
	display.applicationEventLoop(isQuitEventCurses, onEventCurses, &display);
*/

	SDL_Surface *scr = DisplaySdl::initScreen();
	DisplaySdl display { &context, scr };
	display.gameEventLoop(60, true, isQuitEventSdl, onEventSdl, &display);
//	display.applicationEventLoop(isQuitEventSdl, onEventSdl, &display);

	return 0;
}

