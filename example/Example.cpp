/*
 * Copyright (c) 2013, 2014, 2016, Michael Schmiedgen
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

//#include "../core/Component.hpp"
#include "../core/Container.hpp"
#include "../core/Context.hpp"
#include "../core/DisplayCurses.hpp"
#include "../core/DisplayXcb.hpp"
#include "../core/DisplaySdl.hpp"
#include "../core/Widget.hpp"


static const std::basic_string<char> LOG_FACILITY = "EXAMPLE";


int main(int argc, char **argv) {

	Context context;

	Container root { &context };
	Widget widget1 { &root };
	Widget widget2 { &root };
	Widget widget3 { &root };

/*
	DisplayCurses display { &context };
	for (;;) {
	mvaddstr(0, 0, "bla\0");
	refresh();
		const int c = getch();
		if (c == 27)
			break;
		display.handleEvent(c);
		context.draw();
		refresh();
	}
*/

	DisplaySdl display { &context };
	SDL_Event event;
	for (;;) {
		SDL_PollEvent(&event);
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			break;
		display.handleEvent(&event);
		context.draw();
	}
	return 0;
}

