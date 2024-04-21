#include "enemies.h"
#include "tiles.h"
#include "digger.h"
#include <c64/vic.h>

void enemies_init(void)
{
	for(char i=0; i<8; i++)
		enemies[i].state = ES_FREE;
}

void enemies_move(void)
{
	for(char i=0; i<8; i++)
	{	
		switch(enemies[i].state)
		{
		case ES_MOVE_RIGHT:
			if (enemies[i].sx == 15)
			{
				enemies[i].tx++;
				enemies[i].sx = 0;
			}
			else
				enemies[i].sx++;

			if (!--enemies[i].count)
			{
				enemies[i].state = ES_IDLE;
				enemies[i].mi = 98;
			}
			else
				enemies[i].mi ^= 1;
			break;
		case ES_MOVE_LEFT:
			if (enemies[i].sx == 0)
			{
				enemies[i].tx--;
				enemies[i].sx = 15;
			}
			else
				enemies[i].sx--;

			if (!--enemies[i].count)
			{
				enemies[i].state = ES_IDLE;
				enemies[i].mi = 96;
			}
			else
				enemies[i].mi ^= 1;
			break;		
		case ES_ATTACK_RIGHT:
		case ES_ATTACK_LEFT:
			if (!--enemies[i].count)
			{
				enemies[i].state = ES_IDLE;
				enemies[i].mi = 98;
			}
			else
			{
				if (diggers[enemies[i].target].health > 0)
					diggers[enemies[i].target].health--;			

				enemies[i].mi ^= 1;
			}
			break;
		case ES_DEAD:
			if (enemies[i].count)
				enemies[i].count--;
			break;
		case ES_IDLE:
			enemies[i].mi = 96;
			break;
		}
	}
}

void enemy_spawn(char ei)
{
	char ri = rand() & 255;
	if (TileFlags[BunkerMapData[ri]] & TF_BUNKER)
	{
		enemies[ei].tx = ri & 0x0f;
		enemies[ei].ty = ri >> 4; 
		enemies[ei].sy = 8;
		enemies[ei].sx = 8;
		enemies[ei].mi = 96;
		enemies[ei].color = VCOL_YELLOW;
		enemies[ei].state = ES_IDLE;
		enemies[ei].target = 0xff;
		enemies[ei].health = 0x10;
	}
}

bool enemy_attack(char ei)
{
	for(char i=0; i<32; i++)
	{
		if (diggers[i].state >= DS_IDLE &&
			diggers[i].tx == enemies[ei].tx &&
			diggers[i].ty == enemies[ei].ty)
		{
			if (diggers[i].sx >= enemies[ei].sx)
			{
				enemies[ei].state = ES_ATTACK_RIGHT;
				enemies[ei].mi = 98;
			}
			else
			{
				enemies[ei].state = ES_ATTACK_LEFT;
				enemies[ei].mi = 96;
			}
			enemies[ei].target = i;
			enemies[ei].count = 8;
			return true;
		}
	}

	return false;	
}

void enemies_iterate(void)
{
	char ei = 0xff;
	for(char i=0; i<8; i++)
	{	
		if (enemies[i].state >= ES_IDLE && enemies[i].health == 0)
		{
			enemies[i].state = ES_DEAD;
			enemies[i].count = 50;
			enemies[i].mi = 100;
		}

		switch(enemies[i].state)
		{
		case ES_DEAD:
			if (enemies[i].count == 0)
				enemies[i].state = ES_FREE;
			break;
		case ES_FREE:
			ei = i;
			break;
		case ES_IDLE:
			{
			char ti = enemies[i].ty * 16 + enemies[i].tx;
			char tf = TileFlags[BunkerMapData[ti]];

			switch (rand() & 7)
			{
			case 0:
				if (enemies[i].tx > 0 && (tf & TF_LEFT))
				{
					enemies[i].count = 16;
					enemies[i].mi = 96;
					enemies[i].state = ES_MOVE_LEFT;
				}
				break;
			case 1:
				if (enemies[i].tx < 15 && (tf & TF_RIGHT))
				{
					enemies[i].count = 16;
					enemies[i].mi = 98;
					enemies[i].state = ES_MOVE_RIGHT;
				}
				break;
			default:
				enemy_attack(i);
			}
			
			} break;
		}
	}

	if (ei != 0x3f && !(rand() & 0xff))
		enemy_spawn(ei);
}


char enemies_sprites(char si, char sx, char sy)
{
	if (si < 16)
	{
		for(char i=0; i<8; i++)
		{	
			if (enemies[i].state != ES_FREE)
			{
				char dx = enemies[i].tx - sx;
				char dy = enemies[i].ty - sy;

				if (dx < 3 && dy < 3)
				{
					char c = enemies[i].color;
					vspr_set(si, 
						12 + dx * 64 + enemies[i].sx * 4,
						44 + dy * 64 + enemies[i].sy * 4,
						enemies[i].mi, c);
					si++;
					if (si == 16)
						break;
				}		
			}
		}
	}

	return si;
}
