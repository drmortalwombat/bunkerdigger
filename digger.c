#include "digger.h"
#include "tiles.h"
#include "resources.h"
#include <c64/sprites.h>

extern __striped Digger	diggers[32];

static const char digger_task_char[] = {
	'W', 'M', 'G', 'I', 'D'
};

char diggers_born;

const char digger_names[] = 
	"MIKE JIM  JOE  CARL CLIFFMARK BILL ANDY "
	"ALEX FRED CHRISBOB  TIM  TOM  JAMESSIMON"
	"JILL JANE LISA MONA TILLYSUSANMARTHBETH "
	"LINDAANNA MARIAJUDY MINA OLGA TONIAKIM  ";


void diggers_init(void)
{
	for(char i=0; i<32; i++)
		diggers[i].state = DS_FREE;

	diggers[0].tx = 8;
	diggers[0].ty = 0; 
	diggers[0].sy = 8;
	diggers[0].sx = 8;
	diggers[0].mi = 64 + 1;
	diggers[0].color = VCOL_LT_GREY;
	diggers[0].state = DS_IDLE;
	diggers[0].task = DTASK_IDLE;
	diggers[0].target = 0;
	diggers[0].ability = 1;
	diggers[0].fight = 1;
	diggers[0].intelligence = 1;
	diggers[0].health = DIGGER_MAX_HEALTH;

	digger_check_color(0);

	diggers_born = 1;

#if 0
	diggers[1].tx = 8;
	diggers[1].ty = 6; 
	diggers[1].sy = 8;
	diggers[1].sx = 8;
	diggers[1].mi = 64 + 1;
	diggers[1].color = VCOL_YELLOW;
	diggers[1].state = DS_IDLE;
	diggers[1].task = DTASK_MOVE;
	diggers[1].target = 0x4a;
	diggers[1].ability = 1;
	diggers[1].fight = 1;
	diggers[1].intelligence = 1;
	diggers[1].health = DIGGER_MAX_HEALTH;

	diggers[2].tx = 0;
	diggers[2].ty = 4; 
	diggers[2].sy = 8;
	diggers[2].sx = 8;
	diggers[2].mi = 64 + 1;
	diggers[2].color = VCOL_BLUE;
	diggers[2].state = DS_IDLE;
	diggers[2].task = DTASK_MOVE;
	diggers[2].target = 0x4a;
	diggers[2].ability = 1;
	diggers[2].fight = 1;
	diggers[2].intelligence = 1;
	diggers[2].health = DIGGER_MAX_HEALTH;
#endif
}

void digger_check_color(char di)
{
	char color = VCOL_LT_GREY;

	if (diggers[di].intelligence > diggers[di].ability)
	{
		if (diggers[di].intelligence > diggers[di].fight)
		{
			if (diggers[di].intelligence > 8)
				color = VCOL_PURPLE;
			else if (diggers[di].intelligence > 1)
				color = VCOL_BLUE;
		}
		else
		{
			if (diggers[di].fight > 8)
				color = VCOL_LT_RED;
			else if (diggers[di].fight > 1)
				color = VCOL_RED;
		}
	}
	else if (diggers[di].ability > diggers[di].fight)
	{
		if (diggers[di].ability > 8)
			color = VCOL_YELLOW;
		else if (diggers[di].ability > 1)
			color = VCOL_GREEN;
	}
	else
	{
		if (diggers[di].fight > 8)
			color = VCOL_LT_RED;
		else if (diggers[di].fight > 1)
			color = VCOL_RED;
	}

	diggers[di].color = color;
}


