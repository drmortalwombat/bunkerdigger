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
	GMENU_ASSIGN
};


extern GameMenu	gmenu;
extern char gmenux;

void gmenu_init(void);

void gmenu_set(char x);

void gmenu_key(char keyb);

void gmenu_move(signed char dx, signed char dy);

void gmenu_joy(signed char dx, signed char dy);

#pragma compile("gamemenu.c")

#endif

