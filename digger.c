#include "digger.h"
#include "tiles.h"
#include "resources.h"
#include "display.h"
#include "minimap.h"
#include "gamemenu.h"
#include "messages.h"
#include "enemies.h"
#include <c64/sprites.h>

extern __striped Digger	diggers[32];

static const char digger_task_char[] = {
	'W', 'M', 'G', 'I', 'D'
};

char diggers_born;

const char digger_names[] = 
	"MIKE JILL JIM  JANE JOE  CARL CLIFFLISA "
	"MONA MARK BILL ANDY TILLYSUSANMARTHBETH "
	"ALEX FRED MARIAJUDY MINA CHRISBOB  TIM  "
	"TOM  TONIAKIM  JAMESSIMONLINDAANNA OLGA ";

#define DIF_ABILITY			0x00
#define DIF_FIGHT			0x40
#define DIF_INTELLIGENCE	0x80


const char diggers_flags[] = {
	DIF_ABILITY      + 0,
	DIF_FIGHT        + 1,
	DIF_FIGHT        + 1,
	DIF_ABILITY      + 1,

	DIF_INTELLIGENCE + 1,
	DIF_ABILITY      + 1,
	DIF_FIGHT        + 2,
	DIF_INTELLIGENCE + 1,

	DIF_ABILITY      + 1,
	DIF_FIGHT        + 2,
	DIF_ABILITY      + 1,
	DIF_ABILITY      + 2,

	DIF_INTELLIGENCE + 2,
	DIF_FIGHT        + 3,
	DIF_FIGHT        + 2,
	DIF_ABILITY      + 3,

	DIF_ABILITY      + 4,
	DIF_FIGHT        + 4,
	DIF_ABILITY      + 6,
	DIF_ABILITY      + 5,

	DIF_INTELLIGENCE + 7,
	DIF_FIGHT        + 5,
	DIF_FIGHT        + 9,
	DIF_ABILITY      + 6,

	DIF_ABILITY      + 8,
	DIF_FIGHT        + 10,
	DIF_ABILITY      + 7,
	DIF_ABILITY      + 11,

	DIF_INTELLIGENCE + 12,
	DIF_ABILITY      + 13,
	DIF_FIGHT        + 8,
	DIF_ABILITY      + 16
};

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
	diggers[0].enemy = 0xff;

	digger_check_color(0);

	diggers_born = 1;
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
		if (statusview == STVIEW_MINIMAP && diggers[i].state != DS_FREE)
		{
			char dx = diggers[i].tx;
			char dy = diggers[i].ty;
			char sx = diggers[i].sx >> 2;
			char sy = diggers[i].sy >> 2;

			__assume(dx < 16);
			__assume(sx < 16);
			__assume(sy < 16);

			char * dp = MinimapHiresTab[dy] + (8 * dx + 2 * sy);

			char m = ~(0xc0 >> 2 * sx);
			dp[0] &= m;
			dp[1] &= m;
		}

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
		case DS_DEFEND_LEFT:
		case DS_DEFEND_RIGHT:
			if (enemies[diggers[i].enemy].health > 0)
				enemies[diggers[i].enemy].health--;			
			if (!--diggers[i].count)
			{
				diggers[i].state = DS_IDLE;
				diggers[i].mi = 65;
			}
			break;
		case DS_IDLE:
			diggers[i].mi = 65;
			break;
		}

		if (statusview == STVIEW_MINIMAP && diggers[i].state != DS_FREE)
		{
			char dx = diggers[i].tx;
			char dy = diggers[i].ty;
			char sx = diggers[i].sx >> 2;
			char sy = diggers[i].sy >> 2;

			__assume(dx < 16);
			__assume(sx < 16);
			__assume(sy < 16);

			char * dp = MinimapHiresTab[dy] + 8 * dx + 2 * sy;

			char m = 0x55 & (0xc0 >> 2 * sx);
			dp[0] |= m;
			dp[1] |= m;
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

bool digger_defend(char di)
{
	for(char i=0; i<8; i++)
	{
		if (enemies[i].state >= ES_IDLE &&
			enemies[i].tx == diggers[di].tx &&
			enemies[i].ty == diggers[di].ty)
		{
			if (enemies[i].sx >= diggers[di].sx)
			{
				diggers[di].state = DS_DEFEND_RIGHT;
				diggers[di].mi = 64 + 17;
			}
			else
			{
				diggers[di].state = DS_DEFEND_LEFT;
				diggers[di].mi = 64 + 9;
			}
			diggers[di].enemy = i;
			diggers[di].count = 8;
			return true;
		}
	}

	diggers[di].enemy = 0xff;
	return false;
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
		if (!digger_defend(di))
			digger_move_random(di);
		break;
	case DTASK_GUARD:
		if (!digger_defend(di))
		{
			if (ti == diggers[di].target)
				digger_move_random(di);
			else
			{
				char mi = tile_plan(ti, diggers[di].target);
				digger_move_to(di, mi);
			}
		}
		break;
	case DTASK_WORK:
		if (diggers[di].enemy == 0xff || !digger_defend(di))
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
			msg_queue(MSG_DIGGER_KILLED, i);
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
				char c = diggers[i].color;
				if (i == diggeri && (irqcount & 1))
					c = VCOL_DARK_GREY;

				vspr_set(si, 
					12 + dx * 64 + diggers[i].sx * 4,
					44 + dy * 64 + diggers[i].sy * 4,
					diggers[i].mi, c);
				si++;
				if (si == 16)
					break;
			}		
		}
	}

	return si;
}

