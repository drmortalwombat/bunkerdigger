#include "rooms.h"
#include "display.h"
#include "tiles.h"
#include "resources.h"
#include "messages.h"
#include "window.h"

const RoomInfo		room_infos[16] = {
	{1, 0, 0},
	{4, 0, 0},
	{1, 0, 0},
	{2, 0, 0},

	{1, 0, 0},
	{3, 0, 0},
	{4, 1, 0},
	{4, 0, 0},

	{2, 3, 0},
	{4, 6, 0},
	{2, 3, 0},
	{1, 4, 0},

	{4, 4, 0},
	{7, 4, 0},
	{2, 9, 0},	
	{1, 3, 2},
};


const char room_names[16 * 10 + 1] = 
	"QUARTERS  "
	"WORKBENCH "
	"HYDRO     "
	"GENERATOR "

	"MINE      "
	"STORAGE   "
	"LABORATORY"
	"VENTILATOR"

	"GYM       "
	"ARMOURY   "
	"SICKBAY   "
	"RADIO     "

	"STUDY     "
	"EXCAVATOR "
	"CENTRIFUGE"
	"LAUNCH    ";

#pragma align(room_names, 256)

__striped Construction		room_constructions[4];
char						room_num_construction;

void rooms_count(void)
{
	for(char i=0; i<21; i++)
		room_count[i] = 0;
	for(int i=0; i<256; i++)
	{
		char tile = BunkerMapData[i];
		if (TileFlags[tile] & TF_ROOM)
		{
			RoomTile	rt = tile - TILE_ROOMS;
			room_count[rt]++;
		}
	}

	char	nstore = room_count[RTILE_STORAGE];

	res_storage[RES_ENERGY] = 12 + 8 * room_count[RTILE_GENERATOR];

	res_storage[RES_BUILDING] = 4 * room_count[RTILE_WORKBENCH];
	res_storage[RES_HEALING] = 4 * room_count[RTILE_SICKBAY];
	res_storage[RES_WATER] = 10 + 6 * room_count[RTILE_HYDRO];

	res_storage[RES_METAL] = 8 + 16 * nstore;
	res_storage[RES_CARBON] = 4 + 8 * nstore;
	res_storage[RES_URANIUM] = 2 * nstore;

	res_storage[RES_FUEL] = 
		(room_count[RTILE_MISSILE_TOP] + 
		room_count[RTILE_MISSILE_MID] +
		room_count[RTILE_MISSILE_BOTTOM]) * 16;

	if (room_count[RTILE_LAUNCH_TOP] > 0 &&
		room_count[RTILE_LAUNCH_MID] > 0 &&
		room_count[RTILE_LAUNCH_BOTTOM] > 0)
		story_pending |= 1 << SIM_ROCKET_BUILD;


	for(char i=0; i<NUM_RESOURCES; i++)
	{
		char s = (res_storage[i] + 15) & ~15;
		if (s > MAX_STORAGE)
			s = MAX_STORAGE;
		res_storage[i] = s;
	}
}

void rooms_display(void)
{
	for(char i=0; i<rooms_blueprints; i++)
	{
		char cnt = room_count[i];

		char c = VCOL_BLACK;
		char fc = VCOL_WHITE + 16 * VCOL_LT_GREY;

		if (i >= rooms_researched)
		{
			fc = VCOL_LT_BLUE + 16 * VCOL_BLUE;
			if ( i == buildingi)
				c = VCOL_BLUE;
		}
		else if (res_stored[RES_METAL] < 4 * room_infos[i].res_metal ||
			res_stored[RES_CARBON] < 4 * room_infos[i].res_carbon ||
			res_stored[RES_URANIUM] < 4 * room_infos[i].res_uranium)
		{
			fc = VCOL_ORANGE + 16 * VCOL_RED;
			if ( i == buildingi)
				c = VCOL_RED;
		}
		else if (i == buildingi)
			c = VCOL_MED_GREY;

		char t = '0';
		while (cnt >= 10)
		{
			t++;
			cnt -= 10;
		}
		if (t != '0')
			disp_char(24, i, t, c, 0xbf);
		else			
			disp_char(24, i, ' ', c, 0xbf);

		disp_char(25, i, '0' + cnt, c, 0xbf);

		disp_char(26, i, '*', c, 0xbf);


		disp_chars(27, i, room_names + 10 * i, 10, c, fc);
		disp_char(37, i, '0' + room_infos[i].res_metal, VCOL_BLACK, 0xbf);
		disp_char(38, i, '0' + room_infos[i].res_carbon, VCOL_BLACK, 0xbc);
		disp_char(39, i, '0' + room_infos[i].res_uranium, VCOL_BLACK, 0x64);
	}

	for(char i=rooms_blueprints; i<16; i++)
		disp_space(24, i, 16, VCOL_BLACK, VCOL_BLACK);
}

