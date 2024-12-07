#ifndef DISPLAY_H
#define DISPLAY_H

#include <c64/vic.h>

static char * const Screen	=	(char *)0xcc00;
static char * const Color	=	(char *)0xd800; 
static char * const Sprites =  (char *)0xd000;
static char * const Hires	=	(char *)0xe000;

extern __striped char * const HiresRow[25];
extern __striped char * const ScreenRow[25];
extern __striped char * const ColorRow[25];

extern const char FontHiresData[];

// Init display system
void disp_init(void);

// Fill a window rectangle
void disp_fill(char x, char y, char w, char h, char back);

// Write zero terminated string
void disp_text(char x, char y, const char * text, char back, char color);

// Write a single character
void disp_char(char x, char y, char ch, char back, char color);

// Write an array of characters
void disp_chars(char x, char y, const char * text, char n, char back, char color);

// Write a message over the main screen
void disp_chars_msg(char x, char y, const char * text, char n, char back, char color);

// Draw spaces
void disp_space(char x, char y, char n, char back, char color);

// Draw a power bar
void disp_bar(char x, char y, char w, char back, char color);

// Draw a resource power bar
void disp_rbar(char x, char y, char w, char total, char area, char color);

// Draw a vertical power bar
void disp_vbar(char x, char y, char h, char back, char color);

// Draw a building progress bar in a room
void disp_bbar(char x, char y, char w);

// Display menu string
void disp_menu(char x, const char * text, char back, char color, char colork);

// Highlight a menu entry
void disp_menu_color(char x, const char * text, char back, char color, char colork);

#pragma compile("display.c")

#endif
