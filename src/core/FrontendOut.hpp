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

#ifndef SWF_CORE_FRONTEND_OUT
#define SWF_CORE_FRONTEND_OUT

#include <string>
#include <utility>

#include "Component.hpp"

class Context;

class FrontendOut {

private:
	Context *context;

protected:

public:
	FrontendOut(Context&);
	~FrontendOut();

	// getter
	Context* getContext() const;

	// drawing
//	virtual void drawBorder(const std::pair<int,int>&, const std::pair<int,int>&) const = 0;
//	virtual void drawText(const std::pair<int,int>&, const std::pair<int,int>&, const std::basic_string<char>&) const = 0;
//	virtual void draw(const std::pair<int,int>&, const std::pair<int,int>&, const std::basic_string<char>&) const = 0;
	virtual void draw(const Position&, const Style&, const std::basic_string<char>&) const = 0;
	virtual std::pair<int,int> screenDimension() const = 0;
	virtual std::pair<int,int> fontDimension() const = 0;
	virtual void gameLoopDrawFinish() const = 0;

};

#endif // SWF_CORE_FRONTEND_OUT