void diggers_move(void)
{
	for(char i=0; i<32; i++)
	{	
		switch(diggers[i].state)
		{
		case DS_MOVE_RIGHT:
			if (diggers[i].sx == 15)
			{
				diggers[i].tx++;
				diggers[i].sx = 0;
			}
			else
				diggers[i].sx++;

			if (!--diggers[i].count)
			{
				diggers[i].state = DS_IDLE;
				diggers[i].mi = 65;
			}
			else
			{
				diggers[i].mi++;
				if (diggers[i].mi == 64 + 17)
					diggers[i].mi = 64 + 11;				
			}
			break;
		case DS_MOVE_LEFT:
			if (diggers[i].sx == 0)
			{
				diggers[i].tx--;
				diggers[i].sx = 15;
			}
			else
				diggers[i].sx--;

			if (!--diggers[i].count)
			{
				diggers[i].state = DS_IDLE;
				diggers[i].mi = 65;
			}
			else
			{
				diggers[i].mi++;
				if (diggers[i].mi == 64 + 9)
					diggers[i].mi = 64 + 3;				
			}
			break;		
		case DS_CLIMB_DOWN:
			if (diggers[i].sy == 15)
			{
				diggers[i].ty++;
				diggers[i].sy = 0;
			}
			else
				diggers[i].sy++;

			if (!--diggers[i].count)
			{
				diggers[i].state = DS_IDLE;
				diggers[i].mi = 65;
			}
			else
			{
				diggers[i].mi++;
				if (diggers[i].mi == 64 + 24)
					diggers[i].mi = 64 + 18;				
			}
			break;
		case DS_CLIMB_UP:
			if (diggers[i].sy == 0)
			{
				diggers[i].ty--;
				diggers[i].sy = 15;
			}
			else
				diggers[i].sy--;

			if (!--diggers[i].count)
			{
				diggers[i].state = DS_IDLE;
				diggers[i].mi = 65;
			}
			else
			{
				diggers[i].mi++;
				if (diggers[i].mi == 64 + 24)
					diggers[i].mi = 64 + 18;				
			}
			break;		
		case DS_WORKING:
			if (diggers[i].task == DTASK_WORK)
			{
				diggers[i].mi = 64;
				if (!(irqcount & 3) && diggers[i].count)
					diggers[i].count--;
			}
			else
				diggers[i].count = 0;
			break;
		case DS_DEAD:
			diggers[i].mi = 88;
			if (!(irqcount & 3) && diggers[i].count)
				diggers[i].count--;
			break;
		case DS_IDLE:
			diggers[i].mi = 65;
			break;
		}
	}
}

void digger_left(char di, char n)
{
	diggers[di].count = 16 * n;
	diggers[di].mi = 64 + 3;
	diggers[di].state = DS_MOVE_LEFT;
}

void digger_right(char di, char n)
{
	diggers[di].count = 16 * n;
	diggers[di].mi = 64 + 11;
	diggers[di].state = DS_MOVE_RIGHT;
}

void digger_up(char di, char n)
{
	diggers[di].count = 16 * n;
	diggers[di].mi = 64 + 18;
	diggers[di].state = DS_CLIMB_UP;
}

void digger_down(char di, char n)
{
	diggers[di].count = 16 * n;
	diggers[di].mi = 64 + 18;
	diggers[di].state = DS_CLIMB_DOWN;
}

void digger_move_to(char di, char mi)
{
	char ti = diggers[di].ty * 16 + diggers[di].tx;
	char tf = TileFlags[BunkerMapData[ti]];

	char tx = mi & 0x0f;
	char ty = mi >> 4;

	if (tx < diggers[di].tx && (tf & TF_LEFT))
		digger_left(di, diggers[di].tx - tx);
	else if (tx > diggers[di].tx && (tf & TF_RIGHT))
		digger_right(di, tx - diggers[di].tx);
	else if (ty < diggers[di].ty && (tf & TF_UP))
		digger_up(di, diggers[di].ty - ty);
	else if (ty > diggers[di].ty && (tf & TF_DOWN))
		digger_down(di, ty - diggers[di].ty);	
}

void digger_move_random(char di)
{
	char ti = diggers[di].ty * 16 + diggers[di].tx;
	char tf = TileFlags[BunkerMapData[ti]];

	switch (rand() & 15)
	{
	case 0:
		if (diggers[di].tx > 0 && (tf & TF_LEFT))
			digger_left(di, 1);
		break;
	case 1:
		if (diggers[di].tx < 15 && (tf & TF_RIGHT))
			digger_right(di, 1);
		break;
	case 2:
		if (diggers[di].ty > 0 && (tf & TF_UP))
			digger_up(di, 1);
		break;
	case 3:
		if (diggers[di].ty < 15 && (tf & TF_DOWN))
			digger_down(di, 1);
		break;
	}	
}

