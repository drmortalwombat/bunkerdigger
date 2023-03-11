#ifndef GAMEMENU_H
#define GAMEMENU_H


enum GameMenu
{
	GMENU_NONE,

	GMENU_MAP,
	GMENU_TEAM,
	GMENU_RSRC,

	GMENU_DIG,
	GMENU_BUILD,
	GMENU_ASSIGN,
	GMENU_GUARD
};

extern GameMenu	gmenu;
extern char gmenux;
extern char cursorx, cursory;
extern char diggeri;
extern bool diggerchanged;

enum StatusView
{
	STVIEW_MINIMAP,
	STVIEW_TEAM
};

extern StatusView	statusview;


void gmenu_init(void);

void gmenu_set(char x);

void gmenu_key(char keyb);

void gmenu_nav(signed char dx);

void gmenu_push(void);

void gmenu_joy(signed char dx, signed char dy);

#pragma compile("gamemenu.c")

#endif