void diggers_minimap_hide(void)
{
	for(char i=0; i<32; i++)
	{	
		if (diggers[i].state != DS_FREE)
		{
			char dx = diggers[i].tx;
			char dy = diggers[i].ty;
			char sx = diggers[i].sx >> 2;
			char sy = diggers[i].sy >> 2;

			__assume(dx < 16);
			__assume(sx < 16);
			__assume(sy < 16);

			char * dp = MinimapHiresTab[dy] + 8 * dx + 2 * sy;

			char m = ~(0xc0 >> 2 * sx);
			dp[0] &= m;
			dp[1] &= m;
		}
	}
}

void diggers_minimap_show(void)
{
	for(char i=0; i<32; i++)
	{	
		if (diggers[i].state != DS_FREE)
		{
			char dx = diggers[i].tx;
			char dy = diggers[i].ty;
			char sx = diggers[i].sx >> 2;
			char sy = diggers[i].sy >> 2;

			__assume(dx < 16);
			__assume(sx < 16);
			__assume(sy < 16);

			char * dp = MinimapHiresTab[dy] + 8 * dx + 2 * sy;

			char m = 0x55 & (0xc0 >> 2 * sx);
			dp[0] |= m;
			dp[1] |= m;
		}
	}
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
			disp_vbar(x + 2, y, 8 - ((diggers[i].health + 7) >> 3), c + 16 * VCOL_LT_GREY, VCOL_LT_RED);
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

	disp_rbar(32, 21, diggers[diggeri].health >> 1, 32, 32, VCOL_LT_RED + 16 * VCOL_DARK_GREY);

	disp_char(24, 22 , 'A', 0x00, 0xf1);
	disp_rbar(25, 22, diggers[diggeri].ability, DIGGER_MAX_SKILL, DIGGER_MAX_SKILL, VCOL_YELLOW + 16 * VCOL_DARK_GREY);

	disp_char(29, 22 , 'F', 0x00, 0xf1);
	disp_rbar(30, 22, diggers[diggeri].fight, DIGGER_MAX_SKILL, DIGGER_MAX_SKILL, VCOL_RED + 16 * VCOL_DARK_GREY);

	disp_char(34, 22 , 'I', 0x00, 0xf1);
	disp_rbar(35, 22, diggers[diggeri].intelligence, DIGGER_MAX_SKILL, DIGGER_MAX_SKILL, VCOL_BLUE + 16 * VCOL_DARK_GREY);
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
				if (res_stored[RES_ENERGY] && res_stored[RES_CARBON] && rooms_researched < rooms_blueprints)
				{
					res_stored[RES_CARBON]--;
					res_stored[RES_ENERGY]--;
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
				diggers[di].count = 50 * (1 + ((diggers[di].ability + diggers[di].fight + diggers[di].intelligence) >> 2));
				diggers[di].state = DS_WORKING;
				return true;

			case RTILE_ARMOURY:
				diggers[di].count = 50 * (1 + ((diggers[di].ability + diggers[di].fight + diggers[di].intelligence) >> 2));
				diggers[di].state = DS_WORKING;
				return true;

			case RTILE_STUDY:
				diggers[di].count = 50 * (1 + ((diggers[di].ability + diggers[di].fight + diggers[di].intelligence) >> 2));
				diggers[di].state = DS_WORKING;
				return true;

			case RTILE_CENTRIFUGE:
				if (res_stored[RES_ENERGY] >= 8 && res_stored[RES_URANIUM])
				{
					res_stored[RES_ENERGY] -= 8;
					res_stored[RES_URANIUM]--;
					diggers[di].count = 16;
					diggers[di].state = DS_WORKING;
					return true;
				}
				break;
			case RTILE_VENTILATION:
			case RTILE_RADIO:
				if (res_stored[RES_ENERGY])
				{
					res_stored[RES_ENERGY]--;
					diggers[di].count = 4;
					diggers[di].state = DS_WORKING;
					return true;
				}
				break;

			case RTILE_LAUNCH_TOP:
			case RTILE_LAUNCH_MID:
			case RTILE_LAUNCH_BOTTOM:
				if (res_stored[RES_ENERGY] >= 8 && res_stored[RES_URANIUM] >= 16 && res_stored[RES_CARBON] >= 16)
				{
					res_stored[RES_ENERGY] -= 8;
					res_stored[RES_URANIUM] -= 16;
					res_stored[RES_CARBON] -= 16;
					diggers[di].count = 32;
					diggers[di].state = DS_WORKING;
					return true;
				}
				break;

			}
		}
	}

	return false;
}


bool digger_procreate(bool radio)
{
	if (2 * room_count[RTILE_QUARTERS] > diggers_born && res_stored[RES_WATER] > 2 && diggers_born < (radio ? 32 : 12))
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
			diggers[di].enemy = 0xff;

			switch (diggers_flags[di] & 0xc0)
			{
			case DIF_ABILITY:
				diggers[di].ability += diggers_flags[di] & 0x3f;
				break;
			case DIF_FIGHT:
				diggers[di].fight += diggers_flags[di] & 0x3f;
				break;
			case DIF_INTELLIGENCE:
				diggers[di].intelligence += diggers_flags[di] & 0x3f;
				break;
			}

			if (radio)
				msg_queue(MSG_DIGGER_ARRIVED, di);
			else
				msg_queue(MSG_DIGGER_DEHYBERNATED, di);

			diggers_born++;
			diggerchanged = true;

			return true;
		}
	}

	return false;
}
