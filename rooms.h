#ifndef ROOMS_H
#define ROOMS_H

struct RoomInfo
{
	char	res_metal, res_carbon, res_uranium;
};

struct Construction
{
	char	tile, room, count, step, progress;
};



extern const RoomInfo		room_infos[16];

extern const char room_names[16 * 10 + 1];

extern char room_count[16];

extern Construction			room_constructions[4];
extern char					room_num_construction;

void rooms_display(void);

void rooms_count(void);

bool rooms_build(void);

bool rooms_check_construction(void);

#pragma compile("rooms.c")

#endif