#include "window.h"
#include "tiles.h"

char story_shown, story_pending;

void story_init(void)
{
	story_shown = 0;
	story_pending = 0;
}

const char * StoryMessageTexts[] = {
	"\x01AFTER THE FINAL WAR YOU FIND\n"
	"YOURSELF IN AN UNDERGROUND\n"
	"SHELTER, TRYING TO SURVIVE.\n\n"
	"\x01OTHER DWELLERS ARE IN\n"
	"HIBERNATION, READY TO BE\n"
	"AWOKEN.\n\x01\x01\x01",

	"\x01THERE ARE NO MORE DWELLERS\n"
	"IN HIBERNATION.\n\n"
	"\x01WE SHOULD USE THE RADIO TO\n"
	"ATTRACT FUGITIVES FROM THE\n"
	"SURFACE TO JOIN OUR TEAM.\n\x01\x01\x01",

	"\x01WE PICKED UP A STRANGE RADIO\n"
	"MESSAGE. IT APPEARS THE\n"
	"ENEMY HAS SURVIVED ON A\n"
	"SECRET MOON BASE.\n\n"
	"\x01WE MUST STOP THEM, BEFORE\n"
	"THEY DESTROY US COMPLETELY.\n\x01\x01\x01",

	"\x01WE SUCCESSFULLY DESTROYED THE\n"
	"MOON.\n\n"
	"\x01UNFORTUNATELY PARTS OF IT WILL\n"
	"HIT THE SURFACE AND ANNIHILATE\n"
	"THIS BUNKER.\n\n"
	"\x01 WE NEED A BIGGER ROCKET\n\x01\x01\x01",


};

bool story_messages(void)
{
	char m = story_pending & ~story_shown;
	if (m)
	{
		StoryMessages i = 0;
		while (!(m & 1))
		{
			m >>= 1;
			i++;
		}

		story_shown |= 1 << i;

		window_open(4, 7, 30, 10);
		window_print(StoryMessageTexts[i]);
		window_close();

		return true;
	}
	else
		return false;
}

char		winX, winY, winW, winH;
char	*	winP, * winS;

void window_fill(char pat)
{
	char	*	wp = winP;
	char		w = 8 * winW;
	
	for(char y=0; y<winH; y++)
	{
		for(char x=0; x<w; x++)
			wp[x] = pat;

		wp += 320;
	}
}


void window_color_rect(char x, char y, char w, char h, char color)
{
	char	*	cp = Screen + 40 * (winY + y) + (winX + x);

	for(char i=0; i<h; i++)
	{
		for(char j=0; j<w; j++)
			cp[j] = color;
		cp += 40;
	}
}

static inline char * winpos(char x, char y)
{
	__assume(x < 40);
	__assume(y < 24);

	return winP + 8 * (40 * y + x);
}

void window_fill_rect(char x, char y, char w, char h, char pat)
{
	char	*	wp = winpos(x, y);
	char		bw = 8 * w;
	
	for(char i=0; i<h; i++)
	{
		for(char j=0; j<bw; j++)
			wp[j] = pat;
		wp += 320;
	}
}

void window_scroll(void)
{
	char	*	wp = winP;
	char		w = 8 * winW;
	
	for(char y=1; y<winH; y++)
	{
		char	*	hp = wp + 320;
		for(char x=0; x<w; x++)
			wp[x] = hp[x];

		wp = hp;
	}
	for(char x=0; x<w; x++)
		wp[x] = 0;
}


void window_print(const char * text)
{
	char	*	wlp = winP;
	char	*	slp = winS;

	char	*	wp = wlp;
	char	*	sp = slp;
	char	*	psp = nullptr;

	while (char	c = *text++)
	{
		if (c == '\n')
		{			
			wlp += 320; wp = wlp;
			slp += 40;  sp = slp;
		}
		else if (c == 1)
		{
			*sp = 0xd1;
			for(char j=0; j<8; j++)
				wp[j] = 0xaa;
			for(char j=0; j<5; j++)
			{
				vic_waitFrames(4);			
				*sp = 0x5d;
				vic_waitFrames(4);			
				*sp = 0x00;
				vic_waitFrames(4);			
				*sp = 0x5d;
				vic_waitFrames(4);			
				*sp = 0xd1;
			}
		}
		else
		{
			*sp = 0xd1;
			for(char j=0; j<8; j++)
				wp[j] = 0xaa;
			vic_waitFrame();			

			const char * cp = FontHiresData + 8 * (c & 0x3f);

			for(char j=0; j<8; j++)
				wp[j] = cp[j];
			vic_waitFrame();
			if (psp)
				*psp = 0x5d;
			psp = sp;
			wp += 8;
			sp ++;
		}
	}
}

