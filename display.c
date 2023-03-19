#include "display.h"

const char FontHiresData[] = {
	#embed ctm_chars "colorfont.ctm"
};

__striped char * const HiresRow[25] = {
	Hires +  0 * 320, Hires +  1 * 320, Hires +  2 * 320, Hires +  3 * 320, Hires +  4 * 320,
	Hires +  5 * 320, Hires +  6 * 320, Hires +  7 * 320, Hires +  8 * 320, Hires +  9 * 320,
	Hires + 10 * 320, Hires + 11 * 320, Hires + 12 * 320, Hires + 13 * 320, Hires + 14 * 320,
	Hires + 15 * 320, Hires + 16 * 320, Hires + 17 * 320, Hires + 18 * 320, Hires + 19 * 320,
	Hires + 20 * 320, Hires + 21 * 320, Hires + 22 * 320, Hires + 23 * 320, Hires + 24 * 320,
};

void disp_fill(char x, char y, char w, char h, char back)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = HiresRow[y] + x * 8;
	char * cp = Color + 40 * y + x;

	for(char i=0; i<h; i++)
	{
		char * lp = hp;
		for(char j=0; j<w; j++)
		{
			#pragma unroll(full)
			for(char i=0; i<8; i++)
				lp[i] = 0xaa;
			cp[j] = back;
			lp += 8;
		}
		hp += 320;
	}
}

void disp_text(char x, char y, const char * text, char back, char color)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = HiresRow[y] + x * 8;
	char * sp = Screen + 40 * y + x;
	char * cp = Color + 40 * y + x;

	char i = 0;
	while (char c = text[i])
	{
		const char * shp = FontHiresData + 8 * c;
		#pragma unroll(full)
		for(char j=0; j<8; j++)
			hp[j] = shp[j];

		sp[i] = color;
		cp[i] = back;
		hp += 8;

		i++;
	}
}

const char barmask[4] = {0x00, 0xc0, 0xf0, 0xfc};

void disp_vbar(char x, char y, char h, char back, char color)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = HiresRow[y] + x * 8;
	char * sp = Screen + 40 * y + x;
	char * cp = Color + 40 * y + x;


	for(char i=0; i<h; i++)
		hp[i] = 0x96;
	for(char i=h; i<8; i++)
		hp[i] = 0xbe;
	sp[0] = back;
	cp[0] = color;
}

void disp_rbar(char x, char y, char w, char total, char color)
{
	__assume(y < 25);
	__assume(x < 40);

	total >>= 2;

	char * sp = Screen + 40 * y + x;
	for(char c=0; c<total; c++)
		sp[c] = color;

	char * hp = HiresRow[y] + x * 8;


	char m = barmask[w & 3] ^ 0x55;
	
	w >>= 2;	

	char i = 0;
	while (i < w)
	{
		hp[0] = 0x00;
		for(char j=1; j<7; j++)
			hp[j] = 0xaa;
		hp[7] = 0x00;
		hp += 8;		
		i ++;
	}

	if (i < total)
	{
		hp[0] = 0x00;
		for(char j=1; j<7; j++)
			hp[j] = m;
		hp[7] = 0x00;
		hp += 8;		
		i ++;

		while (i < total)
		{
			hp[0] = 0x00;
			for(char j=1; j<7; j++)
				hp[j] = 0x55;
			hp[7] = 0x00;
			hp += 8;		
			i ++;
		}	
	}
}

void disp_bar(char x, char y, char w, char back, char color)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = HiresRow[y] + x * 8;
	char * sp = Screen + 40 * y + x;
	char * cp = Color + 40 * y + x;

	char i = 0;
	while (i + 4 <= w)
	{
		hp[0] = 0x00;
		for(char j=1; j<7; j++)
			hp[j] = 0xff;
		hp[7] = 0x00;
		hp += 8;		
		i += 4;
	}

	char m = barmask[w & 3];
	hp[0] = 0x00;
	for(char j=1; j<7; j++)
		hp[j] = m;
	hp[7] = 0x00;
	hp += 8;		
	i += 4;

	while (i < 12)
	{
		for(char j=0; j<8; j++)
			hp[j] = 0x00;
		hp += 8;		
		i += 4;
	}
}

void disp_bbar(char x, char y, char w)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = HiresRow[y] + x * 8;

	for(char i=1; i<7; i++)
		hp[i] &= 0xfc;
	hp += 8;

	char m0 = barmask[w & 3] & 0x55;
	char m1 = 0x00;
	hp[1] = 0x00;
	hp[9] = 0x00;
	hp[6] = 0x00;
	hp[14] = 0x00;

	if (w >= 8)
	{
		m0 = 0x55; 
		m1 = 0x55;
	}
	else if (w >= 4)
	{
		m1 = m0;
		m0 = 0x55;
	}

	hp[2] = m0;
	hp[3] = m0;		
	hp[4] = m0;		
	hp[5] = m0;		

	hp[10] = m1;
	hp[11] = m1;		
	hp[12] = m1;		
	hp[13] = m1;		

	hp += 16;
	for(char i=1; i<7; i++)
		hp[i] &= 0x3f;
}


void disp_chars(char x, char y, const char * text, char n, char back, char color)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = HiresRow[y] + x * 8;
	char * sp = Screen + 40 * y + x;
	char * cp = Color + 40 * y + x;

	for(char i=0; i<n; i++)
	{
		const char * shp = FontHiresData + 8 * text[i];
		#pragma unroll(full)
		for(char j=0; j<8; j++)
			hp[j] = shp[j];

		sp[i] = color;
		cp[i] = back;
		hp += 8;
	}	
}

void disp_space(char x, char y, char n, char back, char color)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = HiresRow[y] + x * 8;
	char * sp = Screen + 40 * y + x;
	char * cp = Color + 40 * y + x;

	for(char i=0; i<n; i++)
	{
		#pragma unroll(full)
		for(char j=0; j<8; j++)
			hp[j] = 0;

		sp[i] = color;
		cp[i] = back;
		hp += 8;
	}		
}

void disp_char(char x, char y, char ch, char back, char color)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = HiresRow[y] + x * 8;
	char * sp = Screen + 40 * y + x;
	char * cp = Color + 40 * y + x;

	const char * shp = FontHiresData + 8 * ch;
	#pragma unroll(full)
	for(char j=0; j<8; j++)
		hp[j] = shp[j];

	sp[0] = color;
	cp[0] = back;
}

void disp_menu(char x, const char * text, char back, char color, char colork)
{
	__assume(x < 40);

	char * hp = Hires + 320 * 24 + x * 8;
	char * sp = Screen + 40 * 24 + x;
	char * cp = Color + 40 * 24 + x;

	char s = 0;
	for(char i=0; i<4; i++)
	{
		char c = text[s++];
		if (c == S'_')
		{
			sp[i] = colork;
			c = text[s++];
		}
		else
			sp[i] = color;
		cp[i] = back;			

		const char * shp = FontHiresData + 8 * c;
		#pragma unroll(full)
		for(char j=0; j<8; j++)
			hp[j] = shp[j];

		hp += 8;
	}	
}

void disp_menu_color(char x, const char * text, char back, char color, char colork)
{
	__assume(x < 40);

	char * sp = Screen + 40 * 24 + x;
	char * cp = Color + 40 * 24 + x;

	char s = 0;
	for(char i=0; i<4; i++)
	{
		char c = text[s++];
		if (c == S'_')
		{
			sp[i] = colork;
			c = text[s++];
		}
		else
			sp[i] = color;
		cp[i] = back;			
	}
}
