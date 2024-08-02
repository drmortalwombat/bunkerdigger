#include "enemies.h"
#include "tiles.h"
#include "digger.h"
#include "gamemenu.h"
#include <c64/vic.h>

#pragma bss(xbss)
__striped Enemy	enemies[8];
#pragma bss(bss)

char enemies_throttle;

void enemies_init(void)
{
	enemies_throttle = 0;
	for(char i=0; i<8; i++)
		enemies[i].state = ES_FREE;
}

static const char defmap[] = {
	0, 1, 1, 2,
	2, 3, 3, 4,
	4, 5, 6, 8,
	12, 16, 24, 32,
	48
};

static const char defshift[] = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 1, 1, 1,
	2, 2, 2, 2,
	3
};

void enemies_move(void)
{
	for(char i=0; i<8; i++)
	{	
		if (statusview == STVIEW_MINIMAP && enemies[i].state != ES_FREE)
		{
			char dx = enemies[i].tx;
			char dy = enemies[i].ty;
			char sx = enemies[i].sx >> 2;
			char sy = enemies[i].sy >> 2;

			__assume(dx < 16);
			__assume(sx < 16);
			__assume(sy < 16);

			char * dp = MinimapHiresTab[dy] + (8 * dx + 2 * sy);

			char m = ~(0xc0 >> 2 * sx);
			dp[0] &= m;
			dp[1] &= m;
		}

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
			enemies[i].count--;

			if (!(enemies[i].count & 3))
			{
				char ti = enemies[i].target;
				char def = defmap[diggers[ti].fight];

				if (enemies[i].damage > def)
				{
					char dmg = (enemies[i].damage - def) >> defshift[diggers[ti].fight];
					if (diggers[ti].health > dmg)
					{
						if (!diggers[ti].warn && diggers[ti].health < 32)
							diggers[ti].warn |= DIGGER_WARN_ATTACKED;
						diggers[ti].warn |= 31;

						diggers[ti].health -= dmg;
					}
					else
						diggers[ti].health = 0;
				}

				enemies[i].mi ^= 1;
			}

			if (!enemies[i].count)
			{
				enemies[i].state = ES_IDLE;
				enemies[i].mi &= ~1;
				enemies[i].sx = 8;
			}
			break;
		case ES_DEAD:
			if (enemies[i].count)
				enemies[i].count--;
			break;
		case ES_IDLE:
			enemies[i].mi &= 0xf8;
			break;
		case ES_VANISHING:
			enemies[i].state = ES_FREE;
			break;
		}

		if (statusview == STVIEW_MINIMAP && enemies[i].state != ES_FREE)
		{
			char dx = enemies[i].tx;
			char dy = enemies[i].ty;
			char sx = enemies[i].sx >> 2;
			char sy = enemies[i].sy >> 2;

			__assume(dx < 16);
			__assume(sx < 16);
			__assume(sy < 16);

			char * dp = MinimapHiresTab[dy] + 8 * dx + 2 * sy;

			char m = 0xaa & (0xc0 >> 2 * sx);
			dp[0] |= m;
			dp[1] |= m;
		}		
	}
}

static const struct EnemyType
{
	char	mi, color, health, damage;	
}	EnemyProgression[] = {
	{0x60, VCOL_BROWN,      0x10,   1},
	{0x68, VCOL_YELLOW,     0x08,   3},
	{0x60, VCOL_DARK_GREY,  0x14,   4},
	{0x70, VCOL_LT_RED,     0x1c,   8},
	{0x68, VCOL_LT_BLUE,    0x20,  12},
	{0x78, VCOL_DARK_GREY,  0x40,  20},
	{0x70, VCOL_BROWN,      0x30,  32},
	{0x78, VCOL_PURPLE,     0x80,  64}	
};

void enemy_spawn(char ei)
{
	if (time_days >= 2)
	{
		char ri = rand() & 255;
		char tf = TileFlags[BunkerMapData[ri]];
		if ((tf & TF_BUNKER) && (tf & (TF_LEFT | TF_RIGHT)))
		{
			char et = ((rand() & 31) + 32) * ((ri & 0xf0) + 16 + 4 * time_days) >> 12;
			if (et > 7) et = 7;

			enemies[ei].tx = ri & 0x0f;
			enemies[ei].ty = ri >> 4; 
			enemies[ei].sy = 8;
			enemies[ei].sx = 8;
			enemies[ei].mi = EnemyProgression[et].mi;
			enemies[ei].color = EnemyProgression[et].color;
			enemies[ei].state = ES_IDLE;
			enemies[ei].target = 0xff;
			enemies[ei].health = EnemyProgression[et].health;
			enemies[ei].damage = EnemyProgression[et].damage;
		}
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
				enemies[i].state = ES_VANISHING;
			break;
		case ES_FREE:
			ei = i;
			break;
		case ES_IDLE:
			{
			char ti = enemies[i].ty * 16 + enemies[i].tx;
			char tf = TileFlags[BunkerMapData[ti]];

			switch (rand() & 255)
			{
			case 0 ... 31:
				if (enemies[i].tx > 0 && (tf & TF_LEFT))
				{
					enemies[i].count = 16;
					enemies[i].mi &= ~0x04;
					enemies[i].state = ES_MOVE_LEFT;
				}
				break;
			case 32 ... 63:
				if (enemies[i].tx < 15 && (tf & TF_RIGHT))
				{
					enemies[i].count = 16;
					enemies[i].mi |= 0x04;
					enemies[i].state = ES_MOVE_RIGHT;
				}
				break;
			case 64:
			case 65:
				enemies[i].state = ES_VANISHING;
				break;
			default:
				enemy_attack(i);
			}
			
			} break;
		}
	}

	while ((char)(frames - enemies_throttle) >= 64)
	{
		if (ei != 0xff)
			enemy_spawn(ei);
		enemies_throttle += 64;
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
