#include <c64/vic.h>
#include <oscar.h>
#include <c64/memmap.h>
#include <c64/cia.h>
#include <c64/sprites.h>
#include <string.h>
#include <c64/joystick.h>

static char * const Screen	=	(char *)0xc000;
static char * const Color	=	(char *)0xd800; 
static char * const Sprites =  (char *)0xc400;
static char * const Hires	=	(char *)0xe000;

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

const char BunkerMapData[] = {
	#embed ctm_map8 "bunkerbm.ctm"
};

#pragma align(BunkerHiresData, 256)
#pragma align(BunkerColor0Data, 256)
#pragma align(BunkerColor1Data, 256)

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

void display_init(void)
{
	mmap_set(MMAP_NO_BASIC);

	mmap_trampoline();

	// Disable interrupts while setting up
	__asm { sei };

	// Kill CIA interrupts
	cia_init();

	mmap_set(MMAP_NO_ROM);

	vic_setmode(VICM_HIRES_MC, Screen, Hires);

	vic.color_border = VCOL_BLACK;
	vic.color_back = VCOL_BLACK;

	spr_init(Screen);
	vic.spr_mcolor0 = VCOL_BLACK;
	vic.spr_mcolor1 = VCOL_WHITE;

	memset(Screen, 0, 1000);
	memset(Color, 0, 1000);
}

int main(void)
{
	display_init();

	char sx = 0, sy = 0;

	for(;;)
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

		joy_poll(0);
		if (joyx[0] > 0 && sx < 13)
			sx++;
		else if (joyx[0] < 0 && sx > 0)
			sx--;
		if (joyy[0] > 0 && sy < 13)
			sy++;
		else if (joyy[0] < 0 && sy > 0)
			sy--;
	}

	return 0;
}
