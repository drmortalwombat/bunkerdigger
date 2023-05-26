#include <c64/vic.h>
#include <oscar.h>
#include <c64/memmap.h>
#include <c64/cia.h>
#include <c64/sprites.h>
#include <string.h>
#include <c64/joystick.h>
#include <c64/rasterirq.h>
#include <c64/keyboard.h>
#include <stdlib.h>
#include "display.h"
#include "tiles.h"
#include "digger.h"
#include "minimap.h"
#include "gamemenu.h"
#include "resources.h"
#include "rooms.h"
#include "gamemusic.h"
#include "gameirq.h"
#include <c64/iecbus.h>

#pragma stacksize(512)

#pragma region( main, 0x0880, 0xa000, , , {code, data, bss, heap})
#pragma region( stack, 0x0400, 0x0600, , , {stack})

const char SpriteData[] = {
	#embed spd_sprites lzo "sprites.spd"
};

void iec_write_bytes(const char * data, int num)
{
	for(int i=0; i<num; i++)
		iec_write(data[i]);
}

void iec_read_bytes(char * data, int num)
{
	for(int i=0; i<num; i++)
		data[i] = iec_read();
}

void game_save(void)
{
	char drive = 9;

	__asm { sei }

	iec_open(drive, 2, "@0:DIGGER,P,W");
	iec_listen(drive, 2);

	iec_write(0x22);
	if (iec_status == IEC_OK)
	{
		for(int i=0; i<256; i++)
			iec_write(BunkerMapData[i]);
		for(int i=0; i<256; i++)
			iec_write(TileMapFlags[i]);	
		iec_write(room_num_construction);
		iec_write(rooms_researched);
		iec_write(researching);
		iec_write_bytes((char *)&room_constructions, sizeof(room_constructions));
		iec_write_bytes((char *)&res_oxygen, sizeof(res_oxygen));
		iec_write_bytes((char *)&res_stored, sizeof(res_stored));
		iec_write(diggers_born);
		iec_write_bytes((char *)&diggers, sizeof(diggers));
	}	
	iec_unlisten();
	iec_close(drive, 2);

	__asm { cli }
}

void game_load(void)
{
	char drive = 9;

	__asm { sei }

	iec_open(drive, 2, "@0:DIGGER,P,R");
	iec_talk(drive, 2);
	char v = iec_read();
	if (iec_status == IEC_OK && v == 0x22)
	{
		for(int i=0; i<256; i++)
			BunkerMapData[i] = iec_read();
		for(int i=0; i<256; i++)
			TileMapFlags[i] = iec_read();
		room_num_construction = iec_read();
		rooms_researched = iec_read();
		researching = iec_read();
		iec_read_bytes((char *)&room_constructions, sizeof(room_constructions));
		iec_read_bytes((char *)&res_oxygen, sizeof(res_oxygen));
		iec_read_bytes((char *)&res_stored, sizeof(res_stored));
		diggers_born = iec_read();
		iec_read_bytes((char *)&diggers, sizeof(diggers));
	}

	iec_untalk();
	iec_close(drive, 2);

	__asm { cli }
}

void display_init(void)
{
	mmap_set(MMAP_NO_BASIC);

	mmap_trampoline();

	// Disable interrupts while setting up
	__asm { sei };

	// Kill CIA interrupts
	cia_init();

	mmap_set(MMAP_RAM);

	oscar_expand_lzo(Sprites, SpriteData);

	mmap_set(MMAP_NO_ROM);

	vic_setmode(VICM_HIRES_MC, Screen, Hires);

	vic.color_border = VCOL_BLACK;
	vic.color_back = VCOL_BLACK;

	vic.spr_mcolor0 = VCOL_BLACK;
	vic.spr_mcolor1 = VCOL_WHITE;
	vic.spr_enable = 0xff;
	vic.spr_multi = 0xff;
	vic.spr_expand_x = 0x00;
	vic.spr_expand_y = 0x00;

	memset(Screen, 0, 1000);
	memset(Color, 0, 1000);

	rirq_init_io();
	vspr_init(Screen);	
}

void status_mapview(void)
{
	statusview = STVIEW_MINIMAP;
	minimap_draw();
	minimap_highlight(mapx, mapy);
}

