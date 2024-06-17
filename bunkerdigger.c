#include <c64/vic.h>
#include <oscar.h>
#include <c64/memmap.h>
#include <c64/cia.h>
#include <c64/sprites.h>
#include <string.h>
#include <c64/joystick.h>
#include <c64/rasterirq.h>
#include <c64/keyboard.h>
#include <c64/sid.h>
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
#include "messages.h"
#include "enemies.h"
#include "window.h"
#include <c64/iecbus.h>

#pragma stacksize(128)

#pragma section(resources, 0)
#pragma section(xbss, 0, , , bss)
#pragma section(ybss, 0, , , bss)

#pragma region( main, 0x0880, 0xa000, , , {code, data, bss, heap})
#pragma region( stack, 0x0800, 0x0880, , , {stack})
#pragma region( resources, 0xc000, 0xd000, , , {resources})
#pragma region( zeropage, 0x80, 0xf8, , , {zeropage})
#pragma region( xbss, 0x0400, 0x0800, , , {xbss})
#pragma region( ybss, 0xc000, 0xc800, , , {ybss})

#pragma data(resources)

const char SpriteData[] = {
	#embed spd_sprites lzo "sprites.spd"
};

#pragma data(data)

char rocket_count;

char save_drive	= 8;

void iec_read_status(void)
{
	iec_talk(save_drive, 15);
	char i = 0;
	while (iec_status == IEC_OK)
		msg_buffer[i++] = iec_read();
	msg_buffer[i] = 0;
	iec_untalk();
}

bool game_save(void)
{	
	bool	ok = false;

	iec_open(save_drive, 2, "@0:DIGGER,P,W");
	iec_listen(save_drive, 2);

	iec_write(0x29);
	if (iec_status < IEC_ERROR)
	{
		iec_write(time_count);
		iec_write(time_days);
		for(int i=0; i<256; i++)
			iec_write(BunkerMapData[i]);
		for(int i=0; i<256; i++)
			iec_write(TileMapFlags[i]);	
		iec_write(room_num_construction);
		iec_write(rooms_researched);
		iec_write(rooms_blueprints);
		iec_write(radio_count);
		iec_write(radio_days);
		iec_write(enemy_days);
		iec_write(moon_days);
		iec_write_bytes((char *)&researching, sizeof(researching));
		iec_write_bytes((char *)&room_constructions, sizeof(room_constructions));
		iec_write_bytes((char *)&res_oxygen, sizeof(res_oxygen));
		iec_write_bytes((char *)&res_stored, sizeof(res_stored));
		iec_write(diggers_born);
		iec_write_bytes((char *)&diggers, sizeof(diggers));
		iec_write(msg_head);
		iec_write(msg_tail);
		iec_write(msg_row);
		iec_write(msg_delay);
		iec_write_bytes((char *)&story_shown, sizeof(story_shown));
		iec_write_bytes((char *)&story_pending, sizeof(story_pending));
		iec_write_bytes((char *)&messages, sizeof(messages));
		iec_write_bytes((char *)&enemies, sizeof(enemies));
		iec_write(rocket_count);
		ok = true;
	}

	iec_unlisten();
	iec_close(save_drive, 2);

	if (!ok)
		iec_read_status();

	return ok;
}

void game_init(void)
{
	tiles_init();
	diggers_init();
	enemies_init();
	res_init();
	story_init();
	rooms_init();	

	tmapx = 8; tmapy = 0;
	cursorx = 8; cursory = 0;
	rocket_count = 0xff;

	rooms_count();
}