bool rooms_build(void)
{
	if (room_num_construction < 4 &&
		res_stored[RES_METAL] >= 4 * room_infos[buildingi].res_metal &&
		res_stored[RES_CARBON] >= 4 * room_infos[buildingi].res_carbon &&
		res_stored[RES_URANIUM] >= 4 * room_infos[buildingi].res_uranium)
	{
		char	ti = cursorx + 16 * cursory;

		char 	tile = BunkerMapData[ti];
#if 0
		if (tile >= TILE_ROOMS && tile < TILE_GROUND)
		{
			tile = TILE_FLOOR;
		}
#endif
		if (tile == TILE_FLOOR)
		{
			res_stored[RES_METAL] -= 4 * room_infos[buildingi].res_metal;
			res_stored[RES_CARBON] -= 4 * room_infos[buildingi].res_carbon;
			res_stored[RES_URANIUM] -= 4 * room_infos[buildingi].res_uranium;

			BunkerMapData[ti] = RTILE_CONSTRUCTION + TILE_ROOMS;

			room_constructions[room_num_construction].tile = ti;
			room_constructions[room_num_construction].count = 
			room_constructions[room_num_construction].step = 
				1 + 
				1 * room_infos[buildingi].res_metal + 
				2 * room_infos[buildingi].res_carbon + 
				4 * room_infos[buildingi].res_uranium;
			room_constructions[room_num_construction].progress = 0;
			room_constructions[room_num_construction].room = buildingi;
			room_num_construction++;

			buildingchanged = true;
			return true;
		}

	}

	return false;
}

char rooms_find_rocket(char n)
{
	n *= 16;
	for(char i=0; i<16; i++)
	{
		if (BunkerMapData[i] == TILE_ROOMS + RTILE_MISSILE_TOP)
		{
			char j = i + 16;
			while (BunkerMapData[j] == TILE_ROOMS + RTILE_MISSILE_MID)
				j += 16;
			if (j >= n && BunkerMapData[j] == TILE_ROOMS + RTILE_MISSILE_BOTTOM)
				return i;
		}
	}

	return 0xff;
}

bool rooms_launch()
{
	if (!room_count[RTILE_MISSILE_TOP] ||
		!room_count[RTILE_MISSILE_MID] ||
		!room_count[RTILE_MISSILE_BOTTOM])
		return false;

	char ri = rooms_find_rocket(1);
	if (ri == 0xff)
		return false;

	if (res_stored[RES_FUEL] < 48)
		return false;

	BunkerMapData[ri] = TILE_ROOMS + RTILE_LAUNCH_TOP;
	char j = ri + 16;
	while (BunkerMapData[j] == TILE_ROOMS + RTILE_MISSILE_MID)
	{
		BunkerMapData[j] = TILE_ROOMS + RTILE_LAUNCH_MID;
		j += 16;
	}
	BunkerMapData[j] = TILE_ROOMS + RTILE_LAUNCH_BOTTOM;

	res_stored[RES_FUEL] -= 48;

	return true;
}


bool rooms_check_construction(void)
{
	char i = 0, j = 0;
	while (i < room_num_construction)
	{		
		bool	done = false;

		if (res_stored[RES_BUILDING] && !--room_constructions[i].count)
		{
			res_stored[RES_BUILDING]--;
			room_constructions[i].progress++;
			room_constructions[i].count = room_constructions[i].step;
			if (room_constructions[i].progress == 8)
				done = true;			
		}

		char x = room_constructions[i].tile & 0x0f;
		char y = room_constructions[i].tile >> 4;

		if ((char)(x - mapx) < 3 && (char)(y - mapy) < 3)
			disp_bbar(8 * (x - mapx) + 2, 8 * (y - mapy) + 4, room_constructions[i].progress);

		if (done)
		{
			char r = room_constructions[i].room;
			char t = room_constructions[i].tile;

			msg_queue(MSG_ROOM_COMPLETED, r);

			if (r == RTILE_LAUNCH_TOP)
			{
				if (t >= 16)
				{
					if (t >= 240 || BunkerMapData[t + 16] < RTILE_LAUNCH_TOP + TILE_ROOMS || BunkerMapData[t + 16] > RTILE_MISSILE_BOTTOM + TILE_ROOMS)
						r += 2;
					else
						r++;

					if (BunkerMapData[t - 16] == RTILE_LAUNCH_BOTTOM + TILE_ROOMS)
						BunkerMapData[t - 16] = RTILE_LAUNCH_MID + TILE_ROOMS;
				}
			}

			BunkerMapData[t] = r + TILE_ROOMS;

			room_count[r]++;

			buildingchanged = true;			
		}
		else
		{
			if (i != j)
				room_constructions[j] = room_constructions[i];
			j++;
		}

		i++;
	}

	room_num_construction = j;

	return i != j;
}
