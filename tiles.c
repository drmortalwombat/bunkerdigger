#include "tiles.h"
#include "display.h"
#include "window.h"

const char BunkerHiresData[] = {
	#embed ctm_chars "bunkerbm.ctm"
};

const char BunkerColor0Data[] = {
	#embed ctm_attr1 "bunkerbm.ctm"
};

const char BunkerColor1Data[] = {
	#embed ctm_attr2 "bunkerbm.ctm"
};

const unsigned short BunkerTileData[] = {
	#embed ctm_tiles16 word "bunkerbm.ctm"
};


struct BunkerTilesBuffer
{
	char			a[10 * sizeof(BunkerColor0Data)];
};

// Reshufle tiles in compile phase using constexpr
constexpr BunkerTilesBuffer build_BunkerTilesBuffer(void)
{
const char BunkerHiresData[] = {
	#embed ctm_chars "bunkerbm.ctm"
};

const char BunkerColor0Data[] = {
	#embed ctm_attr1 "bunkerbm.ctm"
};

const char BunkerColor1Data[] = {
	#embed ctm_attr2 "bunkerbm.ctm"
};

	BunkerTilesBuffer		btd;

	for(int i=0; i<sizeof(BunkerColor0Data); i++)
	{
		for(int j=0; j<8; j++)
			btd.a[10 * i + j] = BunkerHiresData[8 * i + j];
		btd.a[10 * i + 8] = BunkerColor1Data[i];
		btd.a[10 * i + 9] = BunkerColor0Data[i];
	}
	return btd;
}

const BunkerTilesBuffer	bunkerTilesBuffer = build_BunkerTilesBuffer();

const char *	bunkerTiles[] = {
#for(i, 44 * 64)	bunkerTilesBuffer.a + 10 * BunkerTileData[i],
};


#if 0
char BunkerMapData[] = {
	#embed ctm_map8 "bunkerbm.ctm"
};
#endif

#pragma bss(ybss)
char BunkerMapData[256];
char TileMapFlags[256];
#pragma bss(bss)

#pragma align(BunkerHiresData, 256)
#pragma align(BunkerColor0Data, 256)
#pragma align(BunkerColor1Data, 256)

// Flags describing exits and features of various rooms
const char TileFlags[] = {
	TF_BUNKER | TF_LEFT | TF_RIGHT | TF_UP | TF_DOWN,
	TF_BUNKER | TF_LEFT | TF_UP | TF_DOWN,
	TF_BUNKER | TF_RIGHT | TF_UP | TF_DOWN,
	TF_BUNKER | TF_LR,

	TF_BUNKER | TF_LEFT | TF_RIGHT | TF_DOWN,
	TF_BUNKER | TF_LEFT | TF_DOWN,
	TF_BUNKER | TF_RIGHT | TF_DOWN,
	TF_BUNKER | TF_LEFT | TF_RIGHT | TF_UP,

	TF_BUNKER | TF_LEFT | TF_UP,
	TF_BUNKER | TF_RIGHT | TF_UP,
	TF_BUNKER | TF_UP | TF_DOWN,
	TF_BUNKER | TF_LR,

	TF_BUNKER | TF_RIGHT,
	TF_BUNKER | TF_LEFT,
	TF_BUNKER | TF_DOWN,
	TF_BUNKER | TF_UP,
	
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 

	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 

	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 

	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 

	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 

	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 
	TF_ROOM | TF_BUNKER | TF_LR, 

	TF_NONE, TF_NONE, TF_NONE, TF_NONE
};