bool game_load(void)
{
	bool	ok = false;

	iec_open(save_drive, 2, "@0:DIGGER,P,R");
	iec_talk(save_drive, 2);
	char v = iec_read();
	if (iec_status == IEC_OK && v >= 0x24)
	{
		game_init();

		time_count = iec_read();
		time_days = iec_read();
		for(int i=0; i<256; i++)
			BunkerMapData[i] = iec_read();
		for(int i=0; i<256; i++)
			TileMapFlags[i] = iec_read();
		room_num_construction = iec_read();
		rooms_researched = iec_read();		
		rooms_blueprints = iec_read();
		radio_count = iec_read();
		radio_days = iec_read();
		enemy_days = iec_read();
		if (v >= 0x28)
			moon_days = iec_read();
		if (v >= 0x29)
			iec_read_bytes((char *)&researching, sizeof(researching));
		else
			researching = iec_read();
		iec_read_bytes((char *)&room_constructions, sizeof(room_constructions));
		iec_read_bytes((char *)&res_oxygen, sizeof(res_oxygen));
		iec_read_bytes((char *)&res_stored, sizeof(res_stored));
		diggers_born = iec_read();
		iec_read_bytes((char *)&diggers, sizeof(diggers));
		msg_head = iec_read();
		msg_tail = iec_read();
		msg_row = iec_read();
		msg_delay = iec_read();
		if (v < 0x27)
		{
			story_shown = iec_read();
			story_pending = iec_read();
		}
		else
		{
			iec_read_bytes((char *)&story_shown, sizeof(story_shown));
			iec_read_bytes((char *)&story_pending, sizeof(story_pending));			
		}
		iec_read_bytes((char *)&messages, sizeof(messages));
		iec_read_bytes((char *)&enemies, sizeof(enemies));
		if (v >= 0x25)
			rocket_count = iec_read();
		if (v < 0x26)
		{
			for(char i=0; i<32; i++)
				if (diggers[i].task > DTASK_WORK)
					diggers[i].task++;
		}
		ok = true;
	}

	iec_untalk();
	iec_close(save_drive, 2);

	if (!ok)
		iec_read_status();

	rooms_count();

	return ok;
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

enum MenuItem
{
	MITEM_CONTINUE,
	MITEM_MUSIC,
	MITEM_SAVE,
	MITEM_LOAD,
	MITEM_RESTART,
	MITEM_DRIVE,

	MITEM_COUNT
};

void game_menu(void)
{
	window_open(4, 7, 30, 12);
	window_write(1,  1, "BUNKER CONTROL SYSTEM   V2.0");
	window_write(3,  3, "1.) CONTINUE");
	window_write(3,  4, "2.) MUSIC          [---]");

	if (music_enabled)
		window_write(23, 4, "ON ");
	else
		window_write(23, 4, "OFF");

	window_write(3,  5, "3.) SAVE TO DISK");
	window_write(3,  6, "4.) LOAD FROM DISK");
	window_write(3,  7, "5.) RESTART");
	window_write(3,  8, "6.) SELECT DRIVE   [ - ]");

	window_char(24, 8, '0' + save_drive);
	window_color_rect(0, 10, 30, 1, 0x28);
	window_write(1,  10, "SYSTEM READY.");

	MenuItem	mi = MITEM_CONTINUE;

	signed char pjoyy = 0;
	bool		pjoyb = true;
	for(;;)
	{
		bool	select = false;

		keyb_poll();
		if (keyb_key & KSCAN_QUAL_DOWN)
		{
			switch (keyb_key & KSCAN_QUAL_MASK)
			{
			case KSCAN_RETURN:
			case KSCAN_SPACE:
				select = true;
				break;
			case KSCAN_CSR_DOWN:
				if (mi < MITEM_DRIVE)
					mi++;
				break;
			case KSCAN_CSR_DOWN | KSCAN_QUAL_SHIFT:
				if (mi > MITEM_CONTINUE)
					mi--;
				break;
			case KSCAN_1:
				mi = MITEM_CONTINUE;
				select = true;
				break;
			case KSCAN_2:
				mi = MITEM_MUSIC;
				select = true;
				break;
			case KSCAN_3:
				mi = MITEM_SAVE;
				select = true;
				break;
			case KSCAN_4:
				mi = MITEM_LOAD;
				select = true;
				break;
			case KSCAN_5:
				mi = MITEM_RESTART;
				select = true;
				break;
			case KSCAN_6:
				mi = MITEM_DRIVE;
				select = true;
				break;
			}
		}
		joy_poll(0);
		if (joyy[0] != pjoyy)
		{
			if (joyy[0] < 0 && mi > MITEM_CONTINUE)
				mi--;
			else if (joyy[0] > 0 && mi < MITEM_DRIVE)
				mi++;
		}
		pjoyy = joyy[0];
		if (joyb[0] != pjoyb)
		{
			if (joyb[0])
				select = true;
		}
		pjoyb = joyb[0];

		if (select)
		{
			switch (mi)
			{
			case MITEM_CONTINUE:
				window_close();	
				return;
			case MITEM_RESTART:
				game_init();
				window_close();	
				return;
			case MITEM_LOAD:
				window_write(1, 10, "LOADING...            ");
				if (game_load())
				{
					window_close();
					return;
				}
				else
					window_write(1, 10, msg_buffer);
				break;
			case MITEM_SAVE:
				window_write(1, 10, "SAVING...            ");
				if (game_save())
				{
					window_close();
					return;
				}
				else
					window_write(1, 10, msg_buffer);
				break;
			case MITEM_DRIVE:
				save_drive = save_drive ^ 1;
				window_char(24, 8, '0' + save_drive);
				break;
			case MITEM_MUSIC:
				music_toggle();
				if (music_enabled)
					window_write(23, 4, "ON ");
				else
					window_write(23, 4, "OFF");
				break;
			}
		}

		window_color_rect(3, 3 + mi, 24, 1, 0xd1);
		vic_waitFrame();
		window_color_rect(3, 3 + mi, 24, 1, 0x5d);
	}
}

void status_mapview(void)
{
	statusview = STVIEW_MINIMAP;
	minimap_draw();
	minimap_highlight(mapx, mapy);
}

void game_destroy(void)
{
	tile_effect = TILEF_EXPLODED;
	for(char i=0; i<32; i++)
	{
		if (diggers[i].task != DTASK_DEAD)
		{
			diggers[i].task = DTASK_DEAD;
			diggers[i].state = DS_DEAD;
		}
	}				
}

int main(void)
{
	save_drive = 9;

	display_init();
	disp_init();

	gameirq_init();

	gmenu_init();

	game_init();

	game_load();

	statusview = STVIEW_MINIMAP;
	minimap_highlight(mapx, mapy);			
	
	char	rescount = irqcount;
	char	pirqcount = irqcount;
	char	upcount = 0;
	bool	update_status = false;

	music_init(TUNE_THEME_GENERAL_1);

	tiles_draw(tmapx, tmapy);
	minimap_draw();
	game_menu();
	minimap_draw();

	for(;;)
	{
		if (time_count >= 10)
			story_pending |= 1ul << STM_INTRO;
		if (time_days > enemy_days + 2)
			story_pending |= 1ul << STM_ENEMY_THREADS;
		if (time_days > enemy_days + 22)
			story_pending |= 1ul << SIM_ENEMY_INCOMING;
		if (time_days > enemy_days + 25)
		{
			story_pending |= 1ul << SIM_ENEMY_VICTORY;
			game_destroy();
		}
		if (time_days > moon_days + 5)
		{
			story_pending |= 1ul << STM_MOON_IMPACT;
			game_destroy();
		}


		if (rocket_count < 200)
		{
			rocket_count++;
			if (rocket_count == 200)
			{
				story_pending |= 1 << SIM_MOON_DESTROYED;
				enemy_days = enemy_days;
				moon_days = time_days;
			}
		}

		if (tune_queue == tune_current)
		{
			char c = tune_queue - TUNE_THEME_GENERAL_1 + 1 + (rand() & 1);
			if (c >= 3) c -= 3;
			music_queue(TUNE_THEME_GENERAL_1 + c);
		}

		if (msg_delay > 0)
		{
			msg_delay--;
			if (msg_delay == 0)
			{
				msg_row = 0;
				tmapmode = TMMODE_REDRAW;
			}
		}

		if (tmapmode == TMMODE_REDRAW)
		{
			if (mapx != tmapx || mapy != tmapy)
			{
				vic.spr_enable = 0x00;
				mapx = tmapx;
				mapy = tmapy;
			}
			tiles_draw(mapx, mapy);
			msg_refresh();
			if (statusview == STVIEW_MINIMAP)
				minimap_highlight(mapx, mapy);			
			vic.spr_enable = 0xff;
			tmapmode = TMMODE_DRAWN;
		}
		else if (diggerchanged)
		{
			diggerchanged = false;
			if (statusview == STVIEW_TEAM)
				diggers_list();
		}
		else if (buildingchanged)
		{
			buildingchanged = false;
			rooms_count();				
			if (statusview == STVIEW_BUILD)
				rooms_display();
		}
		else if (researching == 0 && rooms_researched < rooms_blueprints)
		{
			msg_queue(MSG_ROOM_RESEARCHED, rooms_researched);

			rooms_researched++;
			researching = 4 + (2 << (rooms_researched - RTILE_LABORATORY));

			if (statusview == STVIEW_BUILD)
				rooms_display();

			if (rooms_researched > RTILE_LAUNCH_TOP)
				story_pending |= 1 << SIM_ROCKET_PLANS;

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
				window_open(10, 10, 20, 3);
				window_write(2, 1, "SAVING...");
				game_save();
				window_close();
				update_status = true;
				break;

			case GMENU_OPTIONS:
				game_menu();
				update_status = true;
				break;

			case GMENU_HISTORY:
				msg_show_history();
				break;

			case GMENU_LAUNCH:
				if (story_pending & SIM_ROCKET_LAUNCHED)
				{
					if (rooms_launch(false))
					{
						story_pending |= 1 << SIM_MARS_ESCAPED;
						tmapmode = TMMODE_REDRAW;			
						rocket_count = 0;
						tile_effect = TILEF_ESCAPED;
						for(char i=0; i<32; i++)
						{
							if (diggers[i].task != DTASK_DEAD)
							{
								diggers[i].task = DTASK_ESCAPED;
								diggers[i].state = DS_ESCAPED;
								diggers[i].color = VCOL_WHITE;
							}
						}
					}
				}
				else if (rooms_launch(false))
				{
					story_pending |= 1 << SIM_ROCKET_LAUNCHED;
					tmapmode = TMMODE_REDRAW;			
					rocket_count = 0;		
				}
				break;
			}

			gmenu = GMENU_NONE;
		}
		else
		{	
			bool	update = false;
			while ((char)(irqcount - rescount) >= 3)
			{
				res_update();
				rescount += 3;
				if (rooms_check_construction())
					tmapmode = TMMODE_REDRAW;
				update = true;
				upcount++;
			}

			if (update)
			{
				res_display();
				digger_stats();
			}

			diggers_iterate();
			enemies_iterate(irqcount);

			digger_procreate(false);

			while (pirqcount == irqcount)
				;
			pirqcount = irqcount;

			if (upcount > 10)
			{
				if (statusview == STVIEW_BUILD)
					rooms_display();

				if (tile_effect == TILEF_NORMAL && !diggers_alive())
				{
					story_pending |= 1ul << STM_DIGGERS_DEAD;
					tile_effect = TILEF_ESCAPED;
				}

				upcount = 0;
			}

			if (story_messages() || update_status)
			{
				switch (statusview)
				{
				case STVIEW_BUILD:
					rooms_display();
					break;
				case STVIEW_MINIMAP:
					minimap_draw();
					minimap_highlight(mapx, mapy);
					break;
				case STVIEW_TEAM:
					diggers_list();
					break;
				}

				update_status = false;
			}

		}

	}

	return 0;
}
