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

#ifndef SWF_FRONTEND_IN_SDL2
#define SWF_FRONTEND_IN_SDL2

//#include <string>
//#include <utility>

#include <SDL2/SDL.h>

#include "../../core/FrontendIn.hpp"


class Sdl2In : public FrontendIn {

private:
	// event handling
	SDL_Event currentEvent;

public:
	Sdl2In(Context&);
	~Sdl2In();

	// event handling
	void* eventPoll() override;
	void* eventWait() override;
	void in(void*) const override;

	// game loop
	void gameLoopSleep() const override;
	long gameLoopTicks() const override;

};

#endif // SWF_FRONTEND_IN_SDL2