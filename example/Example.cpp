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
#include <memory>

//#include "../core/Component.hpp"
#include "../core/Button.hpp"
#include "../core/Container.hpp"
#include "../core/Context.hpp"
#include "../core/Widget.hpp"

#ifdef __FreeBSD__
#define SWF_HAS_CURSES
#define SWF_HAS_XCB
#endif
//#define SWF_HAS_SDL
#define SWF_HAS_SDL2


static const std::basic_string<char> LOG_FACILITY = "EXAMPLE";


/*
 * boxes
 */

static const std::pair<int,int> boxFieldDim { 16000, 10000 };
static const int boxMaxDim = (boxFieldDim.first + boxFieldDim.second) / 2 / 10;
static const int boxMaxVel = 4 * boxMaxDim / 100;
class Box {
public:
	Box() { };
	~Box() { };
	std::pair<int,int> offset;
	std::pair<int,int> dimension;
	std::pair<int,int> velocity;
};

static void randomizeBox(Box &box) {
	box.offset.first = std::rand() % (boxFieldDim.first - boxMaxDim);
	box.offset.second = std::rand() % (boxFieldDim.second - boxMaxDim);
	box.dimension.first = std::rand() % (boxMaxDim - 1) + 1;
	box.dimension.second = std::rand() % (boxMaxDim - 1) + 1;
	box.velocity.first = std::rand() % (boxMaxVel - 1) + 1;
	box.velocity.second = std::rand() % (boxMaxVel - 1) + 1;
}

// scale box to screen positions
static bool scaleBox(const Box &box, Box &boxScr, const std::pair<int,int> &scrDim) {
	const float scaleX = (float) scrDim.first / boxFieldDim.first;
	const float scaleY = (float) scrDim.second / boxFieldDim.second;
	boxScr.offset.first = (int) (scaleX * box.offset.first);
	boxScr.offset.second = (int) (scaleY * box.offset.second);
	boxScr.dimension.first = (int) (scaleX * box.dimension.first);
	if (boxScr.dimension.first < 1)
		boxScr.dimension.first = 1;
	boxScr.dimension.second = (int) (scaleY * box.dimension.second);
	if (boxScr.dimension.second < 1)
		boxScr.dimension.second = 1;
	if (boxScr.offset.first < 0)
		return false;
	if (boxScr.offset.second < 0)
		return false;
	if (boxScr.offset.first + boxScr.dimension.first > scrDim.first)
		return false;
	if (boxScr.offset.second + boxScr.dimension.second > scrDim.second)
		return false;
	return true;
}


/*
 * environment
 */

struct Env {
	Context *context;
	std::vector<void*>initData;
	std::vector<std::pair<int (*)(Env&),void(*)(Env&)>> startFunctions;
	std::vector<std::unique_ptr<Box>> boxes;
};

enum ExitCode {
	QUIT		= -1,
	NEXT_DISPLAY	= 1,
};

static void addBoxes(Env &e) {
	int cnt = e.boxes.size();
	if (cnt > 10000)
		return;
	cnt /= 4;
	if (cnt < 1)
		cnt = 1;
	for (int i = 0; i < cnt; i++) {
		std::unique_ptr<Box> b = std::make_unique<Box>();
		Box *box = b.get();
		box->offset.first = std::rand() % (boxFieldDim.first - boxMaxDim);
		box->offset.second = std::rand() % (boxFieldDim.second - boxMaxDim);
		box->dimension.first = std::rand() % (boxMaxDim - 1) + 1;
		box->dimension.second = std::rand() % (boxMaxDim - 1) + 1;
		box->velocity.first = std::rand() % (boxMaxVel - 1) + 1;
		box->velocity.second = std::rand() % (boxMaxVel - 1) + 1;
		e.boxes.push_back(std::move(b));
	}
	e.context->log(Context::LOG_DEBUG, LOG_FACILITY, "addBoxes", "box count %d", e.boxes.size());
}

static void removeBoxes(Env &e) {
	int cnt = e.boxes.size();
	if (cnt < 1)
		return;
	cnt /= 4;
	if (cnt < 1)
		cnt = 1;
	for (int i = 0; i < cnt; i++)
		e.boxes.pop_back();
	e.context->log(Context::LOG_DEBUG, LOG_FACILITY, "removeBoxes", "box count %d", e.boxes.size());
}


