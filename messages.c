#include "messages.h"
#include "display.h"


Message	messages[32];
char	msg_head, msg_tail, msg_row, msg_delay;

char msg_buffer[33];

const char * msg_templates[] = {
	"d DEHYBERNATED",
	"d ARRIVED AT BUNKER",
	"d SUFFOCATED",
	"d DIED OF THIRST",
	"d KILLED",
	"b RESEARCHED",
	"MINE DEPLETED",
	"b COMPLETED",
	"b BLUEPRINT"
};

void msg_expand(MessageType msg, char param)
{
	const char * tp = msg_templates[msg];
	char i = 0;
	while (char c = *tp++)
	{
		switch (c)
		{
		case 'd':
			{
				const char * sp = digger_names + 5 * param;
				char j = 0;
				while (j < 5 && sp[j] != ' ')
					msg_buffer[i++] = sp[j++];
			break;
			}
		case 'b':
			{
				const char * sp = room_names + 10 * param;
				char j = 0;
				while (j < 10 && sp[j] != ' ')
					msg_buffer[i++] = sp[j++];
			break;
			}
		default:
			msg_buffer[i++] = c;
		}
	}
	while (i < 32)
		msg_buffer[i++] = ' ';
}

void msg_queue(MessageType msg, char param)
{
	messages[msg_head & 31].msg = msg;
	messages[msg_head & 31].param = param;
	msg_head++;
	if (msg_tail + 33 == msg_head)
		msg_tail++;
	msg_expand(msg, param);
	disp_chars(0, 23 - msg_row, msg_buffer, 32, VCOL_BLACK, VCOL_WHITE | 16 * VCOL_LT_GREY);	
	msg_row++;
	msg_delay = 100;
}

void msg_refresh(void)
{
	if (msg_delay)
	{
		char h = msg_head - msg_row;
		for(char i=0; i<msg_row; i++)
		{
			msg_expand(messages[h & 31].msg, messages[h & 31].param);
			disp_chars(0, 23 - i, msg_buffer, 32, VCOL_BLACK, VCOL_WHITE | 16 * VCOL_LT_GREY);	
			h++;
		}
	}
}


void msg_show_history(void)
{
	msg_delay = 100;
	msg_row = msg_head - msg_tail;
	if (msg_row > 24)
		msg_row = 24;
	msg_refresh();
}
