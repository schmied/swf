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

#ifndef SWF_FRONTEND_OUT_SDL2
#define SWF_FRONTEND_OUT_SDL2

#include <string>
#include <utility>

#ifdef __FreeBSD__
#include <ft2build.h>
#include <freetype/freetype.h>
#endif
#ifdef WIN32
#include <ft2build.h>
#include <freetype/freetype.h>
#endif
#include <SDL2/SDL.h>

#include "../../core/FrontendOut.hpp"


class Sdl2Out : public FrontendOut {

private:
	SDL_Window *window;
	SDL_Renderer *renderer;

	// font panel, caches often used chars for blitting
	static const int fontPanelFirstChar = 0x20;		// first char: space
	static const int fontPanelLastChar = 0x7e;		// last char: tilde
	static const int fontPanelCharCount = fontPanelLastChar - fontPanelFirstChar + 1;
	struct SDL_Texture *fontPanel;
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
	static void drawGlyph(SDL_Surface*, const FT_GlyphSlot, const int, const int, const Uint32);

public:
	Sdl2Out(Context&, SDL_Window*, SDL_Renderer*);
	~Sdl2Out();

	// getter
	SDL_Window* getWindow() const;
	SDL_Renderer* getRenderer() const;

	// drawing
//	void drawBorder(const std::pair<int,int>&, const std::pair<int,int>&) const override;
//	void drawText(const std::pair<int,int>&, const std::pair<int,int>&, const std::basic_string<char>&) const override;
	void draw(const Position&, const Style&, const std::basic_string<char>&) const override;
	std::pair<int,int> screenDimension() const override;
	std::pair<int,int> fontDimension() const override;
	void gameLoopDrawFinish() const override;

	// sdl helper
	static SDL_Window* initWindow();
	static SDL_Renderer* initRenderer(SDL_Window*);

};

#endif // SWF_FRONTEND_OUT_SDL2
