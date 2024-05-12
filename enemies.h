#ifndef ENEMIES_H
#define ENEMIES_H

enum EnemyState
{
	ES_FREE,
	ES_VANISHING,
	ES_DEAD,
	ES_IDLE,
	ES_MOVE_RIGHT,
	ES_MOVE_LEFT,
	ES_ATTACK_RIGHT,
	ES_ATTACK_LEFT
};

struct Enemy
{
	char 					tx, ty;
	char 					sx, sy;
	char 					mi;
	char 					color;
	volatile EnemyState		state;
	volatile char			count;
	char					target, health, damage;
};

extern __striped Enemy	enemies[8];

void enemies_init(void);

void enemies_move(void);

void enemies_iterate(char frames);

char enemies_sprites(char si, char sx, char sy);

#pragma compile("enemies.c")

#endif
