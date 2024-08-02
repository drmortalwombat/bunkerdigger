#ifndef WINDOW_H
#define WINDOW_H

#include "display.h"

extern unsigned long story_shown, story_pending;

enum StoryMessages
{
	STM_INTRO,

	STM_LOW_WATER,
	STM_LOW_ENERGY,
	STM_LOW_AIR,

	STM_DIGGING_ROCK,
	
	STM_RADIO_INVITES,
	STM_ENEMY_MESSAGE,
	STM_ENEMY_THREADS,
	STM_MOON_DESTROYED,

	STM_ROCKET_PLANS,
	STM_ROCKET_BUILD,
	STM_ROCKET_READY,
	STM_ROCKET_LAUNCHED,

	STM_MARS_READY,
	STM_MARS_ESCAPED,

	STM_ENEMY_INCOMING,
	STM_ENEMY_VICTORY,

	STM_DIGGERS_DEAD,
	STM_MOON_IMPACT,

	STM_NEED_RESEARCH,

	NUM_STORY_MESSAGES
};

extern char	winX, winY, winW, winH;

void window_open(char x, char y, char w, char h);

void window_close(void);

void window_fill(char pat);

void window_color_rect(char x, char y, char w, char h, char color);

void window_color_rect2(char x, char y, char w, char h, char c0, char c1);

void window_fill_rect(char x, char y, char w, char h, char pat);

void window_scroll(void);

void window_print(const char * text);

char window_write(char x, char y, const char * text);

void window_char(char x, char y, char ch);

char window_write_uint(char x, char y, unsigned u);

void story_init(void);

bool story_messages(void);

#pragma compile("window.c")

#endif
