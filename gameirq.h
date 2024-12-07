#ifndef GAMEIRQ_H
#define GAMEIRQ_H

// Current IRQ phase, the IRQ work is distributed over
// three frames
enum IRQPhase
{
	IRQP_MOVE_DIGGER,
	IRQP_UPDATE_SPRITE,
	IRQP_USER_INPUT,
	IRQP_WINDOW,
	IRQP_INTRO,
	
	NUM_IRQPHASE
};

extern IRQPhase			irqphase;
extern volatile char	irqcount;

// Init irq system
void gameirq_init(void);

// Hide digger sprites
void gameirq_hide(void);

// Show digger sprites
void gameirq_show(void);


#pragma compile("gameirq.c")

#endif

