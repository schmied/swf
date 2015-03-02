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


#include <cstdlib>
#include <ctime>
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


/*
 * boxes
 */

static const std::pair<int,int> boxFieldDim { 16000, 10000 };
static const int boxMaxDim = (boxFieldDim.first + boxFieldDim.second) / 2 / 10;
static const int boxMaxVel = 4 * boxMaxDim / 100;
struct Box {
	std::pair<int,int> offset;
	std::pair<int,int> dimension;
	std::pair<int,int> velocity;
};

static bool scaleBox(const Box &box, Box *boxScr, const std::pair<int,int> &scrDim) {
	const float scaleX = (float) scrDim.first / boxFieldDim.first;
	const float scaleY = (float) scrDim.second / boxFieldDim.second;
	boxScr->offset.first = scaleX * box.offset.first;
	boxScr->offset.second = scaleY * box.offset.second;
	boxScr->dimension.first = scaleX * box.dimension.first;
	if (boxScr->dimension.first < 1)
		boxScr->dimension.first = 1;
	boxScr->dimension.second = scaleY * box.dimension.second;
	if (boxScr->dimension.second < 1)
		boxScr->dimension.second = 1;
	if (boxScr->offset.first < 0)
		return false;
	if (boxScr->offset.second < 0)
		return false;
	if (boxScr->offset.first + boxScr->dimension.first > scrDim.first)
		return false;
	if (boxScr->offset.second + boxScr->dimension.second > scrDim.second)
		return false;
	return true;
}


/*
 * environment
 */

struct Env {
	void *display;
	std::vector<Box> boxes { 100 };
};


/*
 * curses
 */

static bool onEventCurses(const bool isFinal, void *event, void *data) {
	if (!isFinal)
		return true;
	if (event == nullptr)
		return true;
	if (data == nullptr) {
		std::printf("%s onEventCurses() display is null\n", LOG_FACILITY.c_str());
		return true;
	}
	const int c = *(const int*) event;
	if (c == ERR)
		return true;
	const Env *env = (const Env*) data;
	const DisplayXcb *display = (const DisplayXcb*) env->display;
	Context *context = display->getContext();
	context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventCurses", "%d %c", c, c);
	switch (c) {
	case 27:	// esc key
		return false;
		break;
	default:
		break;
	}
	return true;
}

static void onDrawCurses(const bool isFinal, void *data) {
	const Env *env = (const Env*) data;
	const DisplayCurses *display = (const DisplayCurses*) env->display;
	if (isFinal) {
		refresh();
		return;
	}
	erase();
	const std::pair<int,int> scrDim = display->screenDimension();
	Box boxScr;
	for (auto &box : env->boxes) {
		if (!scaleBox(box, &boxScr, scrDim))
			continue;
		mvaddstr(boxScr.offset.second, boxScr.offset.first, "#");
	}
}


/*
 * sdl
 */