void digger_decide(char di)
{
	char ti = diggers[di].ty * 16 + diggers[di].tx;

	switch (diggers[di].task)
	{
	case DTASK_MOVE:
		if (ti == diggers[di].target)
		{
			diggers[di].task = DTASK_WORK;
			diggerchanged = true;
		}
		else
		{
			char mi = tile_plan(ti, diggers[di].target);
			if (mi == ti)
			{
				diggers[di].task = DTASK_IDLE;
				diggerchanged = true;
			}
			else
				digger_move_to(di, mi);
		}
		break;
	case DTASK_IDLE:
		digger_move_random(di);
		break;
	case DTASK_GUARD:
		if (ti == diggers[di].target)
			digger_move_random(di);
		else
		{
			char mi = tile_plan(ti, diggers[di].target);
			digger_move_to(di, mi);
		}
		break;
	case DTASK_WORK:
		digger_work(di);
		break;
	}	
}

void diggers_iterate(void)
{
	for(char i=0; i<32; i++)
	{	
		if (diggers[i].state >= DS_IDLE && diggers[i].health == 0)
		{
			diggers[i].state = DS_DEAD;
			diggers[i].count = 50;
			diggers[i].task = DTASK_DEAD;
		}

		if (diggers[i].state == DS_IDLE)
			digger_decide(i);
	}
}

char diggers_sprites(char si, char sx, char sy)
{
	for(char i=0; i<32; i++)
	{	
		if (diggers[i].state != DS_FREE)
		{
			char dx = diggers[i].tx - sx;
			char dy = diggers[i].ty - sy;

			if (dx < 3 && dy < 3)
			{
				vspr_set(si, 
					12 + dx * 64 + diggers[i].sx * 4,
					44 + dy * 64 + diggers[i].sy * 4,
					diggers[i].mi, diggers[i].color);
				si++;
				if (si == 16)
					break;
			}		
		}
	}

	return si;
}

void diggers_vacate_room(char ri)
{
	for(char i=0; i<32; i++)
	{
		if (diggers[i].target == ri)
		{
			if (diggers[i].task == DTASK_MOVE || diggers[i].task == DTASK_WORK)
			{
				diggers[i].task = DTASK_IDLE;
				diggers[i].state = DS_IDLE;
				diggerchanged = true;
			}
		}
	}
}

void diggers_list(void)
{
	for(char i=0; i<32; i++)	
	{
		char x = (i & 1) * 8 + 24;
		char y = i >> 1;

		if (diggers[i].state != DS_FREE)
		{
			char c = VCOL_BLACK;
			if (i == diggeri)
				c = VCOL_MED_GREY;

			disp_char(x + 0, y, ' ', c, VCOL_BLACK);
			disp_char(x + 1, y, digger_task_char[diggers[i].task], c, VCOL_WHITE + 16 * VCOL_LT_GREY);
			disp_vbar(x + 2, y, 8 - ((diggers[i].health + 7) >> 3), c + 16 * VCOL_LT_GREY, VCOL_WHITE);
			disp_chars(x + 3, y, digger_names + i * 5, 5, c, diggers[i].color + 16 * VCOL_DARK_GREY);
			disp_char(x + 7, y, ' ', c, VCOL_BLACK);
		}
		else
		{
			disp_space(x, y, 8, VCOL_BLACK, VCOL_BLACK);
		}
	}
}

void digger_stats(void)
{

	disp_char(24, 21, digger_task_char[diggers[diggeri].task], VCOL_BLACK, VCOL_WHITE + 16 * VCOL_LT_GREY);
	disp_char(25, 21, ' ', VCOL_BLACK, VCOL_BLACK);
	disp_chars(26, 21, digger_names + diggeri * 5, 5, VCOL_BLACK, diggers[diggeri].color + 16 * VCOL_DARK_GREY);

	disp_rbar(32, 21, diggers[diggeri].health >> 1, 32, VCOL_WHITE + 16 * VCOL_DARK_GREY);

	disp_char(24, 22 , 'A', 0x00, 0xf1);
	disp_rbar(25, 22, diggers[diggeri].ability, DIGGER_MAX_SKILL, VCOL_YELLOW + 16 * VCOL_DARK_GREY);

	disp_char(29, 22 , 'F', 0x00, 0xf1);
	disp_rbar(30, 22, diggers[diggeri].fight, DIGGER_MAX_SKILL, VCOL_RED + 16 * VCOL_DARK_GREY);

	disp_char(34, 22 , 'I', 0x00, 0xf1);
	disp_rbar(35, 22, diggers[diggeri].intelligence, DIGGER_MAX_SKILL, VCOL_BLUE + 16 * VCOL_DARK_GREY);
}

