#ifndef DIGGER_H
#define DIGGER_H


enum DiggerState
{
	DS_IDLE,
	DS_MOVE_RIGHT,
	DS_MOVE_LEFT,
	DS_CLIMB_UP,
	DS_CLIMB_DOWN
};

struct Digger
{
	char tx, ty;
	char sx, sy;
	char mi;
	char color;
	DiggerState	state;
	char		count;
};

extern __striped Digger	diggers[32];

void diggers_init(void);

void diggers_move(void);

void diggers_iterate(void);

char diggers_sprites(char si, char sx, char sy);

#pragma compile("digger.c")

#endif
