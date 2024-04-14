#include "resources.h"
#include "tiles.h"
#include "digger.h"
#include "display.h"
#include "messages.h"
#include "rooms.h"

signed char	res_oxygen[16];
char		res_stored[NUM_RESOURCES];
bool		time_changed;

void res_generate(char di)
{
	char ti = diggers[di].target;
	char tile = BunkerMapData[ti];
	if (TileFlags[tile] & TF_ROOM)
	{
		RoomTile	rt = tile - TILE_ROOMS;
		switch (rt)
		{
		case RTILE_HYDRO:
			res_stored[RES_WATER] += diggers[di].ability;
			break;
		case RTILE_GENERATOR:
			res_stored[RES_ENERGY] += (1 + diggers[di].ability) >> 1;
			break;
		case RTILE_SICKBAY:
			res_stored[RES_HEALING] += diggers[di].intelligence;
			break;
		case RTILE_WORKBENCH:
			res_stored[RES_BUILDING] += diggers[di].ability;
			break;
		case RTILE_LABORATORY:
			if (rooms_researched < rooms_blueprints)
			{
				if (researching > diggers[di].intelligence)
					researching -= diggers[di].intelligence;
				else
					researching = 0;
			}
			break;
		case RTILE_GYM:
			if (diggers[di].ability < DIGGER_MAX_SKILL)
			{		
				diggers[di].ability++;
				digger_check_color(di);
			}
			break;
		case RTILE_ARMOURY:
			if (diggers[di].fight < DIGGER_MAX_SKILL)
			{
				diggers[di].fight++;
				digger_check_color(di);
			}
			break;
		case RTILE_STUDY:
			if (diggers[di].intelligence < DIGGER_MAX_SKILL)
			{				
				diggers[di].intelligence++;
				digger_check_color(di);
			}
			break;
		case RTILE_CENTRIFUGE:
			res_stored[RES_FUEL] += (diggers[di].ability + 7) >> 3;
			break;
		case RTILE_VENTILATION:
			{
				char y = diggers[di].ty;
				char a = diggers[di].ability + 3;
				if (y > 0)
					res_oxygen[y-1] += a;
				res_oxygen[y] += 2 * a;
				if (y < 15)
					res_oxygen[y+1] += a;
			}
			break;
		case RTILE_RADIO:
			radio_count += (diggers[di].ability + 3) >> 2;
			if (radio_count > 128)
			{
				radio_count -= 128;
				radio_days++;
				digger_procreate(true);
				if (rooms_blueprints < RTILE_LAUNCH_TOP + 1 && radio_days > 4 + (3 << (rooms_blueprints - RTILE_RADIO)))
				{
					msg_queue(MSG_BLUEPRINT, rooms_blueprints);
					rooms_blueprints++;
				}
			}
			break;
		case RTILE_MINE:
			switch(TileMapFlags[ti] & GROUND_TYPE_MASK)
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
			default:
				return;
			}

			if (TileMapFlags[ti] < 0x08)
			{
				msg_queue(MSG_MINE_DEPLETED, TileMapFlags[ti] & GROUND_TYPE_MASK);
				TileMapFlags[ti] = GTYPE_SOIL;
				tmapmode = TMMODE_REDRAW;
			}
			else
				TileMapFlags[ti] -= 0x08;			
			break;

		case RTILE_LAUNCH_TOP:
		case RTILE_LAUNCH_MID:
		case RTILE_LAUNCH_BOTTOM:
			BunkerMapData[ti] += 3;
			buildingchanged = true;
			break;

		}
	}
}

char digger_heal;
char digger_water_index;
char res_update_cnt;
char digger_shuffle_mask;
char time_count, time_days;

void res_init(void)
{
	digger_heal = 0;
	digger_water_index = 0;
	for(char i=0; i<NUM_RESOURCES; i++)
		res_stored[i] = 0;
	time_count = 0;
	time_days = 1;
	time_changed = true;
	radio_count = 0;
	radio_days = 0;
}

void res_update(void)
{
	char	production = 4;

	res_oxygen[0] = DIGGER_MAX_AIR;
	res_oxygen[1] += 8;
	res_oxygen[2] += 4;
	res_oxygen[3] += 2;
	res_oxygen[4] += 1;

	if (!(res_update_cnt & 15))
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
					if (diggers[i].task == DTASK_WORK)
						res_generate(i);
					diggers[i].state = DS_IDLE;
				}
			}
			res_oxygen[diggers[i].ty]--;

			if (diggers[i].health < DIGGER_MAX_HEALTH)
				heal = 32;
		}
	}

	res_update_cnt++;
	if (res_stored[RES_METAL] < 4 && (res_update_cnt & 0x7f) == 0x29)
		res_stored[RES_METAL]++;
	if (res_stored[RES_HEALING] == 0 && (res_update_cnt & 0x3f) == 0x04)
		res_stored[RES_HEALING] = 1;

	if (!(res_update_cnt & 15))
	{
		// Shuffle diggers a little so the dehydration is more
		// evenly distributed
		digger_shuffle_mask = rand() & 31;
	}

	if (!(res_update_cnt & 1))
	{
		// time counter
		time_count++;
		if (time_count == 225)
		{
			time_count = 0;
			time_days++;
			time_changed = true;
		}
	}

	for(char i=0; i<2; i++)
	{
		char j = digger_water_index ^ digger_shuffle_mask;
		if (diggers[j].state != DS_FREE)
		{
			if (res_stored[RES_WATER] > 0)
				res_stored[RES_WATER]--;
			else if (diggers[j].health > 0)
			{
				diggers[j].health--;			
				diggerchanged = true;
			}
		}
		digger_water_index = (digger_water_index + 1) & 31;
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

const char resletters[9] = "EWBHMCUF";
const char rescolors[9] = {0xb7, 0xbe, 0xb5, 0xb1, 0xbf, 0xb0, 0xb4, 0xb8};

void res_display(void)
{
	for(char i=0; i<8; i++)
	{
		disp_char(24 + 8 * (i >> 2), 16 + (i & 3) , resletters[i], 0x00, 0xf1);
		disp_rbar(25 + 8 * (i >> 2), 16 + (i & 3), res_stored[i] >> 2, res_storage[i] >> 2, MAX_STORAGE >> 2, rescolors[i]);
	}
	disp_char(24, 20, 'X', 0x00, 0xf1);
	for(char i=0; i<15; i++)
		disp_vbar(25 + i, 20, (DIGGER_MAX_AIR - res_oxygen[i + 1] + 7) >> 3, 16 * VCOL_MED_GREY, VCOL_WHITE);

	if (time_changed)
	{
		disp_text(34, 23, "DAY ", VCOL_BLACK, VCOL_WHITE | (VCOL_MED_GREY << 4));
		char d = time_days;
		char t = d / 10; d %= 10;
		disp_char(38, 23, '0' + t , VCOL_BLACK, VCOL_WHITE | (VCOL_MED_GREY << 4));
		disp_char(39, 23, '0' + d , VCOL_BLACK, VCOL_WHITE | (VCOL_MED_GREY << 4));
	}
}
