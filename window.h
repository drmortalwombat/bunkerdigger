#ifndef WINDOW_H
#define WINDOW_H

#include "display.h"

// Bitmask of story messages show or ready to be shown
extern unsigned long story_shown, story_pending;

// Types of messages to progress the story
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

// Coordinates of message window
extern char	winX, winY, winW, winH;

// Open the message window
void window_open(char x, char y, char w, char h);

// Close the message window
void window_close(void);

// Fill the window with the pattern
void window_fill(char pat);

// Change color buffer for rectangle in the window
void window_color_rect(char x, char y, char w, char h, char color);

// Change color/screen buffer for rectangle in the window
void window_color_rect2(char x, char y, char w, char h, char c0, char c1);

// Will hires buffer with pattern
void window_fill_rect(char x, char y, char w, char h, char pat);

// Scroll window up
void window_scroll(void);

// Write a text to the window, using cursor animation, and control
// codes for line breaks and delays.
void window_print(const char * text);

// Write a zero terminated string to the window
char window_write(char x, char y, const char * text);

// Write a character to the window
void window_char(char x, char y, char ch);

// Write an unsigned number to the window
char window_write_uint(char x, char y, unsigned u);

// Init the story system
void story_init(void);

// Write pending story messages
bool story_messages(void);

#pragma compile("window.c")

#endif
