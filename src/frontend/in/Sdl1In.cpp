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

//#include <bitset>
//#include <cmath>
//#include <iostream>
//#include <utility>

#include <SDL/SDL.h>

#include "Sdl1In.hpp"

#include "../../core/Context.hpp"


static const std::basic_string<char> LOG_FACILITY = "SDL1_IN";


/*
 * ******************************************************** constructor
 */

Sdl1In::Sdl1In(Context &ctx) : FrontendIn(ctx) {
}

Sdl1In::~Sdl1In() {
	SWFLOG(getContext(), LOG_WARN, nullptr);
}


/*
 * ******************************************************** private
 */


/*
 * ******************************************************** public
 */


/*
 * event handling
 */

void* Sdl1In::eventPoll() {
	const int i = SDL_PollEvent(&currentEvent);
	if (!i)
		return nullptr;
	return &currentEvent;
}

void* Sdl1In::eventWait() {
	const int i = SDL_WaitEvent(&currentEvent);
	if (!i) {
		SWFLOG(getContext(), LOG_WARN, "sdl wait event error");
		return nullptr;
	}
	return &currentEvent;
}

void Sdl1In::in(void *event) const {
	if (event == nullptr)
		return;
	const SDL_Event *e = (const SDL_Event*) event;
	switch (e->type) {
	case SDL_KEYDOWN:
		SWFLOG(getContext(), LOG_DEBUG, "key %c %d", e->key.keysym.sym, e->key.keysym.sym);
		switch (e->key.keysym.sym) {
		case SDLK_RETURN:
			break;
		case SDLK_UP:
			break;
		case SDLK_DOWN:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

/*
 * game loop
 */

void Sdl1In::gameLoopSleep() const {
	SDL_Delay(1);	
}

long Sdl1In::gameLoopTicks() const {
	return 10;//SDL_GetTicks();
}

