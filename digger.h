#ifndef DIGGER_H
#define DIGGER_H

enum DiggerTask
{
	DTASK_WORK,
	DTASK_MOVE,
	DTASK_GUARD,
	DTASK_IDLE,
	DTASK_DEAD
};

enum DiggerState
{
	DS_FREE,
	DS_DEAD,
	DS_IDLE,
	DS_MOVE_RIGHT,
	DS_MOVE_LEFT,
	DS_CLIMB_UP,
	DS_CLIMB_DOWN,
	DS_WORKING
};

#define DIGGER_MAX_SKILL	16
#define DIGGER_MAX_HEALTH	64

#define DIGGER_MAX_AIR		64

struct Digger
{
	char 					tx, ty;
	char 					sx, sy;
	char 					mi;
	char 					color;
	volatile DiggerState	state;
	DiggerTask				task;
	volatile char			count;
	char					target;
	char					ability, fight, intelligence, health;
};

extern char	diggers_born;

extern __striped Digger	diggers[32];

void diggers_init(void);

void diggers_move(void);

void diggers_iterate(void);

void diggers_vacate_room(char ri);

char diggers_sprites(char si, char sx, char sy);

void diggers_minimap(void);

void diggers_list(void);

void digger_stats(void);

void digger_check_color(char di);

bool digger_work(char di);

bool digger_procreate(bool radio);

#pragma compile("digger.c")

#endif
