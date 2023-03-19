#include "rooms.h"
#include "display.h"
#include "tiles.h"
#include "resources.h"

const RoomInfo		room_infos[16] = {
	{1, 0, 0},
	{1, 0, 0},
	{2, 0, 0},
	{1, 0, 0},

	{2, 0, 0},
	{3, 0, 0},
	{1, 1, 0},
	{2, 4, 0},

	{2, 3, 0},
	{4, 1, 0},
	{2, 3, 0},
	{1, 4, 0},

	{7, 4, 0},
	{4, 0, 0},
	{2, 5, 0},	
	{1, 3, 2},
};


const char room_names[16 * 10 + 1] = 
	"QUARTERS  "
	"HYDRO     "
	"MINE      "
	"VENTILATOR"

	"GENERATOR "
	"WORKBENCH "
	"LABORATORY"
	"CENTRIFUGE"

	"GYM       "
	"ARMOURY   "
	"SICKBAY   "
	"STUDY     "

	"EXCAVATOR "	
	"STORAGE   "
	"RADIO     "
	"LAUNCH    ";

Construction			room_constructions[4];
char					room_num_construction;

void rooms_count(void)
{
	for(char i=0; i<16; i++)
		room_count[i] = 0;
	for(int i=0; i<256; i++)
	{
		char tile = BunkerMapData[i];
		if (TileFlags[tile] & TF_ROOM)
		{
			RoomTile	rt = tile - 16;
			if (rt > RTILE_LAUNCH_TOP)
				rt = RTILE_LAUNCH_TOP;
			room_count[rt]++;
		}
	}

	char	nstore = room_count[RTILE_STORAGE];

	res_storage[RES_ENERGY] = 8 + 8 * room_count[RTILE_GENERATOR];

	res_storage[RES_BUILDING] = 4 * room_count[RTILE_WORKBENCH];
	res_storage[RES_HEALING] = 4 * room_count[RTILE_SICKBAY];
	res_storage[RES_WATER] = 8 + 4 * room_count[RTILE_HYDRO];

	res_storage[RES_METAL] = 8 + 16 * nstore;
	res_storage[RES_CARBON] = 4 + 8 * nstore;
	res_storage[RES_URANIUM] = 2 * nstore;

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
	for(char i=0; i<16; i++)
	{
		char cnt = room_count[i];

		char c = VCOL_BLACK;
		char fc = VCOL_WHITE + 16 * VCOL_LT_GREY;

		if (res_stored[RES_METAL] < room_infos[i].res_metal ||
			res_stored[RES_CARBON] < room_infos[i].res_carbon ||
			res_stored[RES_URANIUM] < room_infos[i].res_uranium)
		{
			fc = VCOL_LT_BLUE + 16 * VCOL_BLUE;
			if ( i == buildingi)
				c = VCOL_BLUE;
		}
		else if (i == buildingi)
			c = VCOL_MED_GREY;

		if (cnt >= 10)
		{
			disp_char(24, i, '1', 0x00, 0xbf);
			cnt -= 10;
		}
		else			
			disp_char(24, i, ' ', c, 0xbf);
		disp_char(25, i, '0' + cnt, c, 0xbf);

		disp_char(26, i, '*', c, 0xbf);


		disp_chars(27, i, room_names + 10 * i, 10, c, fc);
		disp_char(37, i, '0' + room_infos[i].res_metal, VCOL_BLACK, 0xbf);
		disp_char(38, i, '0' + room_infos[i].res_carbon, VCOL_BLACK, 0xbc);
		disp_char(39, i, '0' + room_infos[i].res_uranium, VCOL_BLACK, 0x64);
	}
}

bool rooms_build(void)
{
	if (room_num_construction < 4 &&
		res_stored[RES_METAL] >= room_infos[buildingi].res_metal &&
		res_stored[RES_CARBON] >= room_infos[buildingi].res_carbon &&
		res_stored[RES_URANIUM] >= room_infos[buildingi].res_uranium)
	{
		char	ti = cursorx + 16 * cursory;

		char 	tile = BunkerMapData[ti];
		if (tile == TILE_FLOOR)
		{
			res_stored[RES_METAL] -= room_infos[buildingi].res_metal;
			res_stored[RES_CARBON] -= room_infos[buildingi].res_carbon;
			res_stored[RES_URANIUM] -= room_infos[buildingi].res_uranium;

			BunkerMapData[ti] = RTILE_CONSTRUCTION + 16;

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
			BunkerMapData[room_constructions[i].tile] = room_constructions[i].room + 16;

			room_count[room_constructions[i].room]++;

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