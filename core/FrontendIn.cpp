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

#include "FrontendIn.hpp"

//#include "Component.hpp"
#include "Context.hpp"

static const std::basic_string<char> LOG_FACILITY = "FRONTEND_IN";


/*
 * ******************************************************** constructor / destructor
 */

FrontendIn::FrontendIn(Context &ctx) {
	context = &ctx;
	context->setFrontendIn(*this);

/*
	fpsTicksPrevious = 0;
	fpsFrameMillis = 0;
	fpsCyclesPerFrame = 0;
*/
}

FrontendIn::~FrontendIn() {
}


/*
 * ******************************************************** private
 */


/*
 * event handling
 */

// override e.g. to implement freeing allocated events, defaults to do nothing
void FrontendIn::eventFree(void *event) {
}


/*
 * ******************************************************** public
 */


/*
 * getter
 */

Context* FrontendIn::getContext() const {
	if (context == nullptr)
		std::printf("%s getContext() no context\n", LOG_FACILITY.c_str());
	return context;
}