/*
 * rendering
 */

static void onRender(void *data) {
	Env *env = (Env*) data;
	for (auto &b : env->boxes) {
		auto box = b.get();
		box->offset.first += box->velocity.first;
		if (box->offset.first < 0 || box->offset.first + box->dimension.first > boxFieldDim.first) {
			box->velocity.first *= -1;
			box->offset.first += 2 * box->velocity.first;
		}
		box->offset.second += box->velocity.second;
		if (box->offset.second < 0 || box->offset.second + box->dimension.second > boxFieldDim.second) {
			box->velocity.second *= -1;
			box->offset.second += 2 * box->velocity.second;
		}
	}
}


/*
 * curses
 */

#ifdef SWF_HAS_CURSES

#include <curses.h>

#include "../core/DisplayCurses.hpp"

static int onEventCurses(const bool isFinal, void *event, void *data) {
	if (!isFinal || event == nullptr || data == nullptr)
		return 0;
	Env *env = (Env*) data;
	Context *context = env->context;
//	const DisplayXcb *display = (const DisplayXcb*) context->getDisplay();
	const int c = *(const int*) event;
	if (c == ERR) {
		context->log(Context::LOG_WARN, LOG_FACILITY, "onEventCurses", "input char == ERR");
		return 0;
	}
	context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventCurses", "%d %c", c, c);
	switch (c) {
	case 27:	// esc key
		return ExitCode::QUIT;
	case 276:	// f12 key
		return ExitCode::NEXT_DISPLAY;
	case '+':
		addBoxes(*env);
		break;
	case '-':
		removeBoxes(*env);
		break;
	default:
		break;
	}
	return 0;
}

static void onDrawCurses(const bool isFinal, void *data) {
	if (isFinal) {
		refresh();
		return;
	}
	const Env *env = (const Env*) data;
	Context *context = env->context;
	const DisplayCurses *display = (const DisplayCurses*) context->getDisplay();
	erase();
	const std::pair<int,int> scrDim = display->screenDimension();
	Box boxScr;
	char buf[100];
	int i = 0;
	for (int i = 0; i < 8; i++)
		init_pair(i, i & 7, (i + 5) & 7);
	for (auto &box : env->boxes) {
		i++;
		if (!scaleBox(*box.get(), boxScr, scrDim))
			continue;
		std::memset(buf, 0, 100);
		std::memset(buf, '#', boxScr.dimension.first);
		attron(COLOR_PAIR(i & 7));
		for (int y = 0; y < boxScr.dimension.second; y++) 
			mvaddstr(boxScr.offset.second + y, boxScr.offset.first, buf);
	}
	for (int i = 0; i < 8; i++)
		attroff(COLOR_PAIR(i));
}

static int startCurses(Env &env) {
	WINDOW *w = DisplayCurses::initWindow();
	DisplayCurses display = { *env.context, w };
	return display.gameEventLoop(60, true, onEventCurses, onRender, onDrawCurses, &env);
//	display.applicationEventLoop(isQuitEventCurses, onEventCurses, &env);
}

static void finishCurses(Env &env) {
	endwin();
}

#endif // SWF_HAS_CURSES


/*
 * sdl
 */

#ifdef SWF_HAS_SDL

#include <SDL/SDL.h>

#include "../core/DisplaySdl.hpp"

static int onEventSdl(const bool isFinal, void *event, void *data) {
	if (!isFinal || event == nullptr || data == nullptr)
		return 0;
	Env *env = (Env*) data;
	Context *context = env->context;
//	const DisplaySdl *display = (const DisplaySdl*) context->getDisplay();
	const SDL_Event *e = (const SDL_Event*) event;
	switch (e->type) {
	case SDL_KEYDOWN:
		switch (e->key.keysym.sym) {
		case SDLK_ESCAPE:
			return ExitCode::QUIT;
		case 293:	// f12 key
			return ExitCode::NEXT_DISPLAY;
		case '+':
			addBoxes(*env);
			break;
		case '-':
			removeBoxes(*env);
			break;
		default:
			context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventSdl", "key press %c %d",
			    e->key.keysym.sym, e->key.keysym.sym);
			break;
		}
		break;
	default:
		break;
	}
	return 0;
}