// Map layout
struct tile_strata
{
	GroundType	type;
	char		x, y, w, h;
} tiles_strata[] = {
	{GTYPE_ROCK,  0,  4,  5, 1},
	{GTYPE_ROCK,  4,  5,  4, 1},
	{GTYPE_ROCK,  8,  4,  2, 1},
	{GTYPE_ROCK, 11,  6,  5, 1},	 

	{GTYPE_ROCK,  4,  8,  6, 1},	 
	{GTYPE_ROCK,  0,  9,  5, 1},	 	
	{GTYPE_ROCK,  9,  9,  2, 1},
	{GTYPE_ROCK, 10, 10,  6, 1},

	{GTYPE_ROCK,  3, 12,  3, 1},	 
	{GTYPE_ROCK,  6, 12,  2, 1},	 
	{GTYPE_ROCK,  6, 13,  3, 1},

	{GTYPE_ROCK, 13, 15,  3, 1},

	{GTYPE_METAL  | 0xf8,  11,  2, 2, 1},	 
	{GTYPE_METAL  | 0xf8,  11,  3, 2, 1},	 
	{GTYPE_METAL  | 0xf8,   1,  7, 2, 1},	 
	{GTYPE_METAL  | 0xf8,  13,  7, 2, 1},	 
	{GTYPE_METAL  | 0xf8,   1, 10, 3, 1},	 

	{GTYPE_CARBON | 0x78,   2,  3, 1, 1},	 
	{GTYPE_CARBON | 0x78,   6,  4, 1, 1},	 
	{GTYPE_CARBON | 0x78,   1,  5, 2, 1},	 
	{GTYPE_CARBON | 0x78,  12,  9, 3, 1},	 
	{GTYPE_CARBON | 0x78,   1,  8, 2, 1},	 

	{GTYPE_CARBON | 0x78,  12, 11, 3, 1},	 
	{GTYPE_CARBON | 0x78,   6, 14, 3, 1},	 

	{GTYPE_URANIUM | 0x78,   2, 13, 2, 1},	 
	{GTYPE_URANIUM | 0x78,   1, 15, 2, 1},	 
	{GTYPE_URANIUM | 0x78,  13, 13, 2, 1},	 
};


TileEffect	tile_effect;

// Init tiles and map
void tiles_init(void)
{
	for(int i=0; i<256; i++)
	{
		TileMapFlags[i] = 0;
		BunkerMapData[i] = 40 + (((const char *)0xa000)[i] & 1);
	}

	BunkerMapData[ 7] = 12;
	BunkerMapData[ 8] = 16 + RTILE_WORKBENCH;
	BunkerMapData[ 9] = 16 + RTILE_QUARTERS;
	BunkerMapData[10] = 13;


	for(char i=0; i<sizeof(tiles_strata)/sizeof(tile_strata); i++)
	{
		char ip = tiles_strata[i].y * 16 + tiles_strata[i].x;
		char w = tiles_strata[i].w, h = tiles_strata[i].h;
		GroundType t = tiles_strata[i].type;

		for(char y=0; y<h; y++)
		{
			for(char x=0; x<w; x++)
			{
				TileMapFlags[ip + x] = t;
				if (t == GTYPE_ROCK)
					BunkerMapData[ip + x] = 42;
			}
			ip += 16;
		}
	}

	tile_effect = TILEF_NORMAL;
}


bool tile_is_bunker(char x, char y)
{
	char tf = TileFlags[BunkerMapData[16 * y + x]];
	return (tf & TF_BUNKER) != 0;
}

bool tile_is_room(char x, char y)
{
	char tf = TileFlags[BunkerMapData[16 * y + x]];
	return (tf & TF_ROOM) != 0;
}

char tile_flags(char x, char y)
{
	return TileFlags[BunkerMapData[16 * y + x]];
}

bool tile_expand(char x, char y, char flags)
{
	flags |= TileFlags[BunkerMapData[16 * y + x]];

	char i = 0;
	while (i < 16 && TileFlags[i] != flags)
		i++;

	if (i < 16)
	{
		BunkerMapData[16 * y + x] = i;
		return true;
	}

	return false;
}

