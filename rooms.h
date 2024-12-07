#ifndef ROOMS_H
#define ROOMS_H

// Resource cost per room type
struct RoomInfo
{
	char	res_metal, res_carbon, res_uranium;
};

// Construction in progress
struct Construction
{
	char	tile, room, count, step, progress;
};



extern const RoomInfo		room_infos[16];

extern const char room_names[16 * 10 + 1];

// Number of rooms per type
extern char room_count[22];

// Construction projects in progress
extern __striped Construction	room_constructions[4];
extern char						room_num_construction;

// Level of rooms researched or available as blueprints
extern char						rooms_researched, rooms_blueprints;

// Resources required until current research is finished
extern unsigned					researching;

// Init room system
void rooms_init(void);

// Show rooms menu
void rooms_display(void);

// Count rooms by type
void rooms_count(void);

// Advance room build projects
bool rooms_build(void);

// Check if rooms are under construction
bool rooms_check_construction(void);

// Launch rocket if ready
bool rooms_launch(bool mars);

#pragma compile("rooms.c")

#endif
