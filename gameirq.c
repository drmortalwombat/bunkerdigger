#include "gameirq.h"
#include "gamemenu.h"
#include "digger.h"
#include "tiles.h"
#include <c64/joystick.h>
#include <c64/rasterirq.h>
#include <c64/keyboard.h>
#include <c64/vic.h>

IRQPhase			irqphase;
volatile char		irqcount;


RIRQCode	rirqlow, rirqup, rirqmenu;

signed char pjoyx, pjoyy;
bool		pjoyb;

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
	music_play();
	
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


void gameirq_init(void)
{
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