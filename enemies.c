#include "enemies.h"
#include "tiles.h"
#include "digger.h"
#include <c64/vic.h>

char enemies_throttle;

void enemies_init(void)
{
	enemies_throttle = 0;
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
				enemies[i].mi &= ~3;
			}
			else
				enemies[i].mi ^= 2;
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
				enemies[i].mi &= ~3;
			}
			else
				enemies[i].mi ^= 2;
			break;		
		case ES_ATTACK_RIGHT:
		case ES_ATTACK_LEFT:
			if (!--enemies[i].count)
			{
				enemies[i].state = ES_IDLE;
				enemies[i].mi &= ~1;
				enemies[i].sx = 8;
			}
			else if (enemies[i].count & 1)
			{
				if (diggers[enemies[i].target].health >= enemies[i].damage)
					diggers[enemies[i].target].health -= enemies[i].damage;
				else
					diggers[enemies[i].target].health = 0;

				enemies[i].mi ^= 1;
			}
			break;
		case ES_DEAD:
			if (enemies[i].count)
				enemies[i].count--;
			break;
		case ES_IDLE:
			enemies[i].mi &= 0xf8;
			break;
		}
	}
}

static const struct EnemyType
{
	char	mi, color, health, damage;	
}	EnemieProgression[] = {
	{0x60, VCOL_BROWN,      0x10, 0x01},
	{0x68, VCOL_YELLOW,     0x08, 0x03},
	{0x60, VCOL_DARK_GREY,  0x20, 0x01},
	{0x70, VCOL_LT_RED,     0x10, 0x04},
	{0x68, VCOL_LT_BLUE,    0x20, 0x06},
	{0x78, VCOL_DARK_GREY,  0x40, 0x08},
	{0x70, VCOL_BROWN,      0x30, 0x0c},
	{0x78, VCOL_PURPLE,     0x80, 0x10}	
};

void enemy_spawn(char ei)
{
	char ri = rand() & 255;
	char tf = TileFlags[BunkerMapData[ri]];
	if ((tf & TF_BUNKER) && (tf & (TF_LEFT | TF_RIGHT)))
	{
		char et = (rand() & 127) * ((ri & 0xf0) + 0x10 + time_days) >> 12;
		if (et > 7) et = 7;

		enemies[ei].tx = ri & 0x0f;
		enemies[ei].ty = ri >> 4; 
		enemies[ei].sy = 8;
		enemies[ei].sx = 8;
		enemies[ei].mi = EnemieProgression[et].mi;
		enemies[ei].color = EnemieProgression[et].color;
		enemies[ei].state = ES_IDLE;
		enemies[ei].target = 0xff;
		enemies[ei].health = EnemieProgression[et].health;
		enemies[ei].damage = EnemieProgression[et].damage;
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
			diggers[i].enemy = ei;

			if (diggers[i].sx >= enemies[ei].sx)
			{
				enemies[ei].state = ES_ATTACK_RIGHT;
				enemies[ei].mi |= 0x04;
				enemies[ei].sx -= 4;
			}
			else
			{
				enemies[ei].state = ES_ATTACK_LEFT;
				enemies[ei].mi &= ~0x04;
				enemies[ei].sx += 4;
			}
			enemies[ei].target = i;
			enemies[ei].count = 8;
			return true;
		}
	}

	return false;	
}

void enemies_iterate(char frames)
{
	char ei = 0xff;
	for(char i=0; i<8; i++)
	{	
		if (enemies[i].state >= ES_IDLE && enemies[i].health == 0)
		{
			enemies[i].state = ES_DEAD;
			enemies[i].count = 50;
			enemies[i].mi = (enemies[i].mi & 0xf8) | 3;
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

			switch (rand() & 63)
			{
			case 0 ... 7:
				if (enemies[i].tx > 0 && (tf & TF_LEFT))
				{
					enemies[i].count = 16;
					enemies[i].mi &= ~0x04;
					enemies[i].state = ES_MOVE_LEFT;
				}
				break;
			case 8 ... 15:
				if (enemies[i].tx < 15 && (tf & TF_RIGHT))
				{
					enemies[i].count = 16;
					enemies[i].mi |= 0x04;
					enemies[i].state = ES_MOVE_RIGHT;
				}
				break;
			case 16:
				enemies[i].state = ES_FREE;
				break;
			default:
				enemy_attack(i);
			}
			
			} break;
		}
	}

	while ((char)(frames - enemies_throttle) >= 32)
	{
		if (ei != 0xff)
			enemy_spawn(ei);
		enemies_throttle += 32;
	}
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
