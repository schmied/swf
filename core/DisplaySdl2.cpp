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

#include <bitset>
#include <cmath>
#include <iostream>
#include <utility>

#ifdef __FreeBSD__
#include <freetype2/ft2build.h>
#include <freetype2/freetype.h>
#endif
#ifdef WIN32
#include <ft2build.h>
#include <freetype/freetype.h>
#endif
#include <SDL2/SDL.h>

#include "DisplaySdl2.hpp"

#include "Component.hpp"
#include "Context.hpp"


// used in drawLine()
#define DRAW_LINE_YSTEP err += dy; if (err << 1 >= dx) { y += ystep; err -= dx; }

static const std::basic_string<char> LOG_FACILITY = "DISPLAY_SDL";


/*
 * ******************************************************** constructor
 */

DisplaySdl2::DisplaySdl2(Context &ctx, SDL_Window *win, SDL_Renderer *rnd) : Display(ctx) {

	window = win;
	renderer = rnd;

	int error = FT_Init_FreeType(&fontLibrary);
	if (error) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "freetype init error: %d", error);
		return;
	}

	error = FT_New_Face(fontLibrary, "term14.pcf.gz", 0, &fontFace);
	if (error) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "freetype new face error: %d", error);
		return;
	}

	int screenHeight;
	SDL_GetWindowSize(window, NULL, &screenHeight);
	int targetFontSize = screenHeight / 100;
	if (targetFontSize < 8)
		targetFontSize = 8;
	fontHeight = 0;
	fontSize = 0;

	const FT_Bitmap_Size *sizes = fontFace->available_sizes;
	const FT_Int sizesCount = fontFace->num_fixed_sizes;
	if (sizes != NULL && sizesCount > 0) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "look for bitmap sizes");
		int fontWidth = 0;
		for (int i = 0; i < sizesCount; i++) {
			fontWidth = sizes[i].width;
			fontHeight = sizes[i].height;
			fontSize = std::lround(sizes[i].size / 64.0);
			if (fontHeight >= targetFontSize)
				break;
		}
/*
		error = FT_Set_Pixel_Sizes(fontFace, fontWidth, fontHeight);
		if (error) {
			getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "freetype set pixel sizes error: %d", error);
			return;
		}
*/
	}
	if (fontHeight == 0 || fontSize == 0) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "cannot determine font size");
		return;
	}
	getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "font: %s, height: %d, size: %d", fontFace->family_name, fontHeight, fontSize);

	// determine font panel offsets
	int lastXOffset = 0;
	for (int i = 0; i < fontPanelCharCount; i++) {
		const char c = fontPanelFirstChar + (char) i;
		error = FT_Load_Char(fontFace, c, FT_LOAD_RENDER);
		if (error) {
			getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "freetype load char error: %d", error);
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
			getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "freetype unknown glyph format");
			return;
		}
		if (i == fontPanelCharCount - 1)
			fontPanelOffsets[fontPanelCharCount-1] = lastXOffset;
	}
	const int fontPanelWidth = fontPanelOffsets[fontPanelCharCount-1];
	fontWidthAvg = fontPanelWidth / fontPanelCharCount;

	// create font panel surface
	SDL_Surface *fontPanelSrf = SDL_CreateRGBSurface(0, fontPanelWidth, fontHeight, 32, 0, 0, 0, 0);
	if (fontPanel == NULL) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "sdl2 create rgb surface error: %s", SDL_GetError());
		return;
	}
	if (SDL_SetColorKey(fontPanelSrf, SDL_TRUE, 0x00000000) == -1)
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "sdl2 set color key error: %s", SDL_GetError());

	// populate font panel
	for (int i = 0; i < fontPanelCharCount; i++) {
		const char c = fontPanelFirstChar + i;
		error = FT_Load_Char(fontFace, c, FT_LOAD_RENDER);
		if (error) {
			getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<init>", "freetype load char error: %d", error);
			continue;
		}
		int x = 0;
		if (i > 0)
			x = fontPanelOffsets[i-1];
		drawGlyph(fontPanelSrf, fontFace->glyph, x, 0, SDL_MapRGB(fontPanelSrf->format, 0xff, 0xff, 0xff));
//		drawGlyph(window, fontFace->glyph, x, 200, SDL_MapRGB(fontPanelSrf->format, 255, 255, 0));
	}

	// load font panel surface into texture
	fontPanel = SDL_CreateTextureFromSurface(renderer, fontPanelSrf);
	SDL_FreeSurface(fontPanelSrf);
}