static void onDrawSdl(const bool isFinal, void *data) {
	const Env *env = (const Env*) data;
	Context *context = env->context;
	const DisplaySdl *display = (const DisplaySdl*) context->getDisplay();
	SDL_Surface *screen = (SDL_Surface*) display->getScreen();
	if (isFinal) {
		SDL_Flip(screen);
		return;
	}
	SDL_FillRect(screen, NULL, 0x00000000);
	const std::pair<int,int> scrDim = display->screenDimension();
	Box boxScr;
	int i = 17;
	for (auto &box : env->boxes) {
		i+=31;
		if (!scaleBox(*box, boxScr, scrDim))
			continue;
		SDL_Rect r { (Sint16) boxScr.offset.first, (Sint16) boxScr.offset.second, (Uint16) boxScr.dimension.first,
		    (Uint16) boxScr.dimension.second };
		Uint32 col = SDL_MapRGB(screen->format, ((i + 3) * 23) & 0xff, ((i + 5) * 13) & 0xff, (i * 19) & 0xff);
		SDL_FillRect(screen, &r, col);
	}
}

static int startSdl(Env &env) {
	SDL_Surface *scr = DisplaySdl::initScreen();
	DisplaySdl display { *env.context, scr };
	return display.gameEventLoop(60, true, onEventSdl, onRender, onDrawSdl, &env);
//	display.applicationEventLoop(isQuitEventSdl, onEventSdl, &env);
//	env.initData.push_back(scr);
}

static void finishSdl(Env &env) {
	SDL_Quit();
}

#endif // SWF_HAS_SDL


/*
 * sdl2
 */

#ifdef SWF_HAS_SDL2

#include <SDL2/SDL.h>

#include "../core/DisplaySdl.hpp"

static int onEventSdl(const bool isFinal, void *event, void *data) {
	if (!isFinal || event == nullptr || data == nullptr)
		return 0;
	Env *env = (Env*) data;
	Context *context = env->context;
//	const DisplaySdl *display = (const DisplaySdl*) context->getDisplay();
	const SDL_Event *e = (const SDL_Event*) event;
	switch (e->type) {
	case SDL_KEYDOWN:
		switch (e->key.keysym.sym) {
		case SDLK_ESCAPE:
			return ExitCode::QUIT;
		case 293:	// f12 key
			return ExitCode::NEXT_DISPLAY;
		case '+':
			addBoxes(*env);
			break;
		case '-':
			removeBoxes(*env);
			break;
		default:
			context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventSdl", "key press %c %d",
			    e->key.keysym.sym, e->key.keysym.sym);
			break;
		}
		break;
	default:
		break;
	}
	return 0;
}

static void onDrawSdl(const bool isFinal, void *data) {
	const Env *env = (const Env*) data;
	Context *context = env->context;
	const DisplaySdl *display = (const DisplaySdl*) context->getDisplay();
	SDL_Surface *screen = (SDL_Surface*) display->getScreen();
	if (isFinal) {
		SDL_Flip(screen);
		return;
	}
	SDL_FillRect(screen, NULL, 0x00000000);
	const std::pair<int,int> scrDim = display->screenDimension();
	Box boxScr;
	int i = 17;
	for (auto &box : env->boxes) {
		i+=31;
		if (!scaleBox(*box, boxScr, scrDim))
			continue;
		SDL_Rect r { (Sint16) boxScr.offset.first, (Sint16) boxScr.offset.second, (Uint16) boxScr.dimension.first,
		    (Uint16) boxScr.dimension.second };
		Uint32 col = SDL_MapRGB(screen->format, ((i + 3) * 23) & 0xff, ((i + 5) * 13) & 0xff, (i * 19) & 0xff);
		SDL_FillRect(screen, &r, col);
	}
}

static int startSdl(Env &env) {
	SDL_Surface *scr = DisplaySdl::initScreen();
	DisplaySdl display { *env.context, scr };
	return display.gameEventLoop(60, true, onEventSdl, onRender, onDrawSdl, &env);
//	display.applicationEventLoop(isQuitEventSdl, onEventSdl, &env);
//	env.initData.push_back(scr);
}

static void finishSdl(Env &env) {
	SDL_Quit();
}

#endif // SWF_HAS_SDL2


/*
 * xcb
 */

