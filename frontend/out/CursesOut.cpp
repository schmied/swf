/*
 * Copyright (c) 2014, 2015, 2016, Michael Schmiedgen
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

#include "CursesOut.hpp"

//#include "Component.hpp"
#include "../../core/Context.hpp"


static const std::basic_string<char> LOG_FACILITY = "CURSES_OUT";


/*
 * ******************************************************** constructor / destructor
 */

CursesOut::CursesOut(Context &ctx, WINDOW *w) : FrontendOut(ctx) {
	window = w;
}

CursesOut::~CursesOut() {
	getContext()->log(Context::LOG_INFO, LOG_FACILITY, "<free>", nullptr);
//	endwin();
}


/*
 * ******************************************************** private
 */


/*
 * event handling
 */

/*
void* DisplayCurses::eventPoll() {
	currentEvent = getch();
	if (currentEvent == ERR)
		return nullptr;
	return &currentEvent;
}

void* DisplayCurses::eventWait() {
	currentEvent = getch();
	if (currentEvent == ERR)
		return nullptr;
	return &currentEvent;
}

void DisplayCurses::gameEventSleep() const {
	timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 1000 * 1000;
	nanosleep(&ts, NULL);
}

long DisplayCurses::gameEventTicks() const {
	timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "fpsTicks", "clock gettime error");
	return 1000L * ts.tv_sec + ts.tv_nsec / 1000L / 1000L;
}
*/


/*
 * ******************************************************** public
 */


/*
 * drawing
 */

void CursesOut::draw(const Position &pos, const Style &stl, const std::basic_string<char> &text) const {
	if ((int) text.length() > pos.w) {
		auto s = text.substr(0, pos.w);
		mvaddstr(pos.textY, pos.textX, s.c_str());
		return;
	}
	mvaddstr(pos.textY, pos.textX, text.c_str());
	if ((int) text.length() < pos.w) {
		std::basic_string<char> s;
		s.append(pos.w - text.length(), ' ');
		mvaddstr(pos.textY, pos.textX + text.length(), s.c_str());
	}
}

std::pair<int,int> CursesOut::screenDimension() const {
	int x, y;
	getmaxyx(window, y, x);
	return { x, y };
}

static const std::pair<int,int> fontDim { 1, 1 };

std::pair<int,int> CursesOut::fontDimension() const {
	return fontDim;
}


/*
 * event handling
 */
/*
void CursesOut::handleEvent(void *event) const {
	((Component*) getContext()->getRootContainer())->invalidatePosition();
	if (event == nullptr)
		return;
	const int c = *(const int*) event;
//	getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "char %d", c);
	switch (c) {
	case 8:			// BS (backspace)
		break;
	case 10:		// NL (newline)
	case 13:		// CR (carriage return)
		break;
	case 19:		// control+s
		break;
//	case 27:		// ESC (escape)
//		break;
	case 127:		// DEL (delete)
		break;
	case KEY_DC:
		break;
	case KEY_LEFT:		// cursor left
		break;
	case KEY_RIGHT:		// cursor right
		break;
	case KEY_UP:		// cursor up
		break;
	case KEY_DOWN:		// cursor down
		break;
	default:
		break;
	}
}
*/

/*
 * curses helper
 */

WINDOW* CursesOut::initWindow() {
	WINDOW *w = initscr();
	start_color();
	use_default_colors();
	nodelay(w, TRUE); // do not block on getch()
	cbreak();
	noecho();
	nonl();
	intrflush(w, FALSE);
	keypad(w, TRUE);
	meta(w, TRUE);
	raw();
	scrollok(w, FALSE);
	erase();
	refresh();
	return w;
}

