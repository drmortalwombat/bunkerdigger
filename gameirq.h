#ifndef GAMEIRQ_H
#define GAMEIRQ_H

enum IRQPhase
{
	IRQP_MOVE_DIGGER,
	IRQP_MOVE_ENEMY,
	IRQP_UPDATE_SPRITE,
	IRQP_USER_INPUT,
	IRQP_WINDOW,
	
	NUM_IRQPHASE
};

extern IRQPhase			irqphase;
extern volatile char	irqcount;


void gameirq_init(void);

#pragma compile("gameirq.c")

#endif

