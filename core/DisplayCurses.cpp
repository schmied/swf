/*
 * Copyright (c) 2014, 2015, Michael Schmiedgen
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

#include "DisplayCurses.hpp"

#include <iostream>

#include <curses.h>

#include "Component.hpp"
#include "Context.hpp"


static const std::basic_string<char> LOG_FACILITY = "DISPLAY_CURSES";


/*
 * ******************************************************** constructor / destructor
 */

DisplayCurses::DisplayCurses(Context &c, WINDOW *w) : Display(c) {
	window = w;
}

DisplayCurses::~DisplayCurses() {
	getContext()->log(Context::LOG_INFO, LOG_FACILITY, "<free>", nullptr);
//	endwin();
}


/*
 * ******************************************************** private
 */


/*
 * event handling
 */

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


/*
 * ******************************************************** public
 */


/*
 * drawing
 */

void DisplayCurses::drawBorder(const std::pair<int,int> &offset, const std::pair<int,int> &dimension) const {
}

void DisplayCurses::drawText(const std::pair<int,int> &offset, const std::pair<int,int> &dimension,
	    const std::basic_string<char> &text) const {
	if ((int) text.length() > dimension.first) {
		auto s = text.substr(0, dimension.first);
		mvaddstr(offset.second, offset.first, s.c_str());
		return;
	}
	mvaddstr(offset.second, offset.first, text.c_str());
	if ((int) text.length() < dimension.first) {
		std::basic_string<char> s;
		s.append(dimension.first - text.length(), ' ');
		mvaddstr(offset.second, offset.first + text.length(), s.c_str());
	}
}

std::pair<int,int> DisplayCurses::screenDimension() const {
	int x, y;
	getmaxyx(window, y, x);
	return { x, y };
}

static const std::pair<int,int> fontDim { 1, 1 };

std::pair<int,int> DisplayCurses::fontDimension() const {
	return fontDim;
}


/*
 * event handling
 */

void DisplayCurses::handleEvent(void *event) const {
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


/*
 * curses helper
 */

WINDOW* DisplayCurses::initWindow() {
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

