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

//#include "../core/Component.hpp"
#include "../core/DisplayCurses.hpp"
#include "../core/DisplayXcb.hpp"
#include "../core/RootContainer.hpp"
#include "../core/Widget.hpp"

int main(int argc, char **argv) {

//	DisplayXcb display {{100, 100}};
	DisplayCurses display {};

	RootContainer root { &display };
	Widget widget1 { &root };
	Widget widget2 { &root };
	Widget widget3 { &root };

	for (;;) {
		root.draw();

		sleep(1);
	}

	sleep(3);

	return 0;
}