static bool onEventSdl(const bool isFinal, void *event, void *data) {
	if (!isFinal)
		return true;
	if (event == nullptr)
		return true;
	if (data == nullptr) {
		std::printf("%s onEventSdl() display is null\n", LOG_FACILITY.c_str());
		return true;
	}
//	const Env *env = (const Env*) data;
//	const DisplayXcb *display = (const DisplayXcb*) env->display;
//	Context *context = display->getContext();
	const SDL_Event *e = (const SDL_Event*) event;
	switch (e->type) {
	case SDL_KEYDOWN:
		switch (e->key.keysym.sym) {
		case SDLK_ESCAPE:
			return false;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return true;
}

static void onDrawSdl(const bool isFinal, void *data) {
	const Env *env = (const Env*) data;
	const DisplaySdl *display = (const DisplaySdl*) env->display;
	SDL_Surface *screen = (SDL_Surface*) display->getScreen();
	if (isFinal) {
		SDL_UpdateRect(screen, 0, 0, 0, 0);
		return;
	}
	SDL_FillRect(screen, NULL, 0x00000000);
	const std::pair<int,int> scrDim = display->screenDimension();
	Box boxScr;
	int i = 17;
	for (auto &box : env->boxes) {
		if (!scaleBox(box, &boxScr, scrDim))
			continue;
		SDL_Rect r { (Sint16) boxScr.offset.first, (Sint16) boxScr.offset.second, (Uint16) boxScr.dimension.first,
		    (Uint16) boxScr.dimension.second };
		Uint32 col = SDL_MapRGB(screen->format, (i * 23) & 0xff, (i * 13) & 0xff, (i * 19) & 0xff);
		i+=31;
		SDL_FillRect(screen, &r, col);
//		SDL_UpdateRect(screen, r.x, r.y, r.w, r.h);
	}
}


/*
 * xcb
 */

static bool onEventXcb(const bool isFinal, void *event, void *data) {
	if (!isFinal)
		return true;
	if (event == nullptr)
		return true;
	if (data == nullptr) {
		std::printf("%s onEventXcb() display is null\n", LOG_FACILITY.c_str());
		return true;
	}
	const xcb_generic_event_t *e = (const xcb_generic_event_t*) event;
	const Env *env = (const Env*) data;
	const DisplayXcb *display = (const DisplayXcb*) env->display;
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
		switch (sym) {
		case 65307:	// esc key
			return false;
			break;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
	return true;
}

static void onDrawXcb(const bool isFinal, void *data) {
	const Env *env = (const Env*) data;
	const DisplayXcb *display = (const DisplayXcb*) env->display;
	if (isFinal) {
		xcb_flush(display->getConnection());
		return;
	}
	const std::pair<int,int> scrDim = display->screenDimension();
	xcb_rectangle_t rect { 0, 0, (uint16_t) scrDim.first, (uint16_t) scrDim.second };
	xcb_poly_fill_rectangle(display->getConnection(), display->getWindow(), display->getGContextInverse(), 1, &rect);
	Box boxScr;
	for (auto &box : env->boxes) {
		if (!scaleBox(box, &boxScr, scrDim))
			continue;
		rect.x = boxScr.offset.first;
		rect.y = boxScr.offset.second;
		rect.width = boxScr.dimension.first;
		rect.height = boxScr.dimension.second;
		xcb_poly_fill_rectangle(display->getConnection(), display->getWindow(), display->getGContext(), 1, &rect);
	}
}


/*
 * rendering
 */

static void onRender(void *data) {
	Env *env = (Env*) data;
	for (auto &box : env->boxes) {
		box.offset.first += box.velocity.first;
		if (box.offset.first < 0 || box.offset.first + box.dimension.first > boxFieldDim.first) {
			box.velocity.first *= -1;
			box.offset.first += 2 * box.velocity.first;
		}
		box.offset.second += box.velocity.second;
		if (box.offset.second < 0 || box.offset.second + box.dimension.second > boxFieldDim.second) {
			box.velocity.second *= -1;
			box.offset.first += 2 * box.velocity.second;
		}
	}
}


/*
 * main
 */

int main(int argc, char **argv) {

	Env env;

	// init boxes with random values
	std::srand(std::time(NULL));
	for (auto &box : env.boxes) {
		box.offset.first = std::rand() % (boxFieldDim.first - boxMaxDim);
		box.offset.second = std::rand() % (boxFieldDim.second - boxMaxDim);
		box.dimension.first = std::rand() % (boxMaxDim - 1) + 1;
		box.dimension.second = std::rand() % (boxMaxDim - 1) + 1;
		box.velocity.first = std::rand() % (boxMaxVel - 1) + 1;
		box.velocity.second = std::rand() % (boxMaxVel - 1) + 1;
	}

	// init gui
	Context context;
	Container root { &context };
	Button button1 { &root };
	Button button2 { &root };
	Button button3 { &root };

/*
	// curses
	WINDOW *w = DisplayCurses::initWindow();
	DisplayCurses display = { &context, w };
	env.display = &display;
	display.gameEventLoop(60, true, onEventCurses, onRender, onDrawCurses, &env);
//	display.applicationEventLoop(isQuitEventCurses, onEventCurses, &env);
*/

	// sdl
	SDL_Surface *scr = DisplaySdl::initScreen();
	DisplaySdl display { &context, scr };
	env.display = &display;
	display.gameEventLoop(60, true, onEventSdl, onRender, onDrawSdl, &env);
//	display.applicationEventLoop(isQuitEventSdl, onEventSdl, &env);

/*
	// xcb
	xcb_connection_t *cn = DisplayXcb::initConnection();
	xcb_screen_t *scr = DisplayXcb::initScreen(cn);
	xcb_window_t win = DisplayXcb::initWindow(cn, scr, nullptr, nullptr);
	xcb_font_t fn = DisplayXcb::initFont(cn);
	DisplayXcb display { &context, cn, scr, win, fn };
	env.display = &display;
	display.gameEventLoop(60, true, onEventXcb, onRender, onDrawXcb, &env);
//	display.applicationEventLoop(isQuitEventXcb, onEventXcb, &display);
*/
	return 0;
}

