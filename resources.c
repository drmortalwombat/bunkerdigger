#include "resources.h"
#include "tiles.h"
#include "digger.h"
#include "display.h"

signed char	res_oxygen[16];
char		res_stored[NUM_RESOURCES];

void res_generate(char di)
{
	char ti = diggers[di].target;
	char tile = BunkerMapData[ti];
	if (TileFlags[tile] & TF_ROOM)
	{
		RoomTile	rt = tile - 16;
		switch (rt)
		{
		case RTILE_HYDRO:
			res_stored[RES_WATER] += diggers[di].ability;
			break;
		case RTILE_GENERATOR:
			res_stored[RES_ENERGY] += diggers[di].ability;
			break;
		case RTILE_SICKBAY:
			res_stored[RES_HEALING] += diggers[di].intelligence;
			break;
		case RTILE_WORKBENCH:
			res_stored[RES_BUILDING] += diggers[di].ability;
			break;
		case RTILE_LABORATORY:
			res_stored[RES_RESEARCH] += diggers[di].intelligence;
			break;
		case RTILE_GYM:
			if (diggers[di].ability < DIGGER_MAX_SKILL)				
				diggers[di].ability++;
			break;
		case RTILE_ARMOURY:
			if (diggers[di].fight < DIGGER_MAX_SKILL)
				diggers[di].fight++;
			break;
		case RTILE_STUDY:
			if (diggers[di].intelligence < DIGGER_MAX_SKILL)
				diggers[di].intelligence++;
			break;
		case RTILE_EXCAVATOR:
			res_stored[RES_DIGGING] += diggers[di].ability;
			break;
		case RTILE_VENTILATION:
			{
				char y = diggers[di].ty;
				char a = diggers[di].ability;
				if (y > 0)
					res_oxygen[y-1] += a;
				res_oxygen[y] += 2 * a;
				if (y < 15)
					res_oxygen[y+1] += a;
			}
			break;
		case RTILE_MINE:
			switch(TileMapFlags[ti])
			{
			case GTYPE_METAL:
				res_stored[RES_METAL] += diggers[di].ability;
				break;
			case GTYPE_CARBON:
				res_stored[RES_CARBON] += diggers[di].ability;
				break;
			case GTYPE_URANIUM:
				res_stored[RES_URANIUM] += diggers[di].ability;
				break;
			}
			break;
		}
	}
}

char digger_heal;

void res_init(void)
{
	digger_heal = 0;
	for(char i=0; i<NUM_RESOURCES; i++)
	{
		res_stored[i] = 0;
		res_storage[i] = 32;
	}

	res_stored[RES_METAL] = 8;
}

void res_update(void)
{
	char	production = 4;

	res_oxygen[0] = DIGGER_MAX_AIR;
	res_oxygen[1] += 16;
	res_oxygen[2] += 8;
	res_oxygen[3] += 4;
	res_oxygen[4] += 2;
	res_oxygen[5] += 1;
	res_stored[RES_ENERGY] += 1;

	char heal = 0;

	for(char i=0; i<32; i++)
	{
		if (diggers[i].state != DS_FREE)
		{
			if (diggers[i].state == DS_WORKING)
			{
				if (diggers[i].count == 0)
				{
					res_generate(i);
					diggers[i].state = DS_IDLE;
				}
			}
			res_oxygen[diggers[i].ty]--;

			if (diggers[i].health < DIGGER_MAX_HEALTH)
				heal = 32;
		}
	}

	for(char i=0; i<32; i++)
	{
		if (diggers[i].state != DS_FREE && res_oxygen[diggers[i].ty] < 0 && diggers[i].health > 0)
		{
			diggers[i].health--;
			diggerchanged = true;
		}
	}

	for(char i=0; i<16; i++)
	{
		if (res_oxygen[i] < 0)
			res_oxygen[i] = 0;
		else if (res_oxygen[i] > DIGGER_MAX_AIR)
			res_oxygen[i] = DIGGER_MAX_AIR;
	}

	while (heal > 0 && res_stored[RES_HEALING] > 0)
	{
		if (diggers[digger_heal].state != DS_FREE && diggers[digger_heal].health < DIGGER_MAX_HEALTH)
		{
			res_stored[RES_HEALING]--;
			diggers[digger_heal].health++;
			heal = 32;
			diggerchanged = true;
		}
		else
			heal--;
		digger_heal = (digger_heal + 1) & 31;
	}

	for(char i=0; i<NUM_RESOURCES; i++)
	{
		if (res_stored[i] > res_storage[i])
			res_stored[i] = res_storage[i];
	}
}

const char resletters[9] = "EWDBMCUH";
const char rescolors[9] = {0xb7, 0xbe, 0xb8, 0xb5, 0xbf, 0xbc, 0xb4, 0xb1};

void res_display(void)
{
	for(char i=0; i<8; i++)
	{
		disp_char(24 + 8 * (i >> 2), 16 + (i & 3) , resletters[i], 0x00, 0xf1);
		disp_rbar(25 + 8 * (i >> 2), 16 + (i & 3), res_stored[i] >> 2, res_storage[i] >> 2, rescolors[i]);
	}
	disp_char(24, 20, 'A', 0x00, 0xf1);
	for(char i=0; i<15; i++)
		disp_vbar(25 + i, 20, (DIGGER_MAX_AIR - res_oxygen[i + 1] + 7) >> 3, 16 * VCOL_MED_GREY, VCOL_WHITE);
}