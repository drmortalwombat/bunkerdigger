#ifndef DIGGER_H
#define DIGGER_H

// Main long term task of a digger, assigned by the player
enum DiggerTask
{
	DTASK_WORK,
	DTASK_TRAIN,
	DTASK_MOVE,
	DTASK_GUARD,
	DTASK_IDLE,
	DTASK_DEAD,
	DTASK_ESCAPED
};

// Current short term state/goal of a digger
enum DiggerState
{
	DS_FREE,
	DS_DEAD,
	DS_IDLE,
	DS_MOVE_RIGHT,
	DS_MOVE_LEFT,
	DS_CLIMB_UP,
	DS_CLIMB_DOWN,
	DS_DEFEND_RIGHT,
	DS_DEFEND_LEFT,
	DS_WORKING,
	DS_ESCAPED
};

// Digger limits
#define DIGGER_MAX_SKILL	16
#define DIGGER_MAX_HEALTH	64

#define DIGGER_MAX_AIR		64

#define DIGGER_WARN_ATTACKED	128

struct Digger
{
	char 					tx, ty;		// Coarse tile position
	char 					sx, sy;		// Fine tile position
	char 					mi;
	char 					color;		// Color for uniform
	volatile DiggerState	state;
	DiggerTask				task;
	volatile char			count;		// State counter of current short term goal

	// References of enemies
	char					target, enemy, warn;

	// Abilities and levels
	char					ability, fight, intelligence, health;
};

// Number of diggers active
extern char	diggers_born;

// Status of all diggers
extern __striped Digger	diggers[32];

// Init digger system
void diggers_init(void);

// Move and animate diggers, called in interrupt
void diggers_move(void);

// Run diggers AI, called in interrupt
void diggers_iterate(void);

// Get other diggers out of the room
void diggers_vacate_room(char ri);

// Update sprites for visible diggers
char diggers_sprites(char si, char sx, char sy);

// Draw the digger minimap
void diggers_minimap(void);

// Draw the list of diggers
void diggers_list(void);

// Draw the stats of the current digger
void digger_stats(void);

// Check if uniform color changed
void digger_check_color(char di);

// Complete work of a digger
bool digger_work(char di);

// Attempt to make more diggers
bool digger_procreate(bool radio);

// Check if there are still some diggers alive
bool diggers_alive(void);

#pragma compile("digger.c")

#endif
