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

#ifndef SWT_CORE_DISPLAY_SDL
#define SWT_CORE_DISPLAY_SDL

#include <string>
#include <utility>

#include <freetype2/ft2build.h>
#include <freetype2/freetype.h>
#include <SDL/SDL.h>


#include "Display.hpp"

class RootContainer;

class DisplaySdl : public Display {

private:
	static const int fontPanelFirstChar = 0x20; // first char: space
	static const int fontPanelLastChar = 0x7e; // last char: tilde
	static const int fontPanelCharCount = fontPanelLastChar - fontPanelFirstChar;

	static void drawPoint(SDL_Surface*, const std::pair<int,int>&, const Uint32);
	static void drawGlyph(SDL_Surface*, const FT_GlyphSlot, std::pair<int,int>&, std::pair<int,int>&);

	struct SDL_Surface *screen;

	struct SDL_Surface *fontPanel; // caches often used chars
	int fontPanelOffsets[fontPanelCharCount];
	int fontHeight;
	int fontWidthAvg;

	bool fontPanelChar(const char c, SDL_Rect*) const;

public:
	DisplaySdl();
	~DisplaySdl();

	void drawBorder(const std::pair<int,int>&, const std::pair<int,int>&) const override;
	void drawText(const std::pair<int,int>&, const std::basic_string<char>&) const override;

	std::pair<int,int> screenDimension() const override;
	std::pair<int,int> fontDimension() const override;

};

#endif // SWT_CORE_DISPLAY_SDL

