#ifndef WINDOW_H
#define WINDOW_H

#include "display.h"

extern char story_shown, story_pending;

enum StoryMessages
{
	STM_INTRO,
	STM_RADIO_INVITES,
	STM_ENEMY_THREADS,
	SIM_MOON_DESTROYED,

	NUM_STORY_MESSAGES
};

extern char	winX, winY, winW, winH;

void window_open(char x, char y, char w, char h);

void window_close(void);

void window_fill(char pat);

void window_color_rect(char x, char y, char w, char h, char color);

void window_fill_rect(char x, char y, char w, char h, char pat);

void window_scroll(void);

void window_print(const char * text);

char window_write(char x, char y, const char * text);

char window_write_uint(char x, char y, unsigned u);

void story_init(void);

bool story_messages(void);

#pragma compile("window.c")

#endif