// Build a bunker at a given spot, connecting it to neighbouring bunkers
bool tile_dig(char x, char y)
{
	if (TileMapFlags[16 * y + x] == GTYPE_ROCK && !room_count[RTILE_EXCAVATOR])
	{
		story_pending |= 1ul << STM_DIGGING_ROCK;
		return false;
	}

	if (tile_is_bunker(x, y) || tile_is_room(x, y))
		return false;

	char	flags = TF_BUNKER;
	if (x > 0 && tile_is_bunker(x - 1, y))
		flags |= TF_LEFT;
	if (x < 15 && tile_is_bunker(x + 1, y))
		flags |= TF_RIGHT;

	if (flags == TF_BUNKER)
	{
		if (y > 0 && tile_is_bunker(x, y - 1) && !tile_is_room(x, y - 1))
			flags |= TF_UP;
		if (y < 15 && tile_is_bunker(x, y + 1) && !tile_is_room(x, y + 1))
			flags |= TF_DOWN;
	}
	else
	{
		if (y > 0 && (tile_flags(x, y - 1) & TF_UP))
			flags |= TF_UP;
		if (y < 15 && (tile_flags(x, y + 1) & TF_DOWN))
			flags |= TF_DOWN;
	}

	if (tile_expand(x, y, flags))
	{
		if (flags & TF_LEFT)
			tile_expand(x - 1, y, TF_RIGHT);
		if (flags & TF_RIGHT)
			tile_expand(x + 1, y, TF_LEFT);
		if (flags & TF_UP)
			tile_expand(x, y - 1, TF_DOWN);
		if (flags & TF_DOWN)
			tile_expand(x, y + 1, TF_UP);

		return true;
	}
	else
		return false;
}

// Colors for resource types
static char tile_ground_color[] = {
	VCOL_BROWN,
	VCOL_DARK_GREY,
	VCOL_MED_GREY,
	VCOL_BLACK,
	VCOL_PURPLE
};

void tile_draw_p(char tile, char flags, char * hp, char * sp, char * cp)
{
	const char * const * tp = bunkerTiles + 64 * tile;

	for(char iy=0; iy<8; iy++)
	{
		for(char ix=0; ix<8; ix++)
		{
			const char * shp = tp[ix];
			#pragma unroll(full)
			for(char i=0; i<8; i++)
				hp[i] = shp[i];

			sp[ix] = shp[8];
			cp[ix] = shp[9];
			hp += 8;
		}

		hp += 320 - 64;
		sp += 40;
		cp += 40;
		tp += 8;
	}

	char	gc = tile_ground_color[flags & GROUND_TYPE_MASK];
	cp -= 40;
	for(char ix=0; ix<8; ix++)
		cp[ix] = gc;
	cp -= 7 * 40;
	cp[0] = gc;
	cp[7] = gc;
}

void tile_draw_g(char tile, char flags, char * hp, char * sp, char * cp)
{
	const char * const * tp = bunkerTiles + 64 * tile;
	char	gc = tile_ground_color[flags & GROUND_TYPE_MASK];

	for(char iy=0; iy<8; iy++)
	{
		for(char ix=0; ix<8; ix++)
		{
			const char * shp = tp[ix];
			#pragma unroll(full)
			for(char i=0; i<8; i++)
				hp[i] = shp[i];

			sp[ix] = shp[8];
			cp[ix] = gc;
			hp += 8;
		}

		hp += 320 - 64;
		sp += 40;
		cp += 40;
		tp += 8;
	}
}

void tile_draw_e(char tile, char flags, char * hp, char * sp, char * cp)
{
	const char * const * tp = bunkerTiles + 64 * tile;

	char	gc = VCOL_DARK_GREY | 16 * VCOL_DARK_GREY, gs = VCOL_DARK_GREY | 16 * VCOL_DARK_GREY;

	if (tile_effect == TILEF_ESCAPED)
	{
		gc = VCOL_RED;
		gs = VCOL_DARK_GREY | 16 * VCOL_MED_GREY;
	}

	for(char iy=0; iy<8; iy++)
	{
		for(char ix=0; ix<8; ix++)
		{
			const char * shp = tp[ix];
			#pragma unroll(full)
			for(char i=0; i<8; i++)
				hp[i] = shp[i];

			sp[ix] = gs;
			cp[ix] = gc;
			hp += 8;
		}

		hp += 320 - 64;
		sp += 40;
		cp += 40;
		tp += 8;
	}
}

inline void tile_draw(char tile, char flags, char * hp, char * sp, char * cp)
{
	if (tile_effect)
		tile_draw_e(tile, flags, hp, sp, cp);
	else if (TileFlags[tile] & TF_ROOM)
		tile_draw_p(tile, flags, hp, sp, cp);
	else
		tile_draw_g(tile, flags, hp, sp, cp);
}


