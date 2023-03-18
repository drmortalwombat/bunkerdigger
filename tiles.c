#include "tiles.h"
#include "display.h"

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

#if 0
char BunkerMapData[] = {
	#embed ctm_map8 "bunkerbm.ctm"
};
#endif

char BunkerMapData[256];

#pragma align(BunkerHiresData, 256)
#pragma align(BunkerColor0Data, 256)
#pragma align(BunkerColor1Data, 256)

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

struct tile_strata
{
	GroundType	type;
	char		x, y, w, h;
} tiles_strata[] = {
	{GTYPE_ROCK,  2,  1,  4, 1},
	{GTYPE_ROCK,  8,  3,  1, 2},
	{GTYPE_ROCK, 11,  5,  3, 1},
	{GTYPE_ROCK,  4,  9,  3, 1},
	{GTYPE_ROCK,  9,  11, 4, 2},	 

	{GTYPE_METAL,  12,  2, 5, 1},	 
	{GTYPE_CARBON, 5,   6, 3, 1},	 
};



void tiles_init(void)
{
	memset(TileMapFlags, 0, 256);

	for(int i=0; i<256; i++)
		BunkerMapData[i] = 40 + (rand() & 1);

	BunkerMapData[ 7] = 12;
	BunkerMapData[ 8] = 16 + RTILE_WORKBENCH;
	BunkerMapData[ 9] = 16 + RTILE_QUARTERS;
	BunkerMapData[10] = 13;


	for(char i=0; i<7; i++)
	{
		char ip = tiles_strata[i].y * 16 + tiles_strata[i].x;
		char w = tiles_strata[i].w, h = tiles_strata[i].h;
		GroundType t = tiles_strata[i].type;

		for(char y=0; y<h; y++)
		{
			for(char x=0; x<w; x++)
				TileMapFlags[ip + x] = t;
			ip += 16;
		}
	}
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

void tile_dig(char x, char y)
{
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

	tile_expand(x, y, flags);
	if (flags & TF_LEFT)
		tile_expand(x - 1, y, TF_RIGHT);
	if (flags & TF_RIGHT)
		tile_expand(x + 1, y, TF_LEFT);
	if (flags & TF_UP)
		tile_expand(x, y - 1, TF_DOWN);
	if (flags & TF_DOWN)
		tile_expand(x, y + 1, TF_UP);
}

static char tile_ground_color[] = {
	VCOL_BROWN,
	VCOL_LT_GREY,
	VCOL_MED_GREY,
	VCOL_BLACK,
	VCOL_PURPLE
};

void tile_draw_p(char tile, char flags, char * hp, char * sp, char * cp)
{
	const unsigned short * tp = BunkerTileData + 64 * tile;

	for(char iy=0; iy<8; iy++)
	{
		for(char ix=0; ix<8; ix++)
		{
			unsigned short c = tp[ix];
			const char * shp = BunkerHiresData + 8 * c;
			#pragma unroll(full)
			for(char i=0; i<8; i++)
				hp[i] = shp[i];

			sp[ix] = BunkerColor1Data[c];
			cp[ix] = BunkerColor0Data[c];
			hp += 8;
		}

		hp += 320 - 64;
		sp += 40;
		cp += 40;
		tp += 8;
	}

	char	gc = tile_ground_color[flags & 0x0f];
	cp -= 40;
	for(char ix=0; ix<8; ix++)
		cp[ix] = gc;
	cp -= 7 * 40;
	cp[0] = gc;
	cp[7] = gc;
}

void tile_draw_g(char tile, char flags, char * hp, char * sp, char * cp)
{
	const unsigned short * tp = BunkerTileData + 64 * tile;

	char	gc = tile_ground_color[flags & 0x0f];

	for(char iy=0; iy<8; iy++)
	{
		for(char ix=0; ix<8; ix++)
		{
			unsigned short c = tp[ix];
			const char * shp = BunkerHiresData + 8 * c;
			#pragma unroll(full)
			for(char i=0; i<8; i++)
				hp[i] = shp[i];

			sp[ix] = BunkerColor1Data[c];
			cp[ix] = gc;
			hp += 8;
		}

		hp += 320 - 64;
		sp += 40;
		cp += 40;
		tp += 8;
	}

}

inline void tile_draw(char tile, char flags, char x, char y)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = Hires + 320 * y + x * 8;
	char * sp = Screen + 40 * y + x;
	char * cp = Color + 40 * y + x;

	if (TileFlags[tile] & TF_ROOM)
		tile_draw_p(tile, flags, hp, sp, cp);
	else
		tile_draw_g(tile, flags, hp, sp, cp);
}


