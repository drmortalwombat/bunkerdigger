#include "gamemenu.h"
#include "display.h"
#include <c64/keyboard.h>
#include "tiles.h"

GameMenu	gmenu;
char gmenux;

static const char * gamemenutexts[] = {
	S"_MAP ", S"_TEAM", S"_RSRC", S"_DIG ", S"_BULD", S"_ASGN"
};

void gmenu_init(void)
{
	memset(Hires + 24 * 320, 0xff, 320);

	for(char x=0; x<6; x++)
	{
		disp_menu(5 * x, gamemenutexts[x], VCOL_BLUE, VCOL_WHITE | VCOL_LT_BLUE * 16, VCOL_YELLOW | VCOL_GREEN * 16);
	}
}


void gmenu_set(char x)
{
	disp_menu_color(5 * gmenux, gamemenutexts[gmenux], VCOL_BLUE, VCOL_WHITE | VCOL_LT_BLUE * 16, VCOL_YELLOW | VCOL_GREEN * 16);
	gmenux = x;
	disp_menu_color(5 * gmenux, gamemenutexts[gmenux], VCOL_RED, VCOL_WHITE | VCOL_YELLOW * 16, VCOL_YELLOW | VCOL_ORANGE * 16);
}

void gmenu_key(char keyb)
{
	switch (keyb)
	{
	case KSCAN_M:
		gmenu_set(0);
		break;
	case KSCAN_T:
		gmenu_set(1);
		break;
	case KSCAN_R:
		gmenu_set(2);
		break;
	case KSCAN_D:
		gmenu_set(3);
		break;
	case KSCAN_B:
		gmenu_set(4);
		break;
	case KSCAN_A:
		gmenu_set(5);
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
	}
}

char cursorx, cursory;

void gmenu_joy(signed char dx, signed char dy)
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
				tile_cursor(cursorx - mapx, cursory - mapy);
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
			tile_cursor(cursorx - mapx, cursory - mapy);
			tmapmode = TMMODE_CURSOR;
		}
	}
}