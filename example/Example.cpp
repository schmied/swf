/*
 * Copyright (c) 2013, Michael Schmiedgen
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

#include "../core/Context.hpp"
#include "../core/DisplayXcb.hpp"

int main(int argc, char **argv) {

	//Context context {};
	DisplayXcb display {100, 100};
	//context.setDisplay(&display);
	Component component {};
	//context.setComponentRoot(&component);

	for (;;) {
		sleep(1);

		display.drawComponent(component);
		display.drawEnd();
/*
		xvalues[0] = 10;
		xvalues[1] = 10;
		xcb_configure_window(xc, xwin, XCB_CONFIG_WINDOW_X |
		    XCB_CONFIG_WINDOW_Y, xvalues);

		xvalues[0] = XCB_STACK_MODE_ABOVE;
		xcb_configure_window(xc, xwin, XCB_CONFIG_WINDOW_STACK_MODE, xvalues);
*/
	}

	return 0;
}