DisplaySdl2::~DisplaySdl2() {
	SDL_DestroyTexture(fontPanel);
	int error = FT_Done_Face(fontFace);
	if (error) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<free>", "freetype done face error: %d", error);
	}
	error = FT_Done_FreeType(fontLibrary);
	if (error) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<free>", "freetype done freetype error: %d", error);
	}
	getContext()->log(Context::LOG_WARN, LOG_FACILITY, "<free>", nullptr);
}


/*
 * ******************************************************** private
 */


/*
 * font panel
 */

bool DisplaySdl2::isFontPanelChar(const int c, SDL_Rect *dimension) const {
	if (c < fontPanelFirstChar || c > fontPanelLastChar)
		return false;
	const int idx = c - fontPanelFirstChar;
	if (idx == 0)
		dimension->x = 0;
	else
		dimension->x = fontPanelOffsets[idx-1];
	dimension->y = 0;
	dimension->w = fontPanelOffsets[idx] - dimension->x;
	dimension->h = fontHeight;
	return true;
}


/*
 * drawing
 */

inline void DisplaySdl2::drawPoint(SDL_Surface *dst, const int x, const int y, const Uint32 color) {
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

void DisplaySdl2::drawLine(SDL_Surface *dst, int x0, int y0, int x1, int y1, const Uint32 color) {
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

void DisplaySdl2::drawGlyph(SDL_Surface *dst, const FT_GlyphSlot glyph, const int offsetX, const int offsetY,
	    const Uint32 color) {
	const FT_Bitmap bitmap = glyph->bitmap;
	const int width = bitmap.width;
	const int height = bitmap.rows;

	const SDL_Rect r = { (Sint16) offsetX, (Sint16) offsetY, (Uint16) width, (Uint16) height };
	if (SDL_FillRect(dst, &r, 0x00000000) == -1)
		std::printf("%s drawText() sdl2 fill rect error: %s", LOG_FACILITY.c_str(), SDL_GetError());

	const int baseX = offsetX + std::lround(glyph->metrics.horiBearingX / 64.0);
	const int baseY = offsetY + glyph->bitmap_top - std::lround(glyph->metrics.horiBearingY / 64.0) - 2;
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


/*
 * event handling
 */

void* DisplaySdl2::eventPoll() {
	const int i = SDL_PollEvent(&currentEvent);
	if (!i)
		return nullptr;
	return &currentEvent;
}

void* DisplaySdl2::eventWait() {
	const int i = SDL_WaitEvent(&currentEvent);
	if (!i) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "eventWait", "sdl wait event error");
		return nullptr;
	}
	return &currentEvent;
}

void DisplaySdl2::gameEventSleep() const {
	SDL_Delay(1);	
}

long DisplaySdl2::gameEventTicks() const {
	return SDL_GetTicks();
}


/*
 * ******************************************************** public
 */


/*
 * getter
 */

SDL_Window* DisplaySdl2::getWindow() const {
	return window;
}

SDL_Renderer* DisplaySdl2::getRenderer() const {
	return renderer;
}


/*
 * drawing
 */

/*
void DisplaySdl2::drawBorder(const std::pair<int,int> &offset, const std::pair<int,int> &dimension) const {
//	getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "drawBorder", "%d+%d %dx%d", offset.first, offset.second, dimension.first, dimension.second);
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
//	SDL_UpdateRect(screen, x0, y0, dimension.first, 1);
//	SDL_UpdateRect(screen, x0, y1, dimension.first, 1);
//	SDL_UpdateRect(screen, x0, y0, 1, dimension.second);
//	SDL_UpdateRect(screen, x1, y0, 1, dimension.second);

	// rect method
	SDL_Rect rectOuter { (Sint16) offset.first, (Sint16) offset.second, (Uint16) dimension.first, (Uint16) dimension.second };
	SDL_FillRect(screen, &rectOuter, color); 
	SDL_Rect rectInner { (Sint16) (offset.first + 1), (Sint16) (offset.second + 1), (Uint16) (dimension.first - 2),
	    (Uint16) (dimension.second - 2) };
	SDL_FillRect(screen, &rectInner, 0x00000000); 
	SDL_UpdateRect(screen, offset.first, offset.second, dimension.first, dimension.second);
}
*/

//void DisplaySdl2::drawText(const std::pair<int,int> &offset, const std::pair<int,int> &dimension,
//	    const std::basic_string<char> &text) const {
void DisplaySdl2::draw(const Position &pos, const Style& stl, const std::basic_string<char> &text) const {
//	getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "drawText", "%d+%d '%s'", offset.first, offset.second, text.c_str());

	SDL_Rect screenRect, fontPanelRect;

	screenRect.x = pos.x;
	screenRect.y = pos.y;
	screenRect.w = pos.w;
	screenRect.h = pos.h;

	// fill background
	if (SDL_RenderFillRect(renderer, &screenRect) != 0) {
		getContext()->log(Context::LOG_WARN, LOG_FACILITY, "draw", "sdl2 render fill rect error: %s", SDL_GetError());
		return;
	}

	for (const auto c : text) {
		const bool isPanelChar = isFontPanelChar(c, &fontPanelRect);
		if (isPanelChar) {
			screenRect.w = fontPanelRect.w;
		} else {
			const int error = FT_Load_Char(fontFace, c, FT_LOAD_RENDER);
			if (error) {
				getContext()->log(Context::LOG_WARN, LOG_FACILITY, "draw", "freetype load char error: %d", error);
				continue;
			}
			screenRect.w = fontFace->glyph->bitmap.width;
		}

/*
		if (screenRect.x + screenRect.w >= screen->w) {
			getContext()->log(Context::LOG_WARN, LOG_FACILITY, "drawText", "%d + %d >= %d", screenRect.x,
			    screenRect.w, screen->w);
			break;
		}
*/
		if (isPanelChar) {
			if (SDL_RenderCopy(renderer, fontPanel, &fontPanelRect, &screenRect) != 0)
//				getContext()->log(Context::LOG_WARN, LOG_FACILITY, "draw", "sdl2 render copy: %s",
//				    SDL_GetError());
		;
		} else {
//			drawGlyph(screen, fontFace->glyph, screenRect.x, screenRect.y,
//			    SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));
		}
		screenRect.x += screenRect.w;
	}

	// debug
/*
	SDL_LockSurface(screen);
	drawPoint(screen, offset.first, offset.second, 0x80808080);
	drawPoint(screen, offset.first + dimension.first - 1, offset.second, 0x80808080);
	drawPoint(screen, offset.first, offset.second + dimension.second - 1, 0x80808080);
	drawPoint(screen, offset.first + dimension.first - 1, offset.second + dimension.second - 1, 0x80808080);
	SDL_UnlockSurface(screen);
*/

/*
	// fill rect to the end of dimension width
	if (screenRect.x < offset.first + dimension.first) {
		screenRect.w = offset.first + dimension.first - screenRect.x;
		if (screenRect.w > 0 && SDL_FillRect(screen, &screenRect, 0x000ff000) == -1)
			getContext()->log(Context::LOG_WARN, LOG_FACILITY, "drawText", "sdl fill rect error: %s", SDL_GetError());
	}
*/

//	SDL_UpdateRect(screen, offset.first, offset.second, dimension.first, dimension.second);
}

std::pair<int,int> DisplaySdl2::screenDimension() const {
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	return { w, h };
}

std::pair<int,int> DisplaySdl2::fontDimension() const {
	return { fontWidthAvg, fontHeight };
}


/*
 * event handling
 */

void DisplaySdl2::handleEvent(void *event) const {
	if (event == nullptr)
		return;
	const SDL_Event *e = (const SDL_Event*) event;
	switch (e->type) {
	case SDL_KEYDOWN:
//		getContext()->log(Context::LOG_DEBUG, LOG_FACILITY, "handleEvent", "typ %d sym %d", e->type, e->key.keysym.sym);
		switch (e->key.keysym.sym) {
		case SDLK_RETURN:
			break;
		case SDLK_UP:
			break;
		case SDLK_DOWN:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}


/*
 * sdl helper
 */

SDL_Window* DisplaySdl2::initWindow() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::printf("%s initScreen() sdl2 init error: %s\n", LOG_FACILITY.c_str(), SDL_GetError());
		return nullptr;
	}
	SDL_Window *scr = SDL_CreateWindow("title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (scr == NULL) {
		std::printf("%s initScreen() sdl2 create window error: %s\n", LOG_FACILITY.c_str(), SDL_GetError());
		return nullptr;
	}
	return scr;
}

SDL_Renderer* DisplaySdl2::initRenderer(SDL_Window *win) {
	SDL_Renderer *rnd = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (rnd == NULL) {
		std::printf("%s initRenderer() sdl2 create renderer error: %s\n", LOG_FACILITY.c_str(), SDL_GetError());
		return nullptr;
	}
	return rnd;
}

