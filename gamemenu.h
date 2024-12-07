#ifndef GAMEMENU_H
#define GAMEMENU_H

// Game menu entries
enum GameMenu
{
	GMENU_NONE,

	GMENU_MAP,
	GMENU_TEAM,

	GMENU_DIG,
	GMENU_BUILD,
	GMENU_ASSIGN,
	GMENU_GUARD,

	GMENU_HISTORY,
	GMENU_LAUNCH,
	
	GMENU_SAVE,
	GMENU_OPTIONS
};

// Current menu entry
extern GameMenu	gmenu;

// Menu position
extern char gmenux;

// Room cursor 
extern char cursorx, cursory;

// Selected digger
extern char diggeri;
extern bool diggerchanged;

// Selected building
extern char buildingi;
extern bool buildingchanged;

// Current status view area
enum StatusView
{
	STVIEW_MINIMAP,
	STVIEW_TEAM,
	STVIEW_BUILD
};

extern StatusView	statusview;

// Init menu system
void gmenu_init(void);

// Select a menu entry
void gmenu_set(char x);

// Process a menu key
void gmenu_key(char keyb);

// Navigate in menu
void gmenu_nav(signed char dx);

// Push menu button
void gmenu_push(void);

// Directional navigate in menu
void gmenu_joy(signed char dx, signed char dy);

#pragma compile("gamemenu.c")

#endif

