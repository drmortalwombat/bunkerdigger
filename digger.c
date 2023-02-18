#include "digger.h"
#include "tiles.h"
#include <c64/sprites.h>

extern __striped Digger	diggers[32];

void diggers_init(void)
{
	char tx = 0;
	char ty = 0;
	for(char i=0; i<32; i++)
	{		
		while (!TileFlags[BunkerMapData[ty * 16 + tx]])
		{
			tx++;
			if (tx == 16)
			{
				ty++;
				tx = 0;
			}
		}

		diggers[i].tx = tx;
		diggers[i].ty = ty; 
		diggers[i].sy = 8;
		diggers[i].sx = 8;
		diggers[i].mi = 64 + 1;
		diggers[i].color = 2 + (i % 14);
		diggers[i].state = DS_IDLE;

		tx++;
		if (tx == 16)
		{
			ty++;
			tx = 0;
		}
	}
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

void diggers_iterate(void)
{
	for(char i=0; i<32; i++)
	{	
		switch(diggers[i].state)
		{
		case DS_IDLE:
			{
				char ti = BunkerMapData[diggers[i].ty * 16 + diggers[i].tx];
				char tf = TileFlags[ti];

				switch (rand() & 15)
				{
				case 0:
					if (diggers[i].tx > 0 && (tf & TF_LEFT))
					{
						diggers[i].count = 16;
						diggers[i].mi = 64 + 3;
						diggers[i].state = DS_MOVE_LEFT;
					}
					break;
				case 1:
					if (diggers[i].tx < 15 && (tf & TF_RIGHT))
					{
						diggers[i].count = 16;
						diggers[i].mi = 64 + 11;
						diggers[i].state = DS_MOVE_RIGHT;
					}
					break;
				case 2:
					if (diggers[i].tx > 0 && (tf & TF_UP))
					{
						diggers[i].count = 16;
						diggers[i].mi = 64 + 18;
						diggers[i].state = DS_CLIMB_UP;
					}
					break;
				case 3:
					if (diggers[i].tx < 15 && (tf & TF_DOWN))
					{
						diggers[i].count = 16;
						diggers[i].mi = 64 + 18;
						diggers[i].state = DS_CLIMB_DOWN;
					}
					break;
				}
			break;
			}
		}
	}
}

char diggers_sprites(char si, char sx, char sy)
{
	for(char i=0; i<32; i++)
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

	return si;
}
