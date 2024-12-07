#ifndef TILES_H
#define TILES_H

// Tile flags
#define TF_NONE		0x00
#define TF_LEFT		0x01
#define TF_RIGHT	0x02
#define TF_UP		0x04
#define TF_DOWN		0x08

#define TF_BUNKER	0x10
#define TF_ROOM		0x20

#define TF_LR		TF_LEFT | TF_RIGHT

// Ranges of tiles
#define TILE_FLOOR	0x03
#define TILE_ROOMS	0x10
#define TILE_GROUND	0x28

// Array of tile flags per room
extern const char TileFlags[];

// Map data
extern char BunkerMapData[256];
extern char TileMapFlags[256];

// Current map mode
enum TileMapMode
{
	TMMODE_REDRAW,
	TMMODE_DRAWN,
	TMMODE_CURSOR
};

// Types of rooms in map
enum RoomTile
{
	RTILE_QUARTERS,
	RTILE_WORKBENCH,
	RTILE_HYDRO,
	RTILE_GENERATOR,

	RTILE_MINE,
	RTILE_STORAGE,
	RTILE_LABORATORY,
	RTILE_VENTILATION,

	RTILE_GYM,
	RTILE_ARMOURY,
	RTILE_SICKBAY,
	RTILE_RADIO,

	RTILE_STUDY,
	RTILE_EXCAVATOR,
	RTILE_CENTRIFUGE,
	
	RTILE_LAUNCH_TOP,
	RTILE_LAUNCH_MID,
	RTILE_LAUNCH_BOTTOM,
	RTILE_MISSILE_TOP,
	RTILE_MISSILE_MID,
	RTILE_MISSILE_BOTTOM,

	RTILE_CONSTRUCTION
};

// Type of ground
enum GroundType
{
	GTYPE_SOIL,
	GTYPE_ROCK,
	GTYPE_METAL,
	GTYPE_CARBON,
	GTYPE_URANIUM,
	NUM_GROUND_TYPES
};

#define GROUND_TYPE_MASK		0x07
#define GROUND_TYPE_RESOURCE	0xf8

// Map display status
extern TileMapMode	tmapmode;
extern char 		mapx, mapy, tmapx, tmapy;

// Global tile display effect
enum TileEffect
{
	TILEF_NORMAL,
	TILEF_EXPLODED,
	TILEF_ESCAPED
};

extern TileEffect	tile_effect;

// Init tile system
void tiles_init(void);

// Is tile a bunker (not ground)
bool tile_is_bunker(char x, char y);

// Is tile a room
bool tile_is_room(char x, char y);

// Get tile flags
char tile_flags(char x, char y);

// Draw tiles in current map view range
void tiles_draw(char sx, char sy);

// Dig a room/bunker into a ground tile
bool tile_dig(char x, char y);

// Show tile map cursor
void tile_cursor_show(char x, char y);

// Hide tile map cursor
void tile_cursor_hide(char x, char y);

// Path finding for diggers from si to di, returns next sub target tile in staight line
char tile_plan(char si, char di);

// Draw room tile
void tile_draw_p(char tile, char flags, char * hp, char * sp, char * cp);

// Draw ground tile
void tile_draw_g(char tile, char flags, char * hp, char * sp, char * cp);

#pragma compile("tiles.c")

#endif

