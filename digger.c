#include "digger.h"
#include "tiles.h"
#include <c64/sprites.h>

extern __striped Digger	diggers[32];

static const char digger_task_char[] = {
	'W', 'M', 'G', 'I'
};

extern const char * digger_names = 
	"MIKE JIM  JOE  CARL CLIFFMARK BILL ANDY "
	"ALEX FRED CHRISBOB  TIM  TOM  JAMESSIMON"
	"JILL JANE LISA MONA TILLYSUSANMARTHBETH "
	"LINDAANNA MARIAJUDY MINA OLGA TONIAKIM  ";


void diggers_init(void)
{
	for(char i=0; i<32; i++)
		diggers[i].state = DS_FREE;

	diggers[0].tx = 0;
	diggers[0].ty = 0; 
	diggers[0].sy = 8;
	diggers[0].sx = 8;
	diggers[0].mi = 64 + 1;
	diggers[0].color = VCOL_RED;
	diggers[0].state = DS_IDLE;
	diggers[0].task = DTASK_MOVE;
	diggers[0].target = 0x4a;

	diggers[1].tx = 8;
	diggers[1].ty = 6; 
	diggers[1].sy = 8;
	diggers[1].sx = 8;
	diggers[1].mi = 64 + 1;
	diggers[1].color = VCOL_YELLOW;
	diggers[1].state = DS_IDLE;
	diggers[1].task = DTASK_MOVE;
	diggers[1].target = 0x4a;

	diggers[2].tx = 0;
	diggers[2].ty = 4; 
	diggers[2].sy = 8;
	diggers[2].sx = 8;
	diggers[2].mi = 64 + 1;
	diggers[2].color = VCOL_BLUE;
	diggers[2].state = DS_IDLE;
	diggers[2].task = DTASK_MOVE;
	diggers[2].target = 0x4a;
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
			vic.color_border++;
			char mi = tile_plan(ti, diggers[di].target);
			vic.color_border--;

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
			vic.color_border++;
			char mi = tile_plan(ti, diggers[di].target);
			vic.color_border--;
			digger_move_to(di, mi);
		}
		break;
	}	
}

void diggers_iterate(void)
{
	for(char i=0; i<32; i++)
	{	
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
			disp_vbar(x + 2, y, y & 7, c, VCOL_WHITE + 16 * VCOL_LT_GREY);
			disp_chars(x + 3, y, digger_names + i * 5, 5, c, diggers[i].color + 16 * VCOL_DARK_GREY);
			disp_char(x + 7, y, ' ', c, VCOL_BLACK);
		}
		else
		{
			disp_space(x, y, 8, VCOL_BLACK, VCOL_BLACK);
		}
	}
}