char window_write(char x, char y, const char * text)
{
	char	*	wp = winpos(x, y);

	char 	i = 0;
	char	c = text[i];
	while (c)
	{
		const char * cp = FontHiresData + 8 * (c & 0x3f);

		for(char j=0; j<8; j++)
			wp[j] = cp[j];

		wp += 8;
		i++;
		c = text[i];
	}

	return x + i;
}

char window_write_uint(char x, char y, unsigned u)
{
	char	buffer[10];
	utoa(u, buffer, 10);
	return window_write(x, y, buffer);
}


void window_open(char x, char y, char w, char h)
{
	vic.spr_enable = 0x00;

	winX = x;
	winY = y;
	winW = w;
	winH = h;

	winP = Hires + 320 * winY + 8 * winX;
	winS = Screen + 40 * winY + winX;

	char	*	wp = winP - 328;

	// Draw top border of window

	char	*	tp = wp;
	tp[4] = (tp[4] & 0xf0) | 0x05;
	tp[5] = (tp[5] & 0xf0) | 0x05;
	tp[6] = (tp[6] & 0xf0) | 0x04;
	tp[7] = (tp[7] & 0xf0) | 0x04;

	tp += 8;
	for(char i=0; i<w; i++)
	{
		tp[4] = 0x55;
		tp[5] = 0x55;
		tp[6] = 0x00;
		tp[7] = 0x00;
		tp += 8;
	}

	tp[4] = (tp[4] & 0x0f) | 0x50;
	tp[5] = (tp[5] & 0x0f) | 0x50;
	tp[6] = (tp[6] & 0x0f) | 0x10;
	tp[7] = (tp[7] & 0x0f) | 0x10;

	wp += 320;
	for(char i=0; i<h; i++)
	{
		tp = wp;

		// Draw left border

		for(char j=0; j<8; j++)
			tp[j] = (tp[j] & 0xf0) | 0x04;

		// Clear center area

		tp += 8;
		for(char k=0; k<w; k++)
		{
			for(char j=0; j<8; j++)
				tp[j] = 0x00;
			tp += 8;
		}

		// Draw right border and shadow

		for(char j=0; j<8; j+=2)
		{
			tp[j + 0] = (tp[j + 0] & 0x0c) | 0x10;
			tp[j + 1] = (tp[j + 1] & 0x03) | 0x10;
		}

		wp += 320;
	}

	// Draw bottom border and shadow
	
	tp = wp;
	tp[0] = (tp[0] & 0xf0) | 0x04;
	tp[1] = (tp[1] & 0xf0) | 0x04;
	tp[2] = (tp[2] & 0xf0) | 0x05;
	tp[3] = (tp[3] & 0xf0) | 0x05;

	tp += 8;
	for(char i=0; i<w; i++)
	{
		tp[0] = 0x00;
		tp[1] = 0x00;
		tp[2] = 0x55;
		tp[3] = 0x55;
		tp[4] &= 0xcc;
		tp[5] &= 0x33;
		tp[6] &= 0xcc;
		tp[7] &= 0x33;
		tp += 8;
	}

	tp[0] = (tp[0] & 0x0c) | 0x10;
	tp[1] = (tp[1] & 0x03) | 0x10;
	tp[2] = (tp[2] & 0x0c) | 0x50;
	tp[3] = (tp[3] & 0x03) | 0x50;
	tp[4] &= 0xcc;
	tp[5] &= 0x33;
	tp[6] &= 0xcc;
	tp[7] &= 0x33;

	char	*	sp = Screen + 40 * winY + winX - 41;
	char	*	cp = Color + 40 * winY + winX - 41;

	for(char i=0; i<w+2; i++)
		sp[i] = (sp[i] & 0x0f) | 0x10;

	sp += 40;
	cp += 40;
	for(char i=0; i<h; i++)
	{
		sp[0] = (sp[0] & 0x0f) | 0x10;
		for(char j=0; j<w; j++)
		{
			sp[j + 1] = 0x5d;
			cp[j + 1] = 0x00;
		}		
		sp[w + 1] = (sp[w + 1] & 0x0f) | 0x10;
		sp += 40;
		cp += 40;
	}

	for(char i=0; i<w+2; i++)
		sp[i] = (sp[i] & 0x0f) | 0x10;

	window_fill(0x00);
}

void window_close(void)
{
	tmapmode = TMMODE_REDRAW;
}

