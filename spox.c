/*
 * Copyright (c) 2001, 2004 Michael Schmiedgen
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted privided that the following conditions
 * are met:
 * 1. Redistribution of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistribution in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 */

/* $Id: spox.c 93 2005-11-14 04:26:15Z  $ */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "global.h"

#ifdef CONIO
#include <conio.h>
#endif
#ifdef CURSES
#include <curses.h>
#endif

#ifdef CONIO
#define CONIOBUFSIZE 4096
short coniobuf[CONIOBUFSIZE];
#endif

struct spox *spox_init(const int, const int, const int, const int);
int spox_uninit(struct spox*);

void spox_draw(struct spox*, int**);
int spox_getheight(struct spox*);
int spox_getwidth(struct spox*);
void spox_setbright(struct spox*, const int);
void spox_setinterval(struct spox*, const int);

char c[] = {	' ','\'', '`', '"',
		' ','\'','\'', '"',
		' ', '`', '`', '"',
		'=', '=', '=', '"',
		'.', ':', ':', '"',
		'.', '[', '/', '/',
		'.', ')', '/', '7',
		'r', '}', '/', 'P',
		'.', ':', ':', '"',
		'.','\\', '(', '"',
		'.','\\', ']','\\',
		'v','\\', '{', 'Y',
		'_', ':', ':', 'X',
		'n', 'L', '6', '[',
		'a', ')', 'J', ']',
		'w', 'b', 'd', '8'
};

struct spox *spox_init(const int width, const int height, const int interval, const int bright) {
	struct spox *s;
#ifdef CURSES
	WINDOW *win;
#endif
#ifdef CONIO
	struct text_info *r;
#endif

	s = (struct spox*) malloc(sizeof (struct spox));
	if (!s)
		return NULL;
#ifdef CURSES
	win = initscr();
	if (!win)
		return NULL;
	nodelay(win, 1);
	getmaxyx(win, s->scr_height, s->scr_width);
#endif
#ifdef CONIO
	r = NULL;
	textmode(BW80);
	_setcursortype(_NOCURSOR);
	gotoxy(3, 3);
	cputs("press <alt> + <enter> for full screen");
	sleep(3);
	gettextinfo(r);
	s->scr_width = (int) r->screenwidth;
	s->scr_height = (int) r->screenheight;
#endif
	s->win_width = s->scr_width;
	s->win_height = s->scr_height;
	s->offset_x = 0;
	s->offset_y = 0;
	if (width < s->scr_width && width > 8) {
		s->win_width = width;
		s->offset_x = (s->win_width - width) / 2;
	}
	if (height < s->scr_height && height > 8) {
		s->win_height = height;
		s->offset_y = (s->win_height - height) / 2;
	}
	s->img_width = s->win_width * PPW;
	s->img_height = s->win_height * PPH;
	s->bright = bright;
	s->interval = interval;
	return s;
}

void spox_draw(struct spox *s, int **image) {
	int m, n, a, h, w, ypos, bright, inter;
#ifdef CONIO
	int inc;

	memset(coniobuf, 0, CONIOBUFSIZE);
	inc = 0;
#endif
	ypos = s->offset_y;
	inter = (1 << s->interval) - 1;
	bright = inter * s->bright / s->interval;
	w = s->img_width;
	h = s->img_height - PPH;
	for (n = 0; n < h; n += PPH) {
#ifdef CURSES
		move(ypos++, s->offset_x);
#endif
		for (m = 0; m < w; m += PPW) {
			if ((image[n  ][m  ] & inter) > bright)
				a = 0x01;
			else
				a = 0x00;
			if ((image[n  ][m+1] & inter) > bright)
				a |= 0x02;
			if ((image[n+1][m  ] & inter) > bright)
				a |= 0x04;
			if ((image[n+1][m+1] & inter) > bright)
				a |= 0x08;
			if ((image[n+2][m  ] & inter) > bright)
				a |= 0x10;
			if ((image[n+2][m+1] & inter) > bright)
				a |= 0x20;
#ifdef CURSES
			addch(c[a]);
#endif
#ifdef CONIO
			coniobuf[inc++] = (7<<8) + c[a];
#endif
		}
	}
#ifdef CURSES
	move(ypos, s->offset_x);
#endif
	w = s->img_width - 2 * PPW;
	for (m = 0; m < w; m += PPW) {
		if ((image[n  ][m  ] & inter) > bright)
			a = 0x01;
		else
			a = 0x00;
		if ((image[n  ][m+1] & inter) > bright)
			a |= 0x02;
		if ((image[n+1][m  ] & inter) > bright)
			a |= 0x04;
		if ((image[n+1][m+1] & inter) > bright)
			a |= 0x08;
		if ((image[n+2][m  ] & inter) > bright)
			a |= 0x10;
		if ((image[n+2][m+1] & inter) > bright)
			a |= 0x20;
#ifdef CURSES
		addch(c[a]);
#endif
#ifdef CONIO
		coniobuf[inc++] = (7<<8) + c[a];
#endif
	}
#ifdef CURSES
	refresh();
#endif
#ifdef CONIO
	puttext(s->offset_x + 1, s->offset_y + 1, s->scr_width - s->offset_x,
	    s->scr_height - s->offset_y, coniobuf);
#endif
}

int spox_uninit(struct spox *s) {
	int r;

	r = EXIT_SUCCESS;
#ifdef CURSES
	if (erase() != OK)
		r = EXIT_FAILURE;
	if (refresh() == ERR)
		r = EXIT_FAILURE;
	if (endwin() == ERR)
		r = EXIT_FAILURE;
#endif
#ifdef CONIO
	clrscr();
#endif
	free(s);
	return r;
}

int spox_getheight(struct spox *s) {
	return s->img_height;
}

int spox_getwidth(struct spox *s) {
	return s->img_width;
}

void spox_setbright(struct spox *s, int b) {
	s->bright = b;
}

void spox_setinterval(struct spox *s, int i) {
	s->interval = i;
}
