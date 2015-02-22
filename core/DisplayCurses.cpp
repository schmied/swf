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
 * constructor / destructor
 */

DisplayCurses::DisplayCurses(Context *c) : Display(c) {
	window = initscr();
	nodelay(window, TRUE); // do not block on getch()
	cbreak();
	noecho();
	nonl();
	intrflush(window, FALSE);
	keypad(window, TRUE);
	meta(window, TRUE);
	raw();
	scrollok(window, FALSE);
	erase();
	refresh();
}

DisplayCurses::~DisplayCurses() {
	endwin();
}


/*
 * public
 */

bool DisplayCurses::handleEvent(const int c) const {
	switch (c) {
	case 8: // BS (backspace)
		break;
	case 10: // NL (newline)
	case 13: // CR (carriage return)
		break;
	case 19: // control+s
		break;
	case 27: // ESC (escape)
		break;
	case 127: // DEL (delete)
		break;
	case KEY_DC:
		break;
	case KEY_LEFT: // cursor left
		break;
	case KEY_RIGHT: // cursor right
		break;
	case KEY_UP: // cursor up
		break;
	case KEY_DOWN: // cursor down
		break;
	default:
		return false;
		break;
	}
	getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "char %d", c);
	return true;
}

void DisplayCurses::drawBorder(const std::pair<int,int> &offset, const std::pair<int,int> &dimension) const {
}

void DisplayCurses::drawText(const std::pair<int,int> &offset, const std::pair<int,int> &dimension,
	    const std::basic_string<char> &text) const {
	mvaddstr(offset.second, offset.first, text.c_str());
//	refresh();
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

