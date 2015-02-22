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

//#include "../core/Component.hpp"
#include "../core/Button.hpp"
#include "../core/Container.hpp"
#include "../core/Context.hpp"
#include "../core/DisplayCurses.hpp"
#include "../core/DisplayXcb.hpp"
#include "../core/DisplaySdl.hpp"
#include "../core/Widget.hpp"


static const std::basic_string<char> LOG_FACILITY = "EXAMPLE";

static bool isMaximumSpeed = false;

static bool handleEventSdl(Context &context, const SDL_Event *event) {
	switch (event->type) {
	case SDL_KEYDOWN:
		switch (event->key.keysym.sym) {
		case 's':
			isMaximumSpeed = !isMaximumSpeed;
			context.log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "maximum speed is %d", isMaximumSpeed);
			break;
		default:
			return false;
			break;
		}
		context.log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "typ %d sym %d", event->type, event->key.keysym.sym);
		break;
	default:
		return false;
		break;
	}
	return true;
}

static bool handleEventCurses(Context &context, const int c) {
	switch (c) {
	case 's':
		isMaximumSpeed = !isMaximumSpeed;
		context.log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "maximum speed is %d", isMaximumSpeed);
		break;
	default:
		return false;
		break;
	}
	context.log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "char %d", c);
	return true;
}

int main(int argc, char **argv) {

	Context context;

	Container root { &context };
	Button button1 { &root };
	Button button2 { &root };
	Button button3 { &root };

/*
	DisplayXcb display { &context, { 500, 1200 } };
	timespec ts;
	for (;;) {
		if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
			context.log(Context::LOG_DEBUG, LOG_FACILITY, "main", "clock gettime error");
		const long ticks = 1000 * ts.tv_sec + ts.tv_nsec / 1000 / 1000;
		const bool isElapsed = display.isTicksElapsed(ticks, 60);
		if (isElapsed) {
			display.resetTicks(ticks);
			// <-- event handling here
		}
		if (isElapsed || isMaximumSpeed) {
			// <-- my calc stuff here
		} 
		if (isElapsed) {
			// <-- my render stuff here
			context.draw();
		}
		if (!isElapsed && !isMaximumSpeed) {
			ts.tv_sec = 0;
			ts.tv_nsec = 1000 * 1000;
			nanosleep(&ts, NULL);
		}
	}
*/

	DisplayCurses display { &context };
	timespec ts;
	for (;;) {
		if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
			context.log(Context::LOG_DEBUG, LOG_FACILITY, "main", "clock gettime error");
		const long ticks = 1000 * ts.tv_sec + ts.tv_nsec / 1000 / 1000;
		const bool isElapsed = display.isTicksElapsed(ticks, 60);
		if (isElapsed) {
			display.resetTicks(ticks);
			// <-- event handling here
			const int c = getch();
			if (c == 27)
				break;
			if (!display.handleEvent(c))
				handleEventCurses(context, c);
		}
		if (isElapsed || isMaximumSpeed) {
			// <-- my calc stuff here
		} 
		if (isElapsed) {
			// <-- my render stuff here
			context.draw();
			refresh();
		}
		if (!isElapsed && !isMaximumSpeed) {
			ts.tv_sec = 0;
			ts.tv_nsec = 1000 * 1000;
			nanosleep(&ts, NULL);
		}
	}

/*
	DisplaySdl display { &context };
	display.resetTicks(SDL_GetTicks());
	SDL_Event event;
	for (;;) {
		const long ticks = SDL_GetTicks();
		const bool isElapsed = display.isTicksElapsed(ticks, 60);
		if (isElapsed) {
			display.resetTicks(ticks);
			if (SDL_PollEvent(&event)) {
				if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
					break;
				if (!display.handleEvent(&event))
					handleEventSdl(context, &event);
			}
		}
		if (isElapsed || isMaximumSpeed) {
			// <-- my calc stuff here
		} 
		if (isElapsed) {
			// <-- my render stuff here
			context.draw();
		}
		if (!isElapsed && !isMaximumSpeed)
			SDL_Delay(1);
	}
*/
	return 0;
}

