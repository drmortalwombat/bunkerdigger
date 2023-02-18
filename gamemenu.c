#include "gamemenu.h"
#include "display.h"
#include <c64/keyboard.h>

GameMenu	gmenu;
char gmenux, gmenuy;

static const char * gamemenutexts[] = {
	S"_MAP ", S"_TEAM", S"_RSRC", S"_DIG ", S"_BULD", S"_ASGN"
};

void gmenu_init(void)
{
	for(char y=0; y<2; y++)
	{
		for(char x=0; x<3; x++)
		{
			disp_menu(25 + 5 * x, 17 + y, gamemenutexts[3 * y + x], VCOL_BLUE, VCOL_WHITE * 16 | VCOL_LT_BLUE, VCOL_YELLOW * 16 | VCOL_GREEN);
		}
	}
}


void gmenu_set(char x, char y)
{
	disp_menu_color(25 + 5 * gmenux, 17 + gmenuy, gamemenutexts[3 * gmenuy + gmenux], VCOL_BLUE, VCOL_WHITE * 16 | VCOL_LT_BLUE, VCOL_YELLOW * 16 | VCOL_GREEN);
	gmenux = x;
	gmenuy = y;
	disp_menu_color(25 + 5 * gmenux, 17 + gmenuy, gamemenutexts[3 * gmenuy + gmenux], VCOL_RED, VCOL_WHITE * 16 | VCOL_YELLOW, VCOL_YELLOW * 16 | VCOL_ORANGE);
}

void gmenu_key(char keyb)
{
	switch (keyb)
	{
	case KSCAN_M:
		gmenu_set(0, 0);
		break;
	case KSCAN_T:
		gmenu_set(1, 0);
		break;
	case KSCAN_R:
		gmenu_set(2, 0);
		break;
	case KSCAN_D:
		gmenu_set(0, 1);
		break;
	case KSCAN_B:
		gmenu_set(1, 1);
		break;
	case KSCAN_A:
		gmenu_set(2, 1);
		break;
	}
}
