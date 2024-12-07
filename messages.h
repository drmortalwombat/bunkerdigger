#ifndef MESSAGES_H
#define MESSAGES_H

#include "display.h"

// Type of head up display messages
enum MessageType
{
	MSG_DIGGER_DEHYBERNATED,
	MSG_DIGGER_ARRIVED,
	MSG_DIGGER_SUFFOCATING,
	MSG_DIGGER_STARVING,
	MSG_DIGGER_KILLED,
	MSG_ROOM_RESEARCHED,
	MSG_MINE_DEPLETED,
	MSG_ROOM_COMPLETED,
	MSG_BLUEPRINT,
	MSG_DIGGER_ATTACKED,
	
	NUM_MESSAGES
};

// List of queued messages
struct Message
{
	MessageType	msg;
	char		param;
};

// Message queue
extern char 	msg_buffer[33];
extern Message	messages[32];
extern char		msg_head, msg_tail, msg_row, msg_delay;

// Init message system
void msg_init(void);

// Queue a message for display
void msg_queue(MessageType msg, char param);

// Refresh list of messages
void msg_refresh(void);

// Show full message history
void msg_show_history(void);


#pragma compile("messages.c")

#endif
