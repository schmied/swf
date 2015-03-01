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

#ifndef SWF_CORE_CONTEXT
#define SWF_CORE_CONTEXT

#include <deque>
#include <string>

class Component;
class Display;
class Container;

//enum LogLevel { LOG_DEBUG, LOG_INFO, LOG_WARN };

class Context {

private:
	Display *display;
	Container *rootContainer;

	std::deque<std::basic_string<char>*> logs;

	// drawing
	static void onDraw(Component*, void*);

public:
	enum LogLevel { LOG_DEBUG, LOG_INFO, LOG_WARN };

	Context();
	~Context();

	// getter / setter
	const Display* getDisplay();
	void setDisplay(Display*);
	const Container* getRootContainer();
	void setRootContainer(Container*);

	// drawing
	void draw();

	// logging
	void log(const int, const std::basic_string<char>&, const std::basic_string<char>&, const char*...);
};

#endif // SWF_CORE_CONTEXT

