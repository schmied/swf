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

#include <iostream>
#include <utility>

#include <freetype2/ft2build.h>
#include <freetype2/freetype.h>
#include <SDL/SDL.h>

#include "DisplaySdl.hpp"

#include "Component.hpp"
#include "RootContainer.hpp"

//static SDL_Rect rectBorder;
static FT_Library library;
static FT_Face face;
//static SDL_Surface *fontSurface;


/*
 * constructor
 */

DisplaySdl::DisplaySdl() {
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		// XXX log SDL_GetError()
		return;
	}
	screen = SDL_SetVideoMode(0, 0, 0, SDL_ANYFORMAT);
	if (screen == NULL) {
		// XXX log SDL_GetError()
		std::cout << "set video mode error " << SDL_GetError() << std::endl;
		return;
	}

	int error = FT_Init_FreeType(&library);
	if (error) {
		// XXX log
		std::cout << "init freetype error " << error << std::endl;
		return;
	}
	error = FT_New_Face(library, "/usr/local/lib/X11/fonts/75dpi/term14.pcf.gz", 0, &face);
	if (error) {
		// XXX log
		std::cout << "new face error " << error << std::endl;
		return;
	}

	fontHeight = 14;
	error = FT_Set_Pixel_Sizes(face, 0, fontHeight);
	if (error) {
		// XXX log
		std::cout << "set pixel size error " << error << std::endl;
		return;
	}

	// determine font panel offsets
	int lastXOffset = 0;
	for (int i = 0; i < fontPanelCharCount; i++) {
		const char c = fontPanelFirstChar + (char) i;
		error = FT_Load_Char(face, c, FT_LOAD_RENDER);
		if (error) {
			// XXX log
			std::cout << "load char error " << error << std::endl;
		}
		if (i > 0)
			fontPanelOffsets[i-1] = lastXOffset;
		lastXOffset += face->glyph->bitmap.width;
		if (i == fontPanelCharCount - 1)
			fontPanelOffsets[fontPanelCharCount-1] = lastXOffset;

//		if (i > 0)
//			std::cout << c << " " << fontPanelOffsets[i-1] << "   ";
	}

	// create font panel
	const int screenWidth = fontPanelOffsets[fontPanelCharCount-1] + 10;
	const int screenHeight = fontHeight + 10;
	std::cout << "panel " << screenWidth << "x" << screenHeight << std::endl;
	const SDL_PixelFormat *fmt = screen->format;
	fontPanel = SDL_CreateRGBSurface(screen->flags, screenWidth, screenHeight, fmt->BitsPerPixel,
	    fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	if (fontPanel == NULL) {
		// XXX log SDL_GetError()
		std::cout << "create font surface error" << SDL_GetError() << std::endl;
		return;
	}

	// populate font panel
	for (int i = 0; i < fontPanelCharCount; i++) {
		const char c = fontPanelFirstChar + i;
		error = FT_Load_Char(face, c, FT_LOAD_RENDER);
		if (error) {
			// XXX log
			std::cout << "load char error " << error << std::endl;
		}
		const FT_Bitmap bitmap = face->glyph->bitmap;
		int x = 0;
		if (i > 0)
			x = fontPanelOffsets[i-1];
		drawGlyph(fontPanel, face->glyph, x, 0);
		drawGlyph(screen, face->glyph, x, 500);
	}
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

DisplaySdl::~DisplaySdl() {
	SDL_Quit();
	std::cout << "displaysdl terminated." << std::endl;
}


/*
 * private
 */

void DisplaySdl::drawPoint(SDL_Surface *dst, const Uint16 x, const Uint16 y, const Uint32 color) {
	const Uint8 bpp = dst->format->BytesPerPixel;
	void *pos = (Uint8*) dst->pixels + (y * dst->pitch + x * bpp);
	switch (bpp) {
	case 1:
		*(Uint8*) pos = color;	
		break;
	case 2:
		*(Uint16*) pos = color;	
		break;
	case 4:
		*(Uint32*) pos = color;	
		break;
	default:
		// XXX log
		break;
	}
}

void DisplaySdl::drawGlyph(SDL_Surface *dst, const FT_GlyphSlot glyph, const Uint16 offsetX, const Uint16 offsetY) {
	const FT_Bitmap bitmap = glyph->bitmap;	
	const Uint16 width = bitmap.width;
	SDL_LockSurface(dst);
	for (Uint16 y = 0; y < bitmap.rows; y++) {
		int bufferIndex = y * bitmap.pitch;
		Uint16 x = 0;
		while (x < width) {
			const unsigned char c = bitmap.buffer[bufferIndex];
			const std::bitset<8> bits = { (const unsigned long long) c };
			for (size_t i = 0; i < bits.size() && x < width; i++) {
				if (bits[i]) {
					drawPoint(dst, offsetX + width - x, offsetY + y, 0x0000ff00);
//					std::cout << "*";
				} else {
//					std::cout << " ";
				}
				x++;
			}
			bufferIndex++;
		}
//		std::cout << "|" << std::endl;
	}
	SDL_UnlockSurface(dst);

}

bool DisplaySdl::fontPanelChar(const char c, SDL_Rect *dimension) const {
	if (c < fontPanelFirstChar || c > fontPanelLastChar)
		return false;
	const int idx = c - fontPanelFirstChar;
	if (idx == 0)
		dimension->x = 0;
	else
		dimension->x = fontPanelOffsets[idx-1];
	dimension->y = 0;
	dimension->w = fontPanelOffsets[idx] - dimension->x;
	dimension->h = 14;//face->height;//size->metrics.height;
	return true;
}


/*
 * public
 */


void DisplaySdl::drawChar(const std::pair<int,int> &offset, const char c) const {
//	SDL_Rect screenRect, srcRect;
//	screenRect.x = offset.first;
//	screenRect.y = offset.second;
}

void DisplaySdl::drawBorder(const std::pair<int,int> &offset, const std::pair<int,int> &dimension) const {
	SDL_Rect rect = { (Sint16) offset.first, (Sint16) offset.second, (Uint16) dimension.first, (Uint16) dimension.second };
//	if (SDL_FillRect(screen, &rect, 0x00777700) == -1) {
		// XXX log
//		std::cout << "fill rect error " << SDL_GetError() << std::endl;
//	}
//	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
}

void DisplaySdl::drawText(const std::pair<int,int> &offset, const std::basic_string<char> &text) const {
	SDL_Rect screenRect, fontPanelRect;
	screenRect.x = offset.first;
	screenRect.y = offset.second;
	screenRect.h = fontPanel->h;
	for (const auto c : text) {
		if (fontPanelChar(c, &fontPanelRect)) {
			screenRect.w = fontPanelRect.w;
			screenRect.h = fontPanelRect.h;
			if (SDL_BlitSurface(fontPanel, &fontPanelRect, screen, &screenRect) == -1) {
				// XXX log
				std::cout << "blit surface error " << SDL_GetError() << std::endl;
			}
		} else {
			int error = FT_Load_Char(face, c, FT_LOAD_RENDER);
			if (error) {
				// XXX log
				std::cout << "load char error " << error << std::endl;
			}
			const FT_Bitmap bitmap = face->glyph->bitmap;
			screenRect.w = 8;
			screenRect.h = 16;
			drawGlyph(screen, face->glyph, screenRect.x, screenRect.y);
		}
		screenRect.x += screenRect.w;
	}
	SDL_UpdateRect(screen, offset.first, offset.second, screenRect.x, screenRect.h);
}

std::pair<int,int> DisplaySdl::screenDimension() const {
	return { screen->w, screen->h };
}

std::pair<int,int> DisplaySdl::fontDimension() const {
	return { 8, 14 };
//	return { fontPanelOffsets[fontPanelCharCount-1] / fontPanelCharCount, face->size->metrics.height };
}

