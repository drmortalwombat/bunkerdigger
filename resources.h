#ifndef RESOURCES_H
#define RESOURCES_H

// Oygen available for floor levels in mine
extern signed char	res_oxygen[16];

// Global resouces
enum Resource
{
	RES_ENERGY,
	RES_WATER,
	RES_BUILDING,
	RES_HEALING,

	RES_METAL,
	RES_CARBON,
	RES_URANIUM,
	RES_FUEL,

	RES_RESEARCH,

	NUM_RESOURCES
};

// Maximum storage per resources
#define MAX_STORAGE		112

// Stored resources
extern char	res_stored[NUM_RESOURCES];

// Storage space for resource
extern char res_storage[NUM_RESOURCES];

// Time counters for game progress
extern char time_count, time_days, enemy_days, moon_days;
extern char radio_count, radio_days;

// Init resource system
void res_init(void);

// Update resources
void res_update(void);

// Display resources in status area
void res_display(void);

#pragma compile("resources.c")

#endif
