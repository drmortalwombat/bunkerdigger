#include <c64/vic.h>
#include <oscar.h>
#include <c64/memmap.h>
#include <c64/cia.h>
#include <c64/sprites.h>
#include <string.h>
#include <c64/joystick.h>
#include <c64/rasterirq.h>
#include <stdlib.h>

static char * const Screen	=	(char *)0xc000;
static char * const Color	=	(char *)0xd800; 
static char * const Sprites =  (char *)0xd000;
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

const char SpriteData[] = {
	#embed spd_sprites lzo "sprites.spd"
};

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

	mmap_set(MMAP_RAM);

	oscar_expand_lzo(Sprites, SpriteData);

	mmap_set(MMAP_NO_ROM);

	vic_setmode(VICM_HIRES_MC, Screen, Hires);

	vic.color_border = VCOL_BLACK;
	vic.color_back = VCOL_BLACK;

	vic.spr_mcolor0 = VCOL_BLACK;
	vic.spr_mcolor1 = VCOL_WHITE;
	vic.spr_enable = 0xff;
	vic.spr_multi = 0xff;
	vic.spr_expand_x = 0x00;
	vic.spr_expand_y = 0x00;

	memset(Screen, 0, 1000);
	memset(Color, 0, 1000);

	rirq_init_io();
	vspr_init(Screen);	
}

struct Digger
{
	char tx, ty;
	char sx, sy;
	char mi, mt;
	char color;
	signed char dx;
};

__striped Digger	diggers[32];

char phase = 0;
char sx = 0, sy = 0;

void digger_move(void)
{
	char si = 0;

	for(char i=0; i<32; i++)
	{	
		bool t = !((phase ^ i) & 3);

		if (diggers[i].dx < 0)
		{
			if (diggers[i].sx == 0)
			{
				diggers[i].tx--;
				diggers[i].sx = 63;
			}
			else
				diggers[i].sx--;

			if (diggers[i].sx == 32)
			{
				if (diggers[i].tx == 0)
				{
					diggers[i].dx = 1;
					diggers[i].mi = 64 + 11;
				}
			}

			if (t)
			{
				diggers[i].mi++;
				if (diggers[i].mi == 64 + 9)
					diggers[i].mi = 64 + 3;
			}
		}
		else if (diggers[i].dx > 0)
		{
			if (diggers[i].sx == 63)
			{
				diggers[i].tx++;
				diggers[i].sx = 0;
			}
			else
				diggers[i].sx++;

			if (diggers[i].sx == 32)
			{
				if (diggers[i].tx == 8)
				{
					diggers[i].dx = -1;
					diggers[i].mi = 64 + 3;
				}
			}

			if (t)
			{
				diggers[i].mi++;
				if (diggers[i].mi == 64 + 17)
					diggers[i].mi = 64 + 11;
			}
		}

		if (diggers[i].ty >= sy && diggers[i].ty < sy + 3 &&
			diggers[i].tx >= sx && diggers[i].tx < sx + 3)
		{
			vspr_set(si, 
				12 + (char)(diggers[i].tx - sx) * 64 + diggers[i].sx,
				50 + (char)(diggers[i].ty - sy) * 64 + diggers[i].sy,
				diggers[i].mi, diggers[i].color);
			si++;
		}		
	}

	vic.color_border++;

	while (si < 16)
	{
		vspr_move(si, 0, 255);
		si++;
	}

	phase++;
}

int main(void)
{
	display_init();

	for(char i=0; i<32; i++)
	{
		diggers[i].tx = (i % 8) + 1;
		diggers[i].ty = i / 8; 
		diggers[i].sy = 26;
		diggers[i].sx = 8 + rand() % 16;
		diggers[i].color = 2 + (i % 14);

		if (rand() & 1)
		{
			diggers[i].dx = 1;
			diggers[i].mi = 64 + 11;
		}
		else
		{
			diggers[i].dx = -1;
			diggers[i].mi = 64 + 7;
		}
	}

	vspr_sort();
	vspr_update();
	rirq_sort();

	// start raster IRQ processing
	rirq_start();

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


		do {
			vic.color_border++;
			digger_move();
			vic.color_border++;
			vspr_sort();
			vic.color_border = 0;

			rirq_wait();
			vic.color_border++;
			vspr_update();
			vic.color_border++;
			rirq_sort();
			joy_poll(0);

		} while (!joyx[0] && !joyy[0]);

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
