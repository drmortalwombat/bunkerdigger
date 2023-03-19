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

#pragma stacksize(512)

#pragma region( stack, 0x0400, 0x0600, , , {stack})

const char SpriteData[] = {
	#embed spd_sprites lzo "sprites.spd"
};

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

enum IRQPhase
{
	IRQP_MOVE_DIGGER,
	IRQP_MOVE_ENEMY,
	IRQP_UPDATE_SPRITE,
	IRQP_USER_INPUT,

	NUM_IRQPHASE
}	irqphase;

RIRQCode	rirqlow, rirqup, rirqmenu;

signed char pjoyx, pjoyy;
bool		pjoyb;

char		irqcount;

void user_interaction(void)
{
	keyb_poll();
	joy_poll(0);

	if (keyb_key & KSCAN_QUAL_DOWN)
		gmenu_key(keyb_key & KSCAN_QUAL_MASK);

	if (joyb[0])
		gmenu_nav(joyx[0]);
	else if (pjoyb)
		gmenu_push();
	else if (joyx[0] != pjoyx || joyy[0] != pjoyy || tmapmode == TMMODE_DRAWN)
		gmenu_joy(joyx[0], joyy[0]);

	pjoyx = joyx[0]; pjoyy = joyy[0];
	pjoyb = joyb[0];
}

__interrupt void irq_lower(void)
{
//	vic.color_border = VCOL_GREEN;
	vspr_update();
	if (irqphase == IRQP_UPDATE_SPRITE)
	{
//		vic.color_border = VCOL_CYAN;
		rirq_sort();
	}

	if (irqphase == IRQP_USER_INPUT)
	{
		irqphase = IRQP_MOVE_DIGGER;
		irqcount++;
	}
	else
		irqphase++;
//	vic.color_border = VCOL_BLACK;

	vic.spr_priority = 0x00;
}

__interrupt void irq_upper(void)
{
	switch(irqphase)
	{
	case IRQP_MOVE_DIGGER:
//		vic.color_border = VCOL_RED;
		diggers_move();
		break;
	case IRQP_MOVE_ENEMY:
		break;
	case IRQP_UPDATE_SPRITE:
//		vic.color_border = VCOL_YELLOW;
		{
			char si = diggers_sprites(0, mapx, mapy);

			while (si < 16)
			{
				vspr_move(si, 0, 255);
				si++;
			}
		}

//		vic.color_border = VCOL_CYAN;
		vspr_sort();
		break;
	case IRQP_USER_INPUT:
		user_interaction();
		break;
	}
//	vic.color_border = VCOL_BLACK;
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

	rirq_build(&rirqlow, 1);
	rirq_call(&rirqlow, 0, irq_lower);
	rirq_set(8, 250, &rirqlow);

	rirq_build(&rirqup, 1);
	rirq_call(&rirqup, 0, irq_upper);
	rirq_set(9, 10, &rirqup);

	rirq_build(&rirqmenu, 1);
	rirq_write(&rirqmenu, 0, &vic.spr_priority, 0xff);
	rirq_set(10, 241, &rirqmenu);

	vspr_sort();
	vspr_update();
	rirq_sort();

	// start raster IRQ processing

	rirq_start();

	gmenu_init();

	res_init();

	rooms_count();

	res_stored[RES_METAL] = 16;
	res_stored[RES_CARBON] = 16;

	statusview = STVIEW_MINIMAP;
	minimap_highlight(mapx, mapy);			
	
	char	rescount = irqcount;

	for(;;)
	{

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
				tmapmode = TMMODE_REDRAW;
				tile_dig(cursorx, cursory);
				if (statusview == STVIEW_MINIMAP)
					minimap_draw();
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
		}

	}

	return 0;
}