static const char tile_draw_tile_offset[9] = {
	0x00, 0x01, 0x02,
	0x10, 0x11, 0x12,
	0x20, 0x21, 0x22
};

static __striped char * const tile_draw_hires[9] = {
	Hires + 320 * 8 * 0 + 8 * 8 * 0,	
	Hires + 320 * 8 * 0 + 8 * 8 * 1,
	Hires + 320 * 8 * 0 + 8 * 8 * 2,	

	Hires + 320 * 8 * 1 + 8 * 8 * 0,	
	Hires + 320 * 8 * 1 + 8 * 8 * 1,
	Hires + 320 * 8 * 1 + 8 * 8 * 2,	

	Hires + 320 * 8 * 2 + 8 * 8 * 0,	
	Hires + 320 * 8 * 2 + 8 * 8 * 1,
	Hires + 320 * 8 * 2 + 8 * 8 * 2
};

static __striped char * const tile_draw_screen[9] = {
	Screen + 40 * 8 * 0 + 8 * 0,	
	Screen + 40 * 8 * 0 + 8 * 1,
	Screen + 40 * 8 * 0 + 8 * 2,	

	Screen + 40 * 8 * 1 + 8 * 0,	
	Screen + 40 * 8 * 1 + 8 * 1,
	Screen + 40 * 8 * 1 + 8 * 2,	

	Screen + 40 * 8 * 2 + 8 * 0,	
	Screen + 40 * 8 * 2 + 8 * 1,
	Screen + 40 * 8 * 2 + 8 * 2
};

static __striped char * const tile_draw_color[9] = {
	Color + 40 * 8 * 0 + 8 * 0,	
	Color + 40 * 8 * 0 + 8 * 1,
	Color + 40 * 8 * 0 + 8 * 2,	

	Color + 40 * 8 * 1 + 8 * 0,	
	Color + 40 * 8 * 1 + 8 * 1,
	Color + 40 * 8 * 1 + 8 * 2,	

	Color + 40 * 8 * 2 + 8 * 0,	
	Color + 40 * 8 * 2 + 8 * 1,
	Color + 40 * 8 * 2 + 8 * 2
};




void tiles_draw(char sx, char sy)
{
	char tb = sy * 16 + sx;
	for(char i=0; i<9; i++)
	{
		char ti = tb + tile_draw_tile_offset[i];
		tile_draw(BunkerMapData[ti], TileMapFlags[ti], tile_draw_hires[i], tile_draw_screen[i], tile_draw_color[i]);
	}
#if 0
	tile_draw(BunkerMapData[(sy + 0) * 16 + (sx + 0)], TileMapFlags[(sy + 0) * 16 + (sx + 0)], 8 * 0, 8 * 0);
	tile_draw(BunkerMapData[(sy + 0) * 16 + (sx + 1)], TileMapFlags[(sy + 0) * 16 + (sx + 1)], 8 * 1, 8 * 0);
	tile_draw(BunkerMapData[(sy + 0) * 16 + (sx + 2)], TileMapFlags[(sy + 0) * 16 + (sx + 2)], 8 * 2, 8 * 0);

	tile_draw(BunkerMapData[(sy + 1) * 16 + (sx + 0)], TileMapFlags[(sy + 1) * 16 + (sx + 0)], 8 * 0, 8 * 1);
	tile_draw(BunkerMapData[(sy + 1) * 16 + (sx + 1)], TileMapFlags[(sy + 1) * 16 + (sx + 1)], 8 * 1, 8 * 1);
	tile_draw(BunkerMapData[(sy + 1) * 16 + (sx + 2)], TileMapFlags[(sy + 1) * 16 + (sx + 2)], 8 * 2, 8 * 1);

	tile_draw(BunkerMapData[(sy + 2) * 16 + (sx + 0)], TileMapFlags[(sy + 2) * 16 + (sx + 0)], 8 * 0, 8 * 2);
	tile_draw(BunkerMapData[(sy + 2) * 16 + (sx + 1)], TileMapFlags[(sy + 2) * 16 + (sx + 1)], 8 * 1, 8 * 2);
	tile_draw(BunkerMapData[(sy + 2) * 16 + (sx + 2)], TileMapFlags[(sy + 2) * 16 + (sx + 2)], 8 * 2, 8 * 2);
#endif
}