bool digger_work(char di)
{
	if (diggers[di].task == DTASK_WORK && diggers[di].state == DS_IDLE)
	{
		char ti = diggers[di].target;
		char tile = BunkerMapData[ti];
		if (TileFlags[tile] & TF_ROOM)
		{
			RoomTile	rt = tile - 16;
			switch (rt)
			{
			case RTILE_HYDRO:
				if (res_stored[RES_ENERGY])
				{
					res_stored[RES_ENERGY]--;
					diggers[di].count = 4;
					diggers[di].state = DS_WORKING;
					return true;
				}
				break;

			case RTILE_GENERATOR:
				diggers[di].count = 2;
				diggers[di].state = DS_WORKING;
				return true;

			case RTILE_SICKBAY:
				diggers[di].count = 4;
				diggers[di].state = DS_WORKING;
				return true;

			case RTILE_WORKBENCH:
				if (res_stored[RES_ENERGY])
				{
					res_stored[RES_ENERGY]--;
					diggers[di].count = 4;
					diggers[di].state = DS_WORKING;
					return true;
				}
				break;

			case RTILE_LABORATORY:
				if (res_stored[RES_ENERGY] && res_stored[RES_CARBON])
				{
					res_stored[RES_CARBON]--;
					res_stored[RES_METAL]--;
					diggers[di].count = 4;
					diggers[di].state = DS_WORKING;
					return true;
				}
				break;

			case RTILE_MINE:
				if (res_stored[RES_ENERGY] &&
					((TileMapFlags[ti] & GROUND_TYPE_MASK) == GTYPE_METAL ||
					 (TileMapFlags[ti] & GROUND_TYPE_MASK) == GTYPE_CARBON ||
					 (TileMapFlags[ti] & GROUND_TYPE_MASK) == GTYPE_URANIUM))
				{
					res_stored[RES_ENERGY]--;
					diggers[di].count = 8;
					diggers[di].state = DS_WORKING;
					return true;
				}
				else
					return false;

			case RTILE_GYM:
				diggers[di].count = 50 * (1 + (diggers[di].ability >> 2));
				diggers[di].state = DS_WORKING;
				return true;

			case RTILE_ARMOURY:
				diggers[di].count = 50 * (1 + (diggers[di].fight >> 2));
				diggers[di].state = DS_WORKING;
				return true;

			case RTILE_STUDY:
				diggers[di].count = 50 * (1 + (diggers[di].intelligence >> 2));
				diggers[di].state = DS_WORKING;
				return true;

			case RTILE_EXCAVATOR:
				if (res_stored[RES_ENERGY])
				{
					res_stored[RES_ENERGY]--;
					diggers[di].count = 4;
					diggers[di].state = DS_WORKING;
					return true;
				}
				break;
			case RTILE_VENTILATION:
				if (res_stored[RES_ENERGY])
				{
					res_stored[RES_ENERGY]--;
					diggers[di].count = 4;
					diggers[di].state = DS_WORKING;
					return true;
				}
				break;
			}
		}
	}

	return false;
}


bool digger_procreate(void)
{
	if (diggers_born < 32)
	{
		char ri = rand() & 255;
		if (BunkerMapData[ri] == RTILE_QUARTERS + 16)
		{
			char di = diggers_born;

			diggers[di].tx = ri & 0x0f;
			diggers[di].ty = ri >> 4; 
			diggers[di].sy = 8;
			diggers[di].sx = 8;
			diggers[di].mi = 64 + 1;
			diggers[di].color = VCOL_LT_GREY;
			diggers[di].state = DS_IDLE;
			diggers[di].task = DTASK_IDLE;
			diggers[di].target = 0;
			diggers[di].ability = 1;
			diggers[di].fight = 1;
			diggers[di].intelligence = 1;
			diggers[di].health = DIGGER_MAX_HEALTH;

			diggers_born++;
			diggerchanged = true;

			return true;
		}
	}

	return false;
}
