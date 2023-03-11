#ifndef DIGGER_H
#define DIGGER_H

enum DiggerTask
{
	DTASK_WORK,
	DTASK_MOVE,
	DTASK_GUARD,
	DTASK_IDLE
};

enum DiggerState
{
	DS_FREE,
	DS_IDLE,
	DS_MOVE_RIGHT,
	DS_MOVE_LEFT,
	DS_CLIMB_UP,
	DS_CLIMB_DOWN
};

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
};

extern __striped Digger	diggers[32];
extern const char * digger_names;

void diggers_init(void);

void diggers_move(void);

void diggers_iterate(void);

void diggers_vacate_room(char ri);

char diggers_sprites(char si, char sx, char sy);

void diggers_list(void);

#pragma compile("digger.c")

#endif
