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

#include <cmath>
#include <iostream>
#include <utility>

#include <freetype2/ft2build.h>
#include <freetype2/freetype.h>
#include <SDL/SDL.h>

#include "DisplaySdl.hpp"

#include "Component.hpp"
#include "RootContainer.hpp"

// used in drawLine()
#define DRAW_LINE_YSTEP err += dy; if (err << 1 >= dx) { y += ystep; err -= dx; }


/*
 * constructor
 */

DisplaySdl::DisplaySdl() {

	int error = FT_Init_FreeType(&fontLibrary);
	if (error) {
		// XXX log
		std::cout << "init freetype error " << error << std::endl;
		return;
	}
	error = FT_New_Face(fontLibrary, "/usr/local/lib/X11/fonts/75dpi/term14.pcf.gz", 0, &fontFace);
//	error = FT_New_Face(fontLibrary, "/usr/local/lib/X11/fonts/75dpi/courR12.pcf.gz", 0, &fontFace);
//	error = FT_New_Face(fontLibrary, "/usr/local/lib/X11/fonts/75dpi/helvR18.pcf.gz", 0, &fontFace);
	if (error) {
		// XXX log
		std::cout << "new face error " << error << std::endl;
		return;
	}

	const FT_Int sizesCount = fontFace->num_fixed_sizes;
	if (sizesCount > 0) {
		const FT_Bitmap_Size *sizes = fontFace->available_sizes;
		if (sizes == NULL) {
			// XXX log
			std::cout << "no bitmap sizes" << std::endl;
			return;
		}
		for (int i = 0; i < sizesCount; i++) {
			std::cout << i << ": size " << std::round(sizes[i].size / 64.0) << ", height " << sizes[i].height << std::endl;
		}
		fontHeight = sizes[0].height;
		fontSize = std::round(sizes[0].size / 64.0);
	} else {
		// XXX log
		std::cout << "bitmapless fonts not supported" << std::endl;
		return;
	}

	// determine font panel offsets
	int lastXOffset = 0;
	for (int i = 0; i < fontPanelCharCount; i++) {
		const char c = fontPanelFirstChar + (char) i;
		error = FT_Load_Char(fontFace, c, FT_LOAD_RENDER);
		if (error) {
			// XXX log
			std::cout << "load char error " << error << std::endl;
		}
		const FT_GlyphSlot glyph = fontFace->glyph;
		if (i > 0) 
			fontPanelOffsets[i-1] = lastXOffset;
		switch (glyph->format) {
		case FT_GLYPH_FORMAT_BITMAP:
			lastXOffset += glyph->bitmap.width;
			break;
		default:
			std::cout << "unknown glyph format" << std::endl;
			return;
		}
		if (i == fontPanelCharCount - 1)
			fontPanelOffsets[fontPanelCharCount-1] = lastXOffset;
	}
	const int fontPanelWidth = fontPanelOffsets[fontPanelCharCount-1];
	fontWidthAvg = fontPanelWidth / fontPanelCharCount;

	// create screen
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		// XXX log SDL_GetError()
		return;
	}
