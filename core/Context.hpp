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

#ifndef SWF_CORE_CONTEXT
#define SWF_CORE_CONTEXT

#include <deque>
#include <map>
#include <string>

#include "Component.hpp"

//class Component;
class Container;
class FrontendIn;
class FrontendOut;

#define SWFLOG(X, Y, FMT, ...) (X)->log(Context::Y, LOG_FACILITY, __func__, FMT, ##__VA_ARGS__)

//enum LogLevel { LOG_DEBUG, LOG_INFO, LOG_WARN };

/*
class Binding {

private:
	void *value;
//	int valueInt;
//	double valueDouble;
//	std::string valueString;

public:
	static const enum Type { INT_VALUE, INT_FUNC, DOUBLE_VALUE, DOUBLE_FUNC, STRING_VALUE, STRING_FUNC };

	Binding();
	~Binding();

	Type typeGet;
	Type typeSet;

	int intValue();
	void intValue(const int);
	double doubleValue();
	void doubleValue(const double);
	std::string stringValue();
	void stringValue(const std::string);
};

Binding::Binding() {
}

Binding::~Binding() {
}

int Binding::intValue() {
	if (typeGet == INT_VALUE)
		return 3;
	return 0;
}

void Binding::intValue(const int value) {
}

double Binding::doubleValue() {
	return 0;
}

void Binding::doubleValue(const double value) {
}

std::string Binding::stringValue() {
	return 0;
}

void Binding::stringValue(const std::string value) {
}


class Hinge {

private:
	std::string componentName;
	std::map<std::string, Binding> assosiations;

public:
	Hinge();
	~Hinge();
};

Hinge::Hinge() {
}

Hinge::~Hinge() {
}


class Site {

private:
	std::map<std::string, Hinge> hinges;

public:
	Site();
	~Site();
};

Site::Site() {
}

Site::~Site() {
}
*/

class Context {

private:
	FrontendIn *frontendIn;
	FrontendOut *frontendOut;
	Container *rootContainer;

	std::deque<std::basic_string<char>*> logs;

	// drawing
	void drawComponents();
	static TraverseCondition onDrawComponent(Component*, void*);

	// fps statistics
	long fpsTicksPrevious;				// need to remember for elapsed ticks calculation
	int fpsCyclesPerFrame;				// number of event loop cycles per frame
	int fpsCyclesPerFrameCounter;
	int fpsFrameMillis;				// duration of a frame
	bool fpsIsTicksElapsed(const long, const long);
	void fpsResetTicks(const long);

public:
	enum LogLevel { LOG_DEBUG, LOG_INFO, LOG_WARN };

	Context();
	~Context();

	// getter / setter
	std::pair<int,int> getFpsStat() const;
	const FrontendIn* getFrontendIn();
	void setFrontendIn(FrontendIn*);
	const FrontendOut* getFrontendOut();
	void setFrontendOut(FrontendOut*);
	const Container* getRootContainer();
	void setRootContainer(Container*);

	// event
	void eventClick(const int, const int);
	void eventKey(const int);

	// loop
	int gameLoop(const int, const bool, int (*)(void*, void*), void (*)(void*), void (*)(void*), void*);
	int applicationLoop(int (*)(const bool, void*, void*), void*);

	// logging
	void log(const int, const std::basic_string<char>&, const std::basic_string<char>&, const char*...);
};

#endif // SWF_CORE_CONTEXT
