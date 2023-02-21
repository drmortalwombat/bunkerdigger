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

void user_interaction(void)
{
	keyb_poll();
	joy_poll(0);
	if (keyb_key & KSCAN_QUAL_DOWN)
		gmenu_key(keyb_key & KSCAN_QUAL_MASK);

	if (joyx[0] != pjoyx || joyy[0] != pjoyy || tmapmode == TMMODE_DRAWN)
	{
		pjoyx = joyx[0]; pjoyy = joyy[0];
		gmenu_joy(joyx[0], joyy[0]);
	}
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
		irqphase = IRQP_MOVE_DIGGER;
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

int main(void)
{
	display_init();

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

	for(;;)
	{

		if (tmapmode == TMMODE_REDRAW)
		{
			vic.spr_enable = 0x00;
			mapx = tmapx;
			mapy = tmapy;
			tiles_draw(mapx, mapy);
			minimap_highlight(mapx, mapy);			
			vic.spr_enable = 0xff;
			tmapmode = TMMODE_DRAWN;
		}
		else
			diggers_iterate();

	}

	return 0;
}
