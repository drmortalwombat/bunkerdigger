#ifndef TILES_H
#define TILES_H

#define TF_NONE		0x00
#define TF_LEFT		0x01
#define TF_RIGHT	0x02
#define TF_UP		0x04
#define TF_DOWN		0x08

#define TF_BUNKER	0x10
#define TF_ROOM		0x20

#define TF_LR		TF_LEFT | TF_RIGHT

#define TILE_FLOOR	0x03

extern const char TileFlags[];

extern char BunkerMapData[256];
extern char TileMapFlags[256];

enum TileMapMode
{
	TMMODE_REDRAW,
	TMMODE_DRAWN,
	TMMODE_CURSOR
};

enum RoomTile
{
	RTILE_QUARTERS,
	RTILE_HYDRO,
	RTILE_MINE,
	RTILE_VENTILATION,

	RTILE_GENERATOR,
	RTILE_WORKBENCH,
	RTILE_LABORATORY,
	RTILE_CENTRIFUGE,

	RTILE_GYM,
	RTILE_ARMOURY,
	RTILE_SICKBAY,
	RTILE_STUDY,

	RTILE_EXCAVATOR,
	RTILE_STORAGE,
	
	RTILE_LAUNCH_TOP,
	RTILE_LAUNCH_MID,
	RTILE_LAUNCH_BOTTOM,
	RTILE_MISSILE_TOP,
	RTILE_MISSILE_MID,
	RTILE_MISSILE_BOTTOM,

	RTILE_CONSTRUCTION
};

enum GroundType
{
	GTYPE_SOIL,
	GTYPE_ROCK,
	GTYPE_METAL,
	GTYPE_CARBON,
	GTYPE_URANIUM,
	NUM_GROUND_TYPES
};

extern TileMapMode	tmapmode;
extern char 		mapx, mapy, tmapx, tmapy;


void tiles_init(void);

void tiles_draw(char sx, char sy);

void tile_dig(char x, char y);

void tile_cursor(char x, char y);

char tile_plan(char si, char di);

#pragma compile("tiles.c")

#endif

