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
#include "Context.hpp"


// used in drawLine()
#define DRAW_LINE_YSTEP err += dy; if (err << 1 >= dx) { y += ystep; err -= dx; }

static const std::basic_string<char> LOG_FACILITY = "DISPLAY_SDL";


/*
 * constructor
 */

DisplaySdl::DisplaySdl(Context *c) : Display(c) {

	int error = FT_Init_FreeType(&fontLibrary);
	if (error) {
		getContext()->logWarn(LOG_FACILITY, "<init>", "freetype init error: %d", error);
		return;
	}

//	error = FT_New_Face(fontLibrary, "/usr/local/lib/X11/fonts/75dpi/courR14.pcf.gz", 0, &fontFace);
	error = FT_New_Face(fontLibrary, "/usr/local/lib/X11/fonts/75dpi/term14.pcf.gz", 0, &fontFace);
//	error = FT_New_Face(fontLibrary, "/usr/local/lib/X11/fonts/75dpi/courR12.pcf.gz", 0, &fontFace);
//	error = FT_New_Face(fontLibrary, "/usr/local/lib/X11/fonts/75dpi/helvR18.pcf.gz", 0, &fontFace);
	if (error) {
		getContext()->logWarn(LOG_FACILITY, "<init>", "freetype new face error: %d", error);
		return;
	}

	const FT_Int sizesCount = fontFace->num_fixed_sizes;
	if (sizesCount > 0) {
		const FT_Bitmap_Size *sizes = fontFace->available_sizes;
		if (sizes == NULL) {
			getContext()->logWarn(LOG_FACILITY, "<init>", "freetype no bitmap sizes");
			return;
		}
		fontHeight = sizes[0].height;
		fontSize = std::lround(sizes[0].size / 64.0);
	} else {
		getContext()->logWarn(LOG_FACILITY, "<init>", "bitmapless fonts not supported (yet)");
		return;
	}

	// determine font panel offsets
	int lastXOffset = 0;
	for (int i = 0; i < fontPanelCharCount; i++) {
		const char c = fontPanelFirstChar + (char) i;
		error = FT_Load_Char(fontFace, c, FT_LOAD_RENDER);
		if (error) {
			getContext()->logWarn(LOG_FACILITY, "<init>", "freetype load char error: %d", error);
			continue;
		}
		const FT_GlyphSlot glyph = fontFace->glyph;
		if (i > 0) 
			fontPanelOffsets[i-1] = lastXOffset;
		switch (glyph->format) {
		case FT_GLYPH_FORMAT_BITMAP:
			lastXOffset += std::lround(glyph->metrics.horiAdvance / 64.00);
			break;
		default:
			getContext()->logWarn(LOG_FACILITY, "<init>", "freetype unknown glyph format");
			return;
		}
		if (i == fontPanelCharCount - 1)
			fontPanelOffsets[fontPanelCharCount-1] = lastXOffset;
	}
	const int fontPanelWidth = fontPanelOffsets[fontPanelCharCount-1];
	fontWidthAvg = fontPanelWidth / fontPanelCharCount;

	// create screen
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		getContext()->logWarn(LOG_FACILITY, "<init>", "sdl init error: %s", SDL_GetError());
		return;
	}
	screen = SDL_SetVideoMode(0, 0, 0, SDL_ANYFORMAT);
