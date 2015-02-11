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

/*
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <time.h>
*/
#include <unistd.h>

#include <iostream>

#include <SDL/SDL.h>

//#include "../core/Component.hpp"
#include "../core/DisplayCurses.hpp"
#include "../core/DisplayXcb.hpp"
#include "../core/DisplaySdl.hpp"
#include "../core/RootContainer.hpp"
#include "../core/Widget.hpp"

static SDL_Event event;

SDL_Event* poll_event(RootContainer *root) {
	if (!SDL_PollEvent(&event)) {
//		std::cout << ">>-- NO" << std::endl;
//		root->log(">>-- NO");
		return NULL;
	}
	root->log(">>-- %d", event.type);
//	std::cout << (int) event.type << std::endl;
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			break;
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
	return &event;
}

bool isTerminate(SDL_Event *e, RootContainer *c) {
	if (e == NULL)
		return false;
	switch (e->type) {
	case SDL_KEYDOWN:
		switch (e->key.keysym.sym) {
		case SDLK_ESCAPE:
			return true;
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
	return false;

}

int main(int argc, char **argv) {

//	DisplayXcb display {{100, 100}};
//	DisplayCurses display {};
	DisplaySdl display {};

	RootContainer root { &display };
	Widget widget1 { &root };
	Widget widget2 { &root };
	Widget widget3 { &root };

	for (;;) {
		SDL_Event *e = poll_event(&root);
		if (e != NULL) {
//			root.log("->> %hhu %i", e->type, 100);
//			root.log("->> %u %i", e->type, 100);
//			root.log("->> %3s", "blaahh hehe");
		}
		if (isTerminate(e, &root)) {
			root.log("terminate");
			break;
		}
		root.draw();
	}

//	sleep(3);

	return 0;
}