#ifdef SWF_HAS_XCB

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

#include "../core/DisplayXcb.hpp"

static int onEventXcb(const bool isFinal, void *event, void *data) {
	if (!isFinal || event == nullptr || data == nullptr)
		return 0;
	Env *env = (Env*) data;
	Context *context = env->context;
	const DisplayXcb *display = (const DisplayXcb*) context->getDisplay();
	const xcb_generic_event_t *e = (const xcb_generic_event_t*) event;
	switch (e->response_type & ~0x80) {
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
			return ExitCode::QUIT;
		case 65481:	// f12 key
			return ExitCode::NEXT_DISPLAY;
		case '+':
			addBoxes(*env);
			break;
		case '-':
			removeBoxes(*env);
			break;
		default:
			context->log(Context::LOG_DEBUG, LOG_FACILITY, "onEventXcb", "key press %c %d", sym, sym);
			break;
		}
		break;
	}
	default:
		break;
	}
	return 0;
}

static void onDrawXcb(const bool isFinal, void *data) {
	const Env *env = (const Env*) data;
	Context *context = env->context;
	const DisplayXcb *display = (const DisplayXcb*) context->getDisplay();
	if (isFinal) {
		xcb_flush(display->getConnection());
		return;
	}
	const std::pair<int,int> scrDim = display->screenDimension();
	xcb_rectangle_t rect { 0, 0, (uint16_t) scrDim.first, (uint16_t) scrDim.second };
	xcb_poly_fill_rectangle(display->getConnection(), display->getWindow(), display->getGContextInverse(), 1, &rect);
	Box boxScr;
	for (auto &box : env->boxes) {
		if (!scaleBox(*box, boxScr, scrDim))
			continue;
		rect.x = boxScr.offset.first;
		rect.y = boxScr.offset.second;
		rect.width = boxScr.dimension.first;
		rect.height = boxScr.dimension.second;
		xcb_poly_fill_rectangle(display->getConnection(), display->getWindow(), display->getGContext(), 1, &rect);
	}
}

static int startXcb(Env &env) {
	xcb_connection_t *cn = DisplayXcb::initConnection();
	xcb_screen_t *scr = DisplayXcb::initScreen(cn);
	xcb_window_t win = DisplayXcb::initWindow(cn, scr, nullptr, nullptr);
	xcb_font_t fn = DisplayXcb::initFont(cn);
	DisplayXcb display { *env.context, cn, scr, win, fn };
	return display.gameEventLoop(60, true, onEventXcb, onRender, onDrawXcb, &env);
//	display.applicationEventLoop(isQuitEventXcb, onEventXcb, &display);
}

static void finishXcb(Env &env) {
}

#endif // SWF_HAS_XCB


/*
 * main
 */

#if defined (_WINDOWS) && !defined (SWF_HAS_SDL) && !defined (SWF_HAS_SDL2)
#include <windows.h>
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow) {
#else
int main(int argc, char **argv) {
#endif

	Env env;

	// init gui
	Context context;
	env.context = &context;
	Container root {&context};
	Button button1 {&root};
	Button button2 {&root};
	Button button3 {&root};

	// register start functions
#ifdef SWF_HAS_CURSES
	env.startFunctions.push_back({ startCurses, finishCurses });
#endif
#ifdef SWF_HAS_SDL
	env.startFunctions.push_back({startSdl, finishSdl});
#endif
#ifdef SWF_HAS_XCB
	env.startFunctions.push_back({ startXcb, finishXcb });
#endif

	if (env.startFunctions.empty()) {
		context.log(Context::LOG_WARN, LOG_FACILITY, "main", "no start functions");
		return -1;
	}

	std::srand((unsigned int) std::time(NULL));
	while (env.boxes.size() < 10)
		addBoxes(env);

	int funcIdx = 0;
	bool run = true;
	while (run) {
		const int exitCode = env.startFunctions[funcIdx].first(env);
		switch (exitCode) {
		case ExitCode::QUIT:
			env.startFunctions[funcIdx].second(env);
			run = false;
			break;
		case ExitCode::NEXT_DISPLAY:
			env.startFunctions[funcIdx].second(env);
			funcIdx++;
			if (funcIdx >= (int) env.startFunctions.size())
				funcIdx = 0;
			break;
		}
	}

	return 0;
}