//	screen = SDL_SetVideoMode(1024, 768, 0, SDL_ANYFORMAT);
	if (screen == NULL) {
		getContext()->logWarn(LOG_FACILITY, "<init>", "sdl set video mode error: %s", SDL_GetError());
		return;
	}
	SDL_FillRect(screen, NULL, 0x00000000);

	// create font panel
	const SDL_PixelFormat *fmt = screen->format;
	fontPanel = SDL_CreateRGBSurface(screen->flags, fontPanelWidth, fontHeight, fmt->BitsPerPixel,
	    fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	if (fontPanel == NULL) {
		getContext()->logWarn(LOG_FACILITY, "<init>", "sdl create rgb surface error: %s", SDL_GetError());
		return;
	}

	// populate font panel
	for (int i = 0; i < fontPanelCharCount; i++) {
		const char c = fontPanelFirstChar + i;
		error = FT_Load_Char(fontFace, c, FT_LOAD_RENDER);
		if (error) {
			getContext()->logWarn(LOG_FACILITY, "<init>", "freetype load char error: %d", error);
			continue;
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
		getContext()->logWarn(LOG_FACILITY, "<free>", "freetype done face error: %d", error);
	}
	error = FT_Done_FreeType(fontLibrary);
	if (error) {
		getContext()->logWarn(LOG_FACILITY, "<free>", "freetype done freetype error: %d", error);
	}
	getContext()->logInfo(LOG_FACILITY, "<free>");
}


/*
 * private
 */

inline void DisplaySdl::drawPoint(SDL_Surface *dst, const int x, const int y, const Uint32 color) {
	int bpp = dst->format->BytesPerPixel;
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
	int y = y0, err = 0;
	void *pos;
	SDL_LockSurface(dst);
	if (steep) {
		switch (bpp) {
		case 1:
			for (int x = x0; x <= x1; x++) {
				pos = (Uint8*) dst->pixels + (dst->pitch * x + y * bpp);
				*(Uint8*) pos = color;
				DRAW_LINE_YSTEP
			}
			break;
		case 2:
			for (int x = x0; x <= x1; x++) {
				pos = (Uint8*) dst->pixels + (dst->pitch * x + y * bpp);
				*(Uint16*) pos = color;
				DRAW_LINE_YSTEP
			}
			break;
		case 4:
			for (int x = x0; x <= x1; x++) {
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
			for (int x = x0; x <= x1; x++) {
				pos = (Uint8*) dst->pixels + (dst->pitch * y + x * bpp);
				*(Uint8*) pos = color;
				DRAW_LINE_YSTEP
			}
			break;
		case 2:
			for (int x = x0; x <= x1; x++) {
				pos = (Uint8*) dst->pixels + (dst->pitch * y + x * bpp);
				*(Uint16*) pos = color;
				DRAW_LINE_YSTEP
			}
			break;
		case 4:
			for (int x = x0; x <= x1; x++) {
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

void DisplaySdl::drawGlyph(SDL_Surface *dst, const FT_GlyphSlot glyph, const int offsetX, const int offsetY,
	    const Uint32 color) const {
	const FT_Bitmap bitmap = glyph->bitmap;
	const int width = bitmap.width;
	const int height = bitmap.rows;
	const int baseX = offsetX + std::lround(glyph->metrics.horiBearingX / 64.0);
	const int baseY = offsetY + fontSize - std::lround(glyph->metrics.horiBearingY / 64.0) - 2;
	unsigned char *buffer = bitmap.buffer;
	SDL_LockSurface(dst);
	for (int y = 0; y < height; y++) {
		int bufferIndex = y * bitmap.pitch;
		Uint16 x = 0;
		while (x < width) {
			const unsigned char c = buffer[bufferIndex];
			const std::bitset<8> bits = { (const unsigned long long) c };
			for (size_t i = 0; i < bits.size() && x < width; i++) {
				if (bits[7-i])
					drawPoint(dst, baseX + x, baseY + y, color);
				x++;
			}
			bufferIndex++;
		}
	}
//	drawPoint(dst, offsetX, offsetY, 0x00ff0000);
//	drawPoint(dst, offsetX, offsetY + fontHeight - 1, 0x00ff0000);
	SDL_UnlockSurface(dst);
}

// look up char in font panel cache
bool DisplaySdl::fontPanelChar(const int c, SDL_Rect *dimension) const {
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

int DisplaySdl::handleEvent(const SDL_Event *event) const {
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
			return 0;
			break;
		}
		break;

	default:
		return 0;
		break;
	}
	return 1;
}

void DisplaySdl::drawBorder(const std::pair<int,int> &offset, const std::pair<int,int> &dimension) const {
	std::printf("## drawBorder %d+%d %dx%d\n", offset.first, offset.second, dimension.first, dimension.second);
	getContext()->logDebug(LOG_FACILITY, "drawBorder", "%d+%d %dx%d", offset.first, offset.second, dimension.first, dimension.second);
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
//	std::printf("## drawText %d+%d %s\n", offset.first, offset.second, text.c_str());
//	getContext()->logDebug(LOG_FACILITY, "drawText", "%d+%d '%s'", offset.first, offset.second, text.c_str());
	SDL_Rect screenRect, fontPanelRect;
	screenRect.x = offset.first;
	screenRect.y = offset.second;
	screenRect.h = fontFace->height;
	for (const auto c : text) {
		if (fontPanelChar(c, &fontPanelRect)) {
			screenRect.w = fontPanelRect.w;
			screenRect.h = fontPanelRect.h;
			if (SDL_BlitSurface(fontPanel, &fontPanelRect, screen, &screenRect) == -1)
				getContext()->logWarn(LOG_FACILITY, "drawText", "sdl blit surface error: %s", SDL_GetError());
		} else {
			int error = FT_Load_Char(fontFace, c, FT_LOAD_RENDER);
			if (error) {
				getContext()->logWarn(LOG_FACILITY, "drawText", "freetype load char error: %d", error);
				continue;
			}
			screenRect.w = fontFace->glyph->bitmap.width;
			screenRect.h = fontFace->height;
			drawGlyph(screen, fontFace->glyph, screenRect.x, screenRect.y,
			    SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));
		}
		screenRect.x += screenRect.w;
	}
	SDL_UpdateRect(screen, offset.first, offset.second, screenRect.x, screenRect.h);
}

std::pair<int,int> DisplaySdl::screenDimension() const {
	return { screen->w, screen->h };
}

std::pair<int,int> DisplaySdl::fontDimension() const {
	return { fontWidthAvg, fontHeight };
}

