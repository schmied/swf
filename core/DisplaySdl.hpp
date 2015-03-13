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

#ifndef SWF_CORE_DISPLAY_SDL
#define SWF_CORE_DISPLAY_SDL

#include <string>
#include <utility>

#include <ft2build.h>
#ifdef __FreeBSD__
#include <freetype.h>
#endif
#ifdef WIN32
#include <freetype/freetype.h>
#endif
#include <SDL.h>

#include "Display.hpp"


class DisplaySdl : public Display {

private:
	struct SDL_Surface *screen;

	// font panel, caches often used chars for blitting
	static const int fontPanelFirstChar = 0x20;		// first char: space
	static const int fontPanelLastChar = 0x7e;		// last char: tilde
	static const int fontPanelCharCount = fontPanelLastChar - fontPanelFirstChar + 1;
	struct SDL_Surface *fontPanel;
	FT_Library fontLibrary;
	FT_Face fontFace;
	int fontPanelOffsets[fontPanelCharCount];
	int fontHeight;
	int fontSize;
	int fontWidthAvg;
	bool isFontPanelChar(const int c, SDL_Rect*) const;	// lookup char in font panel cache

	// drawing
	inline static void drawPoint(SDL_Surface*, const int, const int, const Uint32);
	static void drawLine(SDL_Surface*, int, int, int, int, const Uint32);
	void drawGlyph(SDL_Surface*, const FT_GlyphSlot, const int, const int, const Uint32) const;

	// event handling
	SDL_Event currentEvent;
	void* eventPoll() override;
	void* eventWait() override;
	void gameEventSleep() const override;
	long gameEventTicks() const override;

public:
	DisplaySdl(Context&, SDL_Surface*);
	~DisplaySdl();

	// getter
	SDL_Surface* getScreen() const;

	// drawing
	void drawBorder(const std::pair<int,int>&, const std::pair<int,int>&) const override;
	void drawText(const std::pair<int,int>&, const std::pair<int,int>&, const std::basic_string<char>&) const override;
	std::pair<int,int> screenDimension() const override;
	std::pair<int,int> fontDimension() const override;

	// event handling
	void handleEvent(void*) const override;

	// sdl helper
	static SDL_Surface* initScreen(); 

};

#endif // SWF_CORE_DISPLAY_SDL

