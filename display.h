#ifndef DISPLAY_H
#define DISPLAY_H

#include <c64/vic.h>

static char * const Screen	=	(char *)0xc000;
static char * const Color	=	(char *)0xd800; 
static char * const Sprites =  (char *)0xd000;
static char * const Hires	=	(char *)0xe000;

void disp_fill(char x, char y, char w, char h, char back);

void disp_text(char x, char y, const char * text, char back, char color);

void disp_char(char x, char y, char ch, char back, char color);

void disp_chars(char x, char y, const char * text, char n, char back, char color);

void disp_space(char x, char y, char n, char back, char color);

void disp_bar(char x, char y, char w, char back, char color);

void disp_vbar(char x, char y, char h, char back, char color);

void disp_menu(char x, const char * text, char back, char color, char colork);

void disp_menu_color(char x, const char * text, char back, char color, char colork);

#pragma compile("display.c")

#endif