int main(void)
{
	display_init();
	tiles_init();

	minimap_draw();

	diggers_init();

	gameirq_init();

	gmenu_init();

	res_init();

	game_load();

	rooms_count();

	res_stored[RES_METAL] = 16;
	res_stored[RES_WATER] = 16;
	rooms_researched = RTILE_LABORATORY + 1;
	researching = 6 << (rooms_researched - RTILE_LABORATORY);

	statusview = STVIEW_MINIMAP;
	minimap_highlight(mapx, mapy);			
	
	char	rescount = irqcount;
	char	pirqcount = irqcount;

	tmapx = 8; tmapy = 0;
	cursorx = 8; cursory = 0;

	music_init(TUNE_THEME_GENERAL_1);

	for(;;)
	{
		if (tune_queue == tune_current)
		{
			char c = tune_queue - TUNE_THEME_GENERAL_1 + 1 + (rand() & 1);
			if (c >= 3) c -= 3;
			music_queue(TUNE_THEME_GENERAL_1 + c);
		}

		if (tmapmode == TMMODE_REDRAW)
		{
			vic.spr_enable = 0x00;
			mapx = tmapx;
			mapy = tmapy;
			tiles_draw(mapx, mapy);
			if (statusview == STVIEW_MINIMAP)
				minimap_highlight(mapx, mapy);			
			vic.spr_enable = 0xff;
			tmapmode = TMMODE_DRAWN;
		}
		else if (diggerchanged)
		{
			if (statusview == STVIEW_TEAM)
				diggers_list();
			diggerchanged = false;
		}
		else if (buildingchanged)
		{
			rooms_count();				
			if (statusview == STVIEW_BUILD)
				rooms_display();
			buildingchanged = false;
		}
		else if (researching == 0)
		{
			rooms_researched++;
			researching = 4 + (2 << (rooms_researched - RTILE_LABORATORY));

			if (statusview == STVIEW_BUILD)
				rooms_display();
		}
		else if (gmenu != GMENU_NONE)
		{
			switch (gmenu)
			{
			case GMENU_MAP:
				status_mapview();
				break;
			case GMENU_TEAM:
				if (statusview == STVIEW_TEAM)
				{
					cursorx = diggers[diggeri].tx;
					cursory = diggers[diggeri].ty;

					char tx, ty;

					if (cursorx == 0)
						tx = 0;
					else if (cursorx < 15)
						tx = cursorx - 1;
					else
						tx = 14;

					if (cursory == 0)
						ty = 0;
					else if (cursory < 15)
						ty = cursory - 1;
					else
						ty = 14;

					if (tx != tmapx || ty != tmapy)
					{
						tmapx = tx;
						tmapy = ty;
						tmapmode = TMMODE_REDRAW;					
					}
				}
				else
				{				
					statusview = STVIEW_TEAM;
					diggers_list();
				}
				break;
			case GMENU_DIG:
				if (tile_dig(cursorx, cursory))
				{
					tmapmode = TMMODE_REDRAW;
					if (statusview == STVIEW_MINIMAP)
						minimap_draw();
				}
				break;
			case GMENU_ASSIGN:
				if (diggers[diggeri].task != DTASK_DEAD)
				{
					char ri = cursorx + 16 * cursory;

					diggers_vacate_room(ri);
					diggers[diggeri].task = DTASK_MOVE;
					diggers[diggeri].target = ri;
				}
				status_mapview();
				break;
			case GMENU_GUARD:
				if (diggers[diggeri].task != DTASK_DEAD)
				{
					diggers[diggeri].task = DTASK_GUARD;
					diggers[diggeri].target = cursorx + 16 * cursory;
				}
				status_mapview();
				break;
			case GMENU_BUILD:
				if (statusview == STVIEW_BUILD)
				{
					if (rooms_build())
						tmapmode = TMMODE_REDRAW;
					status_mapview();
				}
				else
				{
					statusview = STVIEW_BUILD;
					rooms_display();
				}
				break;

			case GMENU_SAVE:
				game_save();
				break;

			}

			gmenu = GMENU_NONE;
		}
		else
		{			
			while ((char)(irqcount - rescount) >= 3)
			{
				res_update();
				rescount += 3;
				if (rooms_check_construction())
					tmapmode = TMMODE_REDRAW;
				res_display();
				digger_stats();
			}

			diggers_iterate();

			if (2 * room_count[RTILE_QUARTERS] > diggers_born && res_stored[RES_WATER] > 2)
				digger_procreate();

			while (pirqcount == irqcount)
				;
			pirqcount = irqcount;
		}

	}

	return 0;
}
