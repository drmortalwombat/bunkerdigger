#include "window.h"
#include "tiles.h"
#include "gameirq.h"
#include <c64/keyboard.h>

char story_shown, story_pending;

void story_init(void)
{
	story_shown = 0;
	story_pending = 0;
}

const char * StoryMessageTexts[] = {
	"\x01IF YOU CAN READ THIS, WE MUST\n"
	"HAVE WON THE WAR.\n"
	"\x01WELL AT LEAST KIND OF.\n"
	"\x01THIS SHELTER WITH ITS ROOMS\n"
	"AND MINESHAFTS WILL BRING\n"
	"GLORY TO OUR NATION.\n"
	"\x01OTHER DWELLERS ARE IN\n"
	"HIBERNATION, READY TO BE\n"
	"AWOKEN.\n\x01\x01\x01",

	"\x01A LACK OF FRESH WATER WILL\n"
	"KILL YOUR DWELLERS\n\x01\x01\x01",

	"\x01WE NEED MORE GENERATORS TO\n"
	"KEEP UP WITH OUR DEMANDS.\n\x01\x01\x01",

	"\x01THERE IS A LACK OF VENTILATION\n"
	"IN THE LOWER FLOORS OF YOUR\n"
	"BUNKER.\n\x01\x01\x01",

	"\x01THIS ROCK IS TOO HARD FOR\n"
	"OUR SHOVELS AND PICKAXES.\n\n"
	"\x01WE MAY NEED SOMETHING MORE\n"
	"POWERFUL FOR THIS.\n\x01\x01\x01",

	"\x01THERE ARE NO MORE DWELLERS\n"
	"IN HIBERNATION.\n\n"
	"\x01WE SHOULD USE THE RADIO TO\n"
	"ATTRACT FUGITIVES FROM THE\n"
	"SURFACE TO JOIN OUR TEAM.\n\x01\x01\x01",

	"\x01WE PICKED UP A STRANGE RADIO\n"
	"MESSAGE. IT APPEARS A FACTION\n"
	"HAS SURVIVED ON A SECRET MOON\n"
	"BASE.\n\n"
	"\x01WE WILL TRY TO CONTACT THEM\n"
	"FOR MORE BLUEPRINTS\n\x01\x01\x01",

	"\x01THE GROUP ON THE MOON TURNED\n"
	"OUT TO BE THE ENEMY, THEY\n"
	"THREATEN US WITH A BUNKER\n"
	"BUSTER.\n\n"
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

bool window_check_close(char delay)
{
	for(char i=0; i<delay; i++)
	{
		keyb_poll();

		if (keyb_key == (KSCAN_QUAL_DOWN | KSCAN_SPACE))
			return true;
		vic_waitFrame();
	}
	return false;
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
			if (psp)
				*psp = 0x5d;
			
			for(char j=0; j<5; j++)
			{
				if (window_check_close(4))
					return;
				*sp = 0x5d;
				if (window_check_close(4))
					return;
				*sp = 0x00;
				if (window_check_close(4))
					return;
				*sp = 0x5d;
				if (window_check_close(4))
					return;
				*sp = 0xd1;
			}
		}
		else
		{
			*sp = 0xd1;
			for(char j=0; j<8; j++)
				wp[j] = 0xaa;

			if (window_check_close(1))
				return;

			const char * cp = FontHiresData + 8 * (c & 0x3f);

			for(char j=0; j<8; j++)
				wp[j] = cp[j];

			if (window_check_close(1))
				return;

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


static const char wmand[] = {
	0xff, 0xff, 0xff, 0xff, 0xf0, 0xf0, 0xf0, 0xf0,
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x0f,

	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
	0x0c, 0x03, 0x0c, 0x03, 0x0c, 0x03, 0x0c, 0x03,

	0xf0, 0xf0, 0xf0, 0xf0, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0xcc, 0x33, 0xcc, 0x33,
	0x0c, 0x03, 0x0c, 0x03, 0xcc, 0x33, 0xcc, 0x33,
};

static const char wmor[] = {
	0x00, 0x00, 0x00, 0x00, 0x05, 0x05, 0x04, 0x04,
	0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x50, 0x50, 0x10, 0x10,

	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,

	0x04, 0x04, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x10, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00
};

void window_mask(char * dp, char mi)
{
	for(char i=0; i<8; i++)
		dp[i] = (dp[i] & wmand[mi + i]) | wmor[mi + i];
}

void window_mask3(char * dp, char mi, char w)
{
	window_mask(dp, mi);
	dp += 8;
	mi += 8;

	__assume(w > 0);
	for(char i=0; i<w; i++)
	{
		window_mask(dp, mi);
		dp += 8;
	}
	mi += 8;
	window_mask(dp, mi);
}

void window_open(char x, char y, char w, char h)
{
	irqphase = IRQP_WINDOW;
	vic.spr_enable = 0x00;

	winX = x;
	winY = y;
	winW = w;
	winH = h;

	winP = HiresRow[winY] + 8 * winX;
	winS = Screen + 40 * winY + winX;

	char	*	wp = winP - 328;

	// Draw top border of window

	char	*	tp = wp;

	window_mask3(tp, 0, w);

	char w8 = w * 8;
	wp += 320;
	for(char i=0; i<h; i++)
	{
		tp = wp;

		// Draw left border


		window_mask(tp, 24);

		// Clear center area

		tp += 8;
		for(char k=0; k<w8; k++)
			tp[k] = 0x00;
		tp += w8;

		// Draw right border and shadow

		window_mask(tp, 32);

		wp += 320;
	}

	// Draw bottom border and shadow
	
	tp = wp;
	window_mask3(tp, 40, w);

	char	*	sp = Screen + 40 * winY + winX - 41;
	char	*	cp = Color + 40 * winY + winX - 41;

	__assume(w < 40);
	__assume(w > 0);

	for(signed char i=w+1; i>=0; i--)
		sp[i] = (sp[i] & 0x0f) | 0x10;

	sp += 40;
	cp += 40;
	for(char i=0; i<h; i++)
	{
		sp[0] = (sp[0] & 0x0f) | 0x10;
		for(char j=w; j>0; j--)
		{
			sp[j] = 0x5d;
			cp[j] = 0x00;
		}		
		sp[w + 1] = (sp[w + 1] & 0x0f) | 0x10;
		sp += 40;
		cp += 40;
	}

	for(signed char i=w+1; i>=0; i--)
		sp[i] = (sp[i] & 0x0f) | 0x10;

//	window_fill(0x00);
}

void window_close(void)
{
	tmapmode = TMMODE_REDRAW;
	irqphase = IRQP_MOVE_DIGGER;
}