static char cursor_or_0[] = {
	0x00, 0x00, 0x14, 0x14, 0x10, 0x10,
	0x00, 0x00,
	0x10, 0x10, 0x14, 0x14, 0x00, 0x00
};
static char cursor_or_1[] = {
	0x00, 0x00, 0x14, 0x14, 0x04, 0x04, 
	0x00, 0x00,
	0x04, 0x04, 0x14, 0x14, 0x00, 0x00
};

static char cursor_and_0[] = {
	0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 
	0x0f, 0x0f,
	0x03, 0x03, 0x00, 0x00, 0x00, 0x00
};
static char cursor_and_1[] = {
	0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 
	0xf0, 0xf0,
	0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00
};

char cursor_back[4][8];

// Draw map cursor, saving tile images in variable
void tile_cursor_show(char x, char y)
{
	__assume(y < 3);
	__assume(x < 3);

	char * hp0 = HiresRow[y * 8] + (x * 8) * 8;
	char * hp1 = hp0 + 320 * 7;

	for(char i=0; i<8; i++)
	{
		char	t;

		cursor_back[0][i] = t = hp0[i];
		hp0[i     ] = t & cursor_and_0[i] | cursor_or_0[i];
		cursor_back[1][i] = t = hp0[i + 56];
		hp0[i + 56] = t & cursor_and_1[i] | cursor_or_1[i];
		cursor_back[2][i] = t = hp1[i];
		hp1[i     ] = t & cursor_and_0[i + 6] | cursor_or_0[i + 6];
		cursor_back[3][i] = t = hp1[i + 56];
		hp1[i + 56] = t & cursor_and_1[i + 6] | cursor_or_1[i + 6];
	}
}

// Restore tiles under map cursor
void tile_cursor_hide(char x, char y)
{
	__assume(y < 3);
	__assume(x < 3);

	char * hp0 =  HiresRow[y * 8] + (x * 8) * 8;
	char * hp1 = hp0 + 320 * 7;

	for(char i=0; i<8; i++)
	{
		hp0[i]= cursor_back[0][i];
		hp0[i + 56] = cursor_back[1][i];
		hp1[i] = cursor_back[2][i];
		hp1[i + 56] = cursor_back[3][i];
	}
}


#pragma bss(ybss)
char queue[256];
signed char dist[256];
#pragma bss(bss)

// Path finding from si to di
char tile_plan(char si, char di)
{
	// Check if already at target
	if (si == di)
		return di;

	// Clear distance for all map entries
	for(int i=0; i<256; i++)
		dist[i] = 0;

	// Init target spot as start of search space
	dist[di] = 99;
	queue[0] = di;

	// Loop while there are ways to explore
	char ri = 0, wi = 1;
	while (ri < wi)
	{
		// Get next queue entry
		char p = queue[ri++];

		// Check if start reached
		if (p == si)
		{
			// Find direct move from start tile to first
			// deviation from initial direction
			signed char dir = dist[p];

			do {
				p += dir;
			} while (dist[p] == dir);

			return p;
		}
		else
		{
			// Flood fill
			char tf = TileFlags[BunkerMapData[p]];

			if ((tf & TF_LEFT) && dist[(char)(p - 1)] == 0)
			{				
				dist[(char)(p - 1)] = 1;
				queue[wi++] = (char)(p - 1);
			}
			if ((tf & TF_RIGHT) && dist[(char)(p + 1)] == 0)
			{				
				dist[(char)(p + 1)] = -1;
				queue[wi++] = (char)(p + 1);
			}
			if ((tf & TF_UP) && dist[(char)(p - 16)] == 0)
			{				
				dist[(char)(p - 16)] = 16;
				queue[wi++] = (char)(p - 16);
			}
			if ((tf & TF_DOWN) && dist[(char)(p + 16)] == 0)
			{				
				dist[(char)(p + 16)] = -16;
				queue[wi++] = (char)(p + 16);
			}
		}
	}

	return si;
}

