#ifndef MESSAGES_H
#define MESSAGES_H

#include "display.h"

enum MessageType
{
	MSG_DIGGER_DEHYBERNATED,
	MSG_DIGGER_SUFFOCATED,
	MSG_DIGGER_STARVED,
	MSG_DIGGER_KILLED,
	MSG_ROOM_RESEARCHED,
	MSG_MINE_DEPLETED,

	NUM_MESSAGES
};

struct Message
{
	MessageType	msg;
	char		param;
};

extern Message	messages[32];
extern char		msg_head, msg_tail, msg_row, msg_delay;

void msg_queue(MessageType msg, char param);

void msg_show_history(void);


#pragma compile("messages.c")

#endif
