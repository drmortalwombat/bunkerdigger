#ifndef RESOURCES_H
#define RESOURCES_H

extern signed char	res_oxygen[16];

enum Resource
{
	RES_ENERGY,
	RES_WATER,
	RES_DIGGING,
	RES_BUILDING,
	RES_METAL,
	RES_CARBON,
	RES_URANIUM,
	RES_HEALING,

	RES_RESEARCH,

	NUM_RESOURCES
};

#define MAX_STORAGE		112

extern char	res_stored[NUM_RESOURCES];
extern char res_storage[NUM_RESOURCES];


void res_init(void);

void res_update(void);

void res_display(void);

#pragma compile("resources.c")

#endif