//	screen = SDL_SetVideoMode(0, 0, 0, SDL_ANYFORMAT);
	screen = SDL_SetVideoMode(1024, 768, 0, SDL_ANYFORMAT);
	if (screen == NULL) {
		// XXX log SDL_GetError()
		std::cout << "set video mode error " << SDL_GetError() << std::endl;
		return;
	}
	SDL_FillRect(screen, NULL, 0x00000000);

	// create font panel
	const SDL_PixelFormat *fmt = screen->format;
	fontPanel = SDL_CreateRGBSurface(screen->flags, fontPanelWidth, fontHeight, fmt->BitsPerPixel,
	    fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	if (fontPanel == NULL) {
		// XXX log SDL_GetError()
		std::cout << "create font surface error" << SDL_GetError() << std::endl;
		return;
	}
	SDL_FillRect(fontPanel, NULL, 0x00666600);

	// populate font panel
	for (int i = 0; i < fontPanelCharCount; i++) {
		const char c = fontPanelFirstChar + i;
		error = FT_Load_Char(fontFace, c, FT_LOAD_RENDER);
		if (error) {
			// XXX log
			std::cout << "load char error " << error << std::endl;
		}
		int x = 0;
		if (i > 0)
			x = fontPanelOffsets[i-1];
		drawGlyph(fontPanel, fontFace->glyph, x, 0, SDL_MapRGB(fontPanel->format, 0xff, 0xff, 0xff));
		drawGlyph(screen, fontFace->glyph, x, 200, SDL_MapRGB(fontPanel->format, 255, 255, 0));
	}
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

DisplaySdl::~DisplaySdl() {
	SDL_FreeSurface(fontPanel);
	SDL_Quit();
	int error = FT_Done_Face(fontFace);
	if (error) {
		// XXX log
		std::cout << "done face " << error << std::endl;
	}
	error = FT_Done_FreeType(fontLibrary);
	if (error) {
		// XXX log
		std::cout << "done free type " << error << std::endl;
	}
	std::cout << "displaysdl terminated." << std::endl;
}


/*
 * private
 */

void DisplaySdl::drawPoint(SDL_Surface *dst, const int x, const int y, const Uint32 color) {
	const int bpp = dst->format->BytesPerPixel;
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

void DisplaySdl::drawLine(SDL_Surface *dst, int x0, int y0, int x1, int y1, const Uint32 color) {
	const bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
	int tmp;
	if (steep) {
		tmp = x0;
		x0 = y0;
		y0 = tmp;
		tmp = x1;
		x1 = y1;
		y1 = tmp;
	}
	if (x0 > x1) {
		tmp = x0;
		x0 = x1;
		x1 = tmp;
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}
	const int ystep = y0 < y1 ? 1 : -1;
	const int dx = x1 - x0;
	const int dy = abs(y1 - y0);
	const int bpp = dst->format->BytesPerPixel;
	int x, y = y0, err = 0;
	void *pos;
	SDL_LockSurface(dst);
	if (steep) {
		switch (bpp) {
		case 1:
			for (x = x0; x <= x1; x++) {
				pos = (Uint8*) dst->pixels + (dst->pitch * x + y * bpp);
				*(Uint8*) pos = color;
				DRAW_LINE_YSTEP
			}
			break;
		case 2:
			for (x = x0; x <= x1; x++) {
				pos = (Uint8*) dst->pixels + (dst->pitch * x + y * bpp);
				*(Uint16*) pos = color;
				DRAW_LINE_YSTEP
			}
			break;
		case 4:
			for (x = x0; x <= x1; x++) {
				pos = (Uint8*) dst->pixels + (dst->pitch * x + y * bpp);
				*(Uint32*) pos = color;
				DRAW_LINE_YSTEP
			}
			break;
		default:
			// XXX log
			break;
		}
	} else {
		switch (bpp) {
		case 1:
			for (x = x0; x <= x1; x++) {
				pos = (Uint8*) dst->pixels + (dst->pitch * y + x * bpp);
				*(Uint8*) pos = color;
				DRAW_LINE_YSTEP
			}
			break;
		case 2:
			for (x = x0; x <= x1; x++) {
				pos = (Uint8*) dst->pixels + (dst->pitch * y + x * bpp);
				*(Uint16*) pos = color;
				DRAW_LINE_YSTEP
			}
			break;
		case 4:
			for (x = x0; x <= x1; x++) {
				pos = (Uint8*) dst->pixels + (dst->pitch * y + x * bpp);
				*(Uint32*) pos = color;
				DRAW_LINE_YSTEP
			}
			break;
		default:
			// XXX log
			break;
		}
	}
	SDL_UnlockSurface(dst);

}

void DisplaySdl::drawGlyph(SDL_Surface *dst, const FT_GlyphSlot glyph, const Uint16 offsetX, const Uint16 offsetY,
	    const Uint32 color) const {
	const FT_Bitmap bitmap = glyph->bitmap;
	const Uint16 width = bitmap.width;
	const Uint16 baseY = offsetY + fontSize - std::round(glyph->metrics.horiBearingY / 64.0) - 2;
	unsigned char *buffer = bitmap.buffer;
	SDL_LockSurface(dst);
	for (Uint16 y = 0; y < bitmap.rows; y++) {
		int bufferIndex = y * bitmap.pitch;
		Uint16 x = 0;
		while (x < width) {
			const unsigned char c = buffer[bufferIndex];
			const std::bitset<8> bits = { (const unsigned long long) c };
			for (size_t i = 0; i < bits.size() && x < width; i++) {
				if (bits[7-i])
					drawPoint(dst, offsetX + x, baseY + y, color);
				x++;
			}
			bufferIndex++;
		}
	}
	drawPoint(dst, offsetX, offsetY, 0x00ff0000);
	drawPoint(dst, offsetX, offsetY + fontHeight - 1, 0x00ff0000);
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
	dimension->h = fontPanel->h;
	return true;
}


/*
 * public
 */

const SDL_Event* DisplaySdl::handleEvent(const SDL_Event *event) const {
	switch (event->type) {
	case SDL_KEYDOWN:
		switch (event->key.keysym.sym) {
		case SDLK_ESCAPE:
			break;
		case SDLK_RETURN:
			break;
		case SDLK_UP:
			break;
		case SDLK_DOWN:
			break;
		default:
			return NULL;
			break;
		}
		break;

	default:
		return NULL;
		break;
	}
	return event;

}

void DisplaySdl::drawBorder(const std::pair<int,int> &offset, const std::pair<int,int> &dimension) const {

	const Uint32 color = SDL_MapRGB(screen->format, 0xff, 0x00, 0x00);

	// line method
	const Uint16 x0 = offset.first;
	const Uint16 y0 = offset.second;
	const Uint16 x1 = offset.first + dimension.first - 1;
	const Uint16 y1 = offset.second + dimension.second - 1;
	drawLine(screen, x0, y0, x1, y0, color);
	drawLine(screen, x0, y1, x1, y1, color);
	drawLine(screen, x0, y0, x0, y1, color);
	drawLine(screen, x1, y0, x1, y1, color);
	SDL_UpdateRect(screen, x0, y0, dimension.first, 1);
	SDL_UpdateRect(screen, x0, y1, dimension.first, 1);
	SDL_UpdateRect(screen, x0, y0, 1, dimension.second);
	SDL_UpdateRect(screen, x1, y0, 1, dimension.second);

/*
	// rect method
	SDL_Rect rectOuter { (Sint16) offset.first, (Sint16) offset.second, (Uint16) dimension.first, (Uint16) dimension.second };
	SDL_FillRect(screen, &rectOuter, color); 
	SDL_Rect rectInner { (Sint16) (offset.first + 1), (Sint16) (offset.second + 1), (Uint16) (dimension.first - 2),
	    (Uint16) (dimension.second - 2) };
	SDL_FillRect(screen, &rectInner, 0x00000000); 
	SDL_UpdateRect(screen, offset.first, offset.second, dimension.first, dimension.second);
*/
}

void DisplaySdl::drawText(const std::pair<int,int> &offset, const std::basic_string<char> &text) const {
	SDL_Rect screenRect, fontPanelRect;
	screenRect.x = offset.first;
	screenRect.y = offset.second;
	screenRect.h = fontFace->height;
	for (const auto c : text) {
		if (fontPanelChar(c, &fontPanelRect)) {
			screenRect.w = fontPanelRect.w;
			screenRect.h = fontPanelRect.h;
			if (SDL_BlitSurface(fontPanel, &fontPanelRect, screen, &screenRect) == -1) {
				// XXX log
				std::cout << "blit surface error " << SDL_GetError() << std::endl;
			}
		} else {
			int error = FT_Load_Char(fontFace, c, FT_LOAD_RENDER);
			if (error) {
				// XXX log
				std::cout << "load char error " << error << std::endl;
			}
			screenRect.w = fontFace->glyph->bitmap.width;
			screenRect.h = fontFace->height;
			drawGlyph(screen, fontFace->glyph, screenRect.x, screenRect.y,
			    SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));
		}
		screenRect.x += screenRect.w;
	}
	SDL_UpdateRect(screen, offset.first, offset.second, screenRect.x, screenRect.h);

	drawLine(screen, 100, 100, 200, 300, 0x00ffff00);
	SDL_UpdateRect(screen, 100, 100, 100, 200);
}

std::pair<int,int> DisplaySdl::screenDimension() const {
	return { screen->w, screen->h };
}

std::pair<int,int> DisplaySdl::fontDimension() const {
	return { fontWidthAvg, fontHeight };
}

