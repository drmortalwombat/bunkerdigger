#include "gamemenu.h"
#include "display.h"
#include <c64/keyboard.h>
#include "tiles.h"
#include "digger.h"
#include "resources.h"

GameMenu	gmenu;
char gmenux;
char cursorx, cursory;
char diggerx, diggery;
char diggeri;
bool diggerchanged;
char buildingi;
bool buildingchanged;

StatusView	statusview;

static const char * gamemenutexts[] = {
	S"_MAP", S"_TEM", S"_DIG", S"_BLD", S"_ASG", S"_GRD", S"_HIS", S"_LCH", S"_SAV", S"_OPT"
};

void gmenu_init(void)
{
	memset(Hires + 24 * 320, 0xff, 320);

	for(char x=0; x<10; x++)
	{
		disp_menu(4 * x, gamemenutexts[x], VCOL_BLUE, VCOL_WHITE | VCOL_LT_BLUE * 16, VCOL_YELLOW | VCOL_GREEN * 16);
	}

	disp_menu_color(4 * gmenux, gamemenutexts[gmenux], VCOL_RED, VCOL_WHITE | VCOL_YELLOW * 16, VCOL_YELLOW | VCOL_ORANGE * 16);
}


void gmenu_set(char x)
{
	disp_menu_color(4 * gmenux, gamemenutexts[gmenux], VCOL_BLUE, VCOL_WHITE | VCOL_LT_BLUE * 16, VCOL_YELLOW | VCOL_GREEN * 16);
	gmenux = x;
	disp_menu_color(4 * gmenux, gamemenutexts[gmenux], VCOL_RED, VCOL_WHITE | VCOL_YELLOW * 16, VCOL_YELLOW | VCOL_ORANGE * 16);
}


void gmenu_push(void)
{
	if (gmenu == GMENU_NONE)
		gmenu = (GameMenu)(gmenux + GMENU_MAP);
}

void gmenu_nav(signed char dx)
{
	if (dx < 0 && gmenux > 0)
		gmenu_set(gmenux - 1);
	else if (dx > 0 && gmenux < 9)
		gmenu_set(gmenux + 1);
}

void gmenu_key(char keyb)
{
	switch (keyb)
	{
	case KSCAN_M:
		gmenu_set(0);
		gmenu_push();
		break;
	case KSCAN_T:
		gmenu_set(1);
		gmenu_push();
		break;
	case KSCAN_D:
		gmenu_set(2);
		gmenu_push();
		break;
	case KSCAN_B:
		gmenu_set(3);
		gmenu_push();
		break;
	case KSCAN_A:
		gmenu_set(4);
		gmenu_push();
		break;
	case KSCAN_G:
		gmenu_set(5);
		gmenu_push();
		break;
	case KSCAN_H:
		gmenu_set(6);
		gmenu_push();
		break;
	case KSCAN_L:
		gmenu_set(7);
		gmenu_push();
		break;
	case KSCAN_S:
		gmenu_set(8);
		gmenu_push();
		break;
	case KSCAN_O:
		gmenu_set(9);
		gmenu_push();
		break;
	case KSCAN_CSR_RIGHT:
		gmenu_joy(1, 0);
		break;
	case KSCAN_CSR_RIGHT | KSCAN_QUAL_SHIFT:
		gmenu_joy(-1, 0);
		break;
	case KSCAN_CSR_DOWN:
		gmenu_joy(0, 1);
		break;
	case KSCAN_CSR_DOWN | KSCAN_QUAL_SHIFT:
		gmenu_joy(0, -1);
		break;
	case KSCAN_SPACE:
	case KSCAN_RETURN:
		gmenu_push();
		break;

	case KSCAN_R:
		researching = 0;
		break;

	case KSCAN_X:
		rooms_researched = 16;
		for(char i=0; i<NUM_RESOURCES; i++)
		{
			res_storage[i] = MAX_STORAGE;
			res_stored[i] = MAX_STORAGE;
		}
		break;
	}
}

void gmenu_joy(signed char dx, signed char dy)
{
	if (statusview == STVIEW_MINIMAP)
	{
		char	cx = cursorx, cy = cursory;
		if (dx < 0 && cx > 0)
			cx--;
		else if (dx > 0 && cx < 15)
			cx++;
		if (dy < 0 && cy > 0)
			cy--;
		else if (dy > 0 && cy < 15)
			cy++;

		if (tmapmode == TMMODE_REDRAW)
		{
			cursorx = cx;
			cursory = cy;
		}
		else
		{
			if (cx != cursorx || cy != cursory)
			{
				if (tmapmode == TMMODE_CURSOR)
				{
					tile_cursor_hide(cursorx - mapx, cursory - mapy);
					tmapmode = TMMODE_DRAWN;
				}

				cursorx = cx;
				cursory = cy;
			}

			if (mapx > cursorx)
				tmapx = cursorx;
			else if (mapx + 2 < cursorx)
				tmapx = cursorx - 2;

			if (mapy > cursory)
				tmapy = cursory;
			else if (mapy + 2 < cursory)
				tmapy = cursory - 2;

			if (tmapx != mapx || tmapy != mapy)
				tmapmode = TMMODE_REDRAW;
			else if (tmapmode == TMMODE_DRAWN)
			{
				tile_cursor_show(cursorx - mapx, cursory - mapy);
				tmapmode = TMMODE_CURSOR;
			}			
		}
	}
	else if (statusview == STVIEW_TEAM)
	{
		char di = diggeri;

		if (dx < 0)
		{
			if (di > 0)
			{
				do
				{
					di--;
				} while (diggers[di].state <= DS_DEAD && di > 0);
			}
		}
		else if (dx > 0)
		{
			if (di < 31)
			{
				do
				{
					di++;
				} while (diggers[di].state <= DS_DEAD && di < 31);				
			}
		}
		else if (dy < 0)
		{
			if (di > 1)
			{
				do
				{
					di-=2;
				} while (diggers[di].state <= DS_DEAD && di > 0);
			}
		}
		else if (dy > 0)
		{
			if (di < 30)
			{
				do
				{
					di+=2;
				} while (diggers[di].state <= DS_DEAD && di < 30);
			}
		}

		if (di != diggeri && diggers[di].state > DS_DEAD)
		{
			diggeri = di;
			diggerchanged = true;
		}
	}
	else if (statusview == STVIEW_BUILD)
	{
		char bi = buildingi;

		if (dy < 0)
		{
			if (bi > 0)
			{
//				do
//				{
					bi--;
//				} while (diggers[di].state == DS_FREE && di > 0);
			}
		}
		else if (dy > 0)
		{
			if (bi + 1 < rooms_researched)
			{
//				do
//				{
					bi++;
//				} while (diggers[di].state == DS_FREE && di < 30);
			}
		}

		if (bi != buildingi)
		{
			buildingi = bi;
			buildingchanged = true;
		}
	}
}