void tiles_draw(char sx, char sy)
{
	tile_draw(BunkerMapData[(sy + 0) * 16 + (sx + 0)], TileMapFlags[(sy + 0) * 16 + (sx + 0)], 8 * 0, 8 * 0);
	tile_draw(BunkerMapData[(sy + 0) * 16 + (sx + 1)], TileMapFlags[(sy + 0) * 16 + (sx + 1)], 8 * 1, 8 * 0);
	tile_draw(BunkerMapData[(sy + 0) * 16 + (sx + 2)], TileMapFlags[(sy + 0) * 16 + (sx + 2)], 8 * 2, 8 * 0);

	tile_draw(BunkerMapData[(sy + 1) * 16 + (sx + 0)], TileMapFlags[(sy + 1) * 16 + (sx + 0)], 8 * 0, 8 * 1);
	tile_draw(BunkerMapData[(sy + 1) * 16 + (sx + 1)], TileMapFlags[(sy + 1) * 16 + (sx + 1)], 8 * 1, 8 * 1);
	tile_draw(BunkerMapData[(sy + 1) * 16 + (sx + 2)], TileMapFlags[(sy + 1) * 16 + (sx + 2)], 8 * 2, 8 * 1);

	tile_draw(BunkerMapData[(sy + 2) * 16 + (sx + 0)], TileMapFlags[(sy + 2) * 16 + (sx + 0)], 8 * 0, 8 * 2);
	tile_draw(BunkerMapData[(sy + 2) * 16 + (sx + 1)], TileMapFlags[(sy + 2) * 16 + (sx + 1)], 8 * 1, 8 * 2);
	tile_draw(BunkerMapData[(sy + 2) * 16 + (sx + 2)], TileMapFlags[(sy + 2) * 16 + (sx + 2)], 8 * 2, 8 * 2);
}

static char cursor_xor_0[] = {
	0xff, 0xff, 0xe8, 0xe8, 0xe0, 0xe0, 
	0xc0, 0xc0,
	0xe0, 0xe0, 0xe8, 0xe8, 0xff, 0xff
};
static char cursor_xor_1[] = {
	0xff, 0xff, 0x2b, 0x2b, 0x0b, 0x0b, 
	0x03, 0x03,
	0x0b, 0x0b, 0x2b, 0x2b, 0xff, 0xff
};

void tile_cursor(char x, char y)
{
	__assume(y < 3);
	__assume(x < 3);

	char * hp0 = Hires + 320 * (y * 8) + (x * 8) * 8;
	char * hp1 = hp0 + 320 * 7;

	for(char i=0; i<8; i++)
	{
		hp0[i] ^= cursor_xor_0[i];
		hp0[i + 56] ^= cursor_xor_1[i];
		hp1[i] ^= cursor_xor_0[i + 6];
		hp1[i + 56] ^= cursor_xor_1[i + 6];
	}
}

char queue[256];
signed char dist[256];

char tile_plan(char si, char di)
{
	if (si == di)
		return di;

	for(int i=0; i<256; i++)
		dist[i] = 0;

	dist[di] = 99;
	queue[0] = di;

	char ri = 0, wi = 1;
	while (ri < wi)
	{
		char p = queue[ri++];
		if (p == si)
		{
			signed char dir = dist[p];

			do {
				p += dir;
			} while (dist[p] == dir);

			return p;
		}
		else
		{
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

