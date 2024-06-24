#include <c64/joystick.h>
#include <c64/keyboard.h>
#include <c64/sprites.h>
#include <c64/vic.h>

#include "intro.h"
#include "display.h"
#include "tiles.h"
#include "gameirq.h"
#include "gamemusic.h"

char introrooms[5 * 3] = {
	0x06, 0x24, 0x16, 0x1d, 0x00, 
	0x0a, 0x28, 0x29, 0x28, 0x0a, 
	0x07, 0x1e, 0x04, 0x10, 0x01, 
};

const char intro_text[] = 
	"AFTER DECADES OF COLD WAR, OUR ENEMIES\n"
	"LEFT US NO OPTION BUT TO STRIKE FIRST.\n"
	"RETALIATION WAS TO BE EXPECTED.\n"
	"TO SAVE OUR GREAT COUNTRY FROM\n"
	"TOTAL ANNIHILATION, WE PUT OUR BEST\n"
	"INTO A SYSTEM OF UNDERGROUND SHELTERS,\n"
	"TO BE READY TO REBUILD THIS WORLD\n"
	"WHEN CONDITIONS HAD IMPROVED.\n"
	"WELCOME TO --- MINESHAFT GAP ---\n"
	"CODING AND GFX BY DR.MORTALWOMBAT\n"
	"MUSIC BY CRISPS\n";

void display_intro(void)
{
		irqphase = IRQP_INTRO;

	vic.spr_enable = 0xff;
	vic.spr_multi = 0x00;

	for(char i=0; i<8; i++)
		vspr_set(i, 86 + 24 * i, 126, 32 + i, VCOL_YELLOW);
	for(char i=0; i<3; i++)
		vspr_set(i + 8, 144 + 24 * i, 154, 40 + i, VCOL_YELLOW);
	vspr_sort();

//	disp_chars(0, 24, intro_text, 40, 0x00, VCOL_LT_GREEN | 16 * VCOL_GREEN);

//	minimap_highlight(mapx, mapy);			
	

	music_init(TUNE_TITLE);

//	tiles_draw(tmapx, tmapy);

	char * hp = Hires, * sp = Screen, * cp = Color;
	char 	ti = 0;
	for(char y=0; y<3; y++)
	{
		for(char x=0; x<5; x++)
		{
			char tile = introrooms[ti++];
			if (tile & 0x10)
				tile_draw_p(tile, 0, hp, sp, cp); 
			else
				tile_draw_g(tile, 0, hp, sp, cp); 
			hp += 8 * 8;
			sp += 8;
			cp += 8;
		}
		hp += 320 * 7;
		sp += 40 * 7;
		cp += 40 * 7;
	}

	disp_chars_msg(10, 8, "WE MUST NOT ALLOW A", 19, VCOL_BLACK, VCOL_WHITE | 16 * VCOL_LT_GREY);


	static char title_color[] = {
		VCOL_PURPLE, VCOL_YELLOW, VCOL_PURPLE, VCOL_YELLOW, 
		VCOL_PURPLE, VCOL_YELLOW, VCOL_PURPLE, VCOL_YELLOW
	};


	{
		int 	y0 = 0;
		int 	dy0 = 16;

		const char * it = intro_text;

		for(;;)
		{
			char * wp = Hires + 320 * 24;
			char * cp = Screen + 40 * 24;

			char ix = 0;
			for(char i=0; i<128; i++)
			{
				y0 += dy0;
				dy0 += 1;

				if ((y0 >> 4) > 126)
				{
					y0 -= dy0;
					dy0 = -(dy0 >> 1);
				}

				for(char k=0; k<8; k++)
					vspr_movey(k, y0 >> 4);
				for(char k=8; k<11; k++)
					vspr_movey(k, (y0 >> 4) + 28);

				if (i == 0)
				{
					for(char j=0; j<8; j++)
						wp[j] = 0xaa;					
				}

				if (i >= 120)
				{
					char * hp = Hires + 320 * 24;
					for(char i=0; i<40; i++)
					{
						#pragma unroll(full)
						for(char j=0; j<7; j++)
							hp[j] = hp[j + 1];
						hp[7] = 0;
						hp += 8;
					}
				}
				else if (i < 16 || *it == '\n')
				{
					if (ix >= 1)
						cp[ix - 1] = 0x5d;
					switch ((i >> 1) & 3)
					{
					case 0:
					case 2:
						cp[ix] = 0x5d;
						break;
					case 1:
						cp[ix] = 0x00;
						break;
					case 3:
						cp[ix] = 0xd1;
						break;
					}
				}
				else if (i & 1)
				{
					char c = *it++;
					const char * sp = FontHiresData + 8 * (c & 0x3f);
					for(char j=0; j<8; j++)
						wp[j] = sp[j];
					wp += 8;

					if (ix < 40)
					{
						for(char j=0; j<8; j++)
							wp[j] = 0xaa;
						cp[ix] = 0xd1;
						ix++;
					}
					if (ix >= 2)
						cp[ix - 2] = 0x5d;
				}


				vic_waitFrame();

				joy_poll(0);
				keyb_poll();
				if (joyb[0] || (keyb_key & KSCAN_QUAL_DOWN))
				{
					vic.spr_enable = 0;
					vic.spr_multi = 0xff;
					for(char i=0; i<16; i++)					
						vspr_hide(i);
					return;
				}
			}
			it++;
			if (!*it)
				it = intro_text;
		}
	}
}
