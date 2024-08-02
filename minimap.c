#include "minimap.h"
#include "display.h"

const char MinimapHiresData[] = {
	#embed ctm_chars "minimap.ctm"
};

#pragma align(MinimapHiresData, 256)

__striped char * const MinimapHiresTab[16] = {
	Hires + 24 * 8 + 0 * 320,
	Hires + 24 * 8 + 1 * 320,
	Hires + 24 * 8 + 2 * 320,
	Hires + 24 * 8 + 3 * 320,

	Hires + 24 * 8 + 4 * 320,
	Hires + 24 * 8 + 5 * 320,
	Hires + 24 * 8 + 6 * 320,
	Hires + 24 * 8 + 7 * 320,

	Hires + 24 * 8 + 8 * 320,
	Hires + 24 * 8 + 9 * 320,
	Hires + 24 * 8 + 10 * 320,
	Hires + 24 * 8 + 11 * 320,

	Hires + 24 * 8 + 12 * 320,
	Hires + 24 * 8 + 13 * 320,
	Hires + 24 * 8 + 14 * 320,
	Hires + 24 * 8 + 15 * 320
};

void minimap_draw(void)
{
	for(char y=0; y<16; y++)
	{
		for(char x=0; x<16; x++)
		{
			char * hp = Hires + 320 * y + x * 8 + 24 * 8;
			char * sp = Screen + 40 * y + x + 24;
			char * cp = Color + 40 * y + x + 24;

			char tile = BunkerMapData[x + 16 * y];

			const char * shp = MinimapHiresData + 8 * tile;
			#pragma unroll(full)
			for(char i=0; i<8; i++)
				hp[i] = shp[i];

			sp[0] = VCOL_LT_GREY * 16 | VCOL_ORANGE;
			cp[0] = VCOL_DARK_GREY;
		}
	}
}

void minimap_highlight(char sx, char sy)
{
	for(char y=0; y<16; y++)
	{
		for(char x=0; x<16; x++)
		{
			char * sp = Screen + 40 * y + x + 24;
			char * cp = Color + 40 * y + x + 24;

			if ((char)(x - sx) < 3 && (char)(y - sy) < 3)
			{
				sp[0] = VCOL_WHITE * 16 | VCOL_ORANGE;
				cp[0] = VCOL_RED;
			}
			else
			{
 				sp[0] = VCOL_LT_GREY * 16 | VCOL_ORANGE;
				cp[0] = VCOL_DARK_GREY;
			}
		}
	}
}

