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

char BunkerMapData[] = {
	#embed ctm_map8 "bunkerbm.ctm"
};

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

	TF_NONE, TF_NONE, TF_NONE, TF_NONE
};


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

void tile_draw_p(char tile, char * hp, char * sp, char * cp)
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

}

inline void tile_draw(char tile, char x, char y)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = Hires + 320 * y + x * 8;
	char * sp = Screen + 40 * y + x;
	char * cp = Color + 40 * y + x;

	tile_draw_p(tile, hp, sp, cp);
}


void tiles_draw(char sx, char sy)
{
	tile_draw(BunkerMapData[(sy + 0) * 16 + (sx + 0)], 8 * 0, 8 * 0);
	tile_draw(BunkerMapData[(sy + 0) * 16 + (sx + 1)], 8 * 1, 8 * 0);
	tile_draw(BunkerMapData[(sy + 0) * 16 + (sx + 2)], 8 * 2, 8 * 0);

	tile_draw(BunkerMapData[(sy + 1) * 16 + (sx + 0)], 8 * 0, 8 * 1);
	tile_draw(BunkerMapData[(sy + 1) * 16 + (sx + 1)], 8 * 1, 8 * 1);
	tile_draw(BunkerMapData[(sy + 1) * 16 + (sx + 2)], 8 * 2, 8 * 1);

	tile_draw(BunkerMapData[(sy + 2) * 16 + (sx + 0)], 8 * 0, 8 * 2);
	tile_draw(BunkerMapData[(sy + 2) * 16 + (sx + 1)], 8 * 1, 8 * 2);
	tile_draw(BunkerMapData[(sy + 2) * 16 + (sx + 2)], 8 * 2, 8 * 2);
}

static char cursor_xor_0[] = {0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xff, 0xff};
static char cursor_xor_1[] = {0xff, 0xff, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xff, 0xff};

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
		hp1[i] ^= cursor_xor_0[i + 2];
		hp1[i + 56] ^= cursor_xor_1[i + 2];
	}
}
