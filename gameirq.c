#include "gameirq.h"
#include "gamemenu.h"
#include "digger.h"
#include "enemies.h"
#include "tiles.h"
#include <c64/joystick.h>
#include <c64/rasterirq.h>
#include <c64/keyboard.h>
#include <c64/vic.h>
#include <audio/sidfx.h>

IRQPhase			irqphase;
volatile char		irqcount;


RIRQCode	rirqlow, rirqup, rirqmenu;

signed char pjoyx, pjoyy;
bool		pjoyb;

char		key_queue;

void key_scan(void)
{
	if (!key_queue)
	{
		keyb_poll();
		if (keyb_key & KSCAN_QUAL_DOWN)
			key_queue = keyb_key;
	}
}

void user_interaction(void)
{
	joy_poll(0);

	if (key_queue & KSCAN_QUAL_DOWN)
	{
		gmenu_key(key_queue & KSCAN_QUAL_MASK);
		key_queue = 0;
	}

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
	if (irqphase == IRQP_UPDATE_SPRITE || irqphase == IRQP_INTRO)
	{
//		vic.color_border = VCOL_CYAN;
		rirq_sort();
	}
//	vic.color_border = VCOL_BLACK;

	if (irqphase == IRQP_WINDOW)
		return;
	
	if (irqphase == IRQP_INTRO)
		;
	else if (irqphase == IRQP_USER_INPUT)
	{
		irqphase = IRQP_MOVE_DIGGER;
		irqcount++;
	}
	else
		irqphase++;

	vic.spr_priority = 0x00;
}

__interrupt void irq_upper(void)
{
//	vic.color_border = VCOL_YELLOW;
	music_play();
//	vic.color_border = VCOL_PURPLE;
	
	switch(irqphase)
	{
	case IRQP_MOVE_DIGGER:
		diggers_move();
		enemies_move();
		key_scan();
		break;
	case IRQP_UPDATE_SPRITE:
//		vic.color_border = VCOL_YELLOW;
		{
			char si = diggers_sprites(0, mapx, mapy);
			si = enemies_sprites(si, mapx, mapy);

			while (si < 16)
			{
				vspr_move(si, 0, 255);
				si++;
			}
		}

//		vic.color_border = VCOL_CYAN;
		vspr_sort();
		key_scan();
		break;
	case IRQP_USER_INPUT:
		key_scan();
		user_interaction();
		break;
	case IRQP_WINDOW:
		for(char i=0; i<16; i++)
			vspr_move(i, 0, 255);
		vspr_sort();
		break;
	case IRQP_INTRO:
		break;
	}

	sidfx_loop_2();
	
//	vic.color_border = VCOL_BLACK;
}

void gameirq_hide(void)
{
	irqphase = IRQP_WINDOW;
	vic.spr_enable = 0x00;

	rirq_set(8, 120, &rirqlow);	
	rirq_set(10, 120, &rirqmenu);	
	rirq_sort();
}

void gameirq_show(void)
{
	rirq_set(8, 250, &rirqlow);	
	rirq_set(10, 241, &rirqmenu);
	rirq_sort();

	irqphase = IRQP_MOVE_DIGGER;
}

void gameirq_init(void)
{
	irqphase = IRQP_WINDOW;	
	
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
}
