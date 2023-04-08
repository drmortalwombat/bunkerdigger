#include "gamemenu.h"
#include "display.h"
#include <c64/keyboard.h>
#include "tiles.h"
#include "digger.h"

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
	S"_MAP ", S"_TEAM", S"_DIG ", S"_BULD", S"_ASGN", S"_GARD"
};

void gmenu_init(void)
{
	memset(Hires + 24 * 320, 0xff, 320);

	for(char x=0; x<6; x++)
	{
		disp_menu(5 * x, gamemenutexts[x], VCOL_BLUE, VCOL_WHITE | VCOL_LT_BLUE * 16, VCOL_YELLOW | VCOL_GREEN * 16);
	}

	disp_menu_color(5 * gmenux, gamemenutexts[gmenux], VCOL_RED, VCOL_WHITE | VCOL_YELLOW * 16, VCOL_YELLOW | VCOL_ORANGE * 16);
}


void gmenu_set(char x)
{
	disp_menu_color(5 * gmenux, gamemenutexts[gmenux], VCOL_BLUE, VCOL_WHITE | VCOL_LT_BLUE * 16, VCOL_YELLOW | VCOL_GREEN * 16);
	gmenux = x;
	disp_menu_color(5 * gmenux, gamemenutexts[gmenux], VCOL_RED, VCOL_WHITE | VCOL_YELLOW * 16, VCOL_YELLOW | VCOL_ORANGE * 16);
}


void gmenu_push(void)
{
	if (gmenu == GMENU_NONE)
	{
		switch (gmenux)
		{
		case 0:
			gmenu = GMENU_MAP;
			break;
		case 1:
			gmenu = GMENU_TEAM;
			break;
		case 2:
			gmenu = GMENU_DIG;
			break;
		case 3:
			gmenu = GMENU_BUILD;
			break;
		case 4:
			gmenu = GMENU_ASSIGN;
			break;
		case 5:
			gmenu = GMENU_GUARD;
			break;
		}
	}
}

void gmenu_nav(signed char dx)
{
	if (dx < 0 && gmenux > 0)
		gmenu_set(gmenux - 1);
	else if (dx > 0 && gmenux < 5)
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
	}
}

void gmenu_joy(signed char dx, signed char dy)
{
	if (statusview == STVIEW_MINIMAP)
	{
		if (tmapmode != TMMODE_REDRAW)
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

			if (cx != cursorx || cy != cursory)
			{
				if (tmapmode == TMMODE_CURSOR)
				{
					tile_cursor_hide(cursorx - mapx, cursory - mapy);
					tmapmode = TMMODE_DRAWN;
				}

				cursorx = cx;
				cursory = cy;

				if (mapx > cursorx)
					tmapx = cursorx;
				else if (mapx + 2 < cursorx)
					tmapx = cursorx - 2;

				if (mapy > cursory)
					tmapy = cursory;
				else if (mapy + 2 < cursory)
					tmapy = cursory - 2;
			}

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
				} while (diggers[di].state == DS_FREE && di > 0);
			}
		}
		else if (dx > 0)
		{
			if (di < 31)
			{
				do
				{
					di++;
				} while (diggers[di].state == DS_FREE && di < 31);				
			}
		}
		else if (dy < 0)
		{
			if (di > 1)
			{
				do
				{
					di-=2;
				} while (diggers[di].state == DS_FREE && di > 0);
			}
		}
		else if (dy > 0)
		{
			if (di < 30)
			{
				do
				{
					di+=2;
				} while (diggers[di].state == DS_FREE && di < 30);
			}
		}

		if (di != diggeri && diggers[di].state != DS_FREE)
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