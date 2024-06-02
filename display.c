#include "display.h"

const char FontHiresData[] = {
	#embed ctm_chars "colorfont.ctm"
};

char FontHiresMask[512];

#pragma align(FontHiresData, 256)
#pragma align(FontHiresMask, 256)

__striped char * const HiresRow[25] = {
#for(i, 25)	Hires +  i * 320,
};

__striped char * const ScreenRow[25] = {
#for(i, 25)	Screen +  i * 40,
};

__striped char * const ColorRow[25] = {
#for(i, 25)	Color +  i * 40,
};

void disp_init(void)
{
	const char * sp = FontHiresData;
	char * mp = FontHiresMask;

	for(char i=0; i<64; i++)
	{
		char m = 0;
		for(char j=0; j<8; j++)
		{
			char c = sp[j] ^ 0xff;
			c = (c | (c << 1)) & 0xaa;
			c |= c >> 1;
			c |= c >> 2;
			mp[j] = ~c & ~m;
			m = c;
		}

		sp += 8;
		mp += 8;
	}
}

void disp_fill(char x, char y, char w, char h, char back)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = HiresRow[y] + x * 8;
	char * cp = ColorRow[y] + x;

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
	char * sp = ScreenRow[y] + x;
	char * cp = ColorRow[y] + x;

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
	char * sp = ScreenRow[y] + x;
	char * cp = ColorRow[y] + x;


	for(char i=0; i<h; i++)
		hp[i] = 0x96;
	for(char i=h; i<8; i++)
		hp[i] = 0xbe;
	sp[0] = back;
	cp[0] = color;
}

void disp_rbar(char x, char y, char w, char total, char area, char color)
{
	__assume(y < 25);
	__assume(x < 40);

	total >>= 2;
	area >>= 2;

	char * sp = ScreenRow[y] + x;
	char * cp = ColorRow[y] + x;
	for(char c=0; c<total; c++)
	{
		sp[c] = color;
		cp[c] = 1;
	}

	char * hp = HiresRow[y] + x * 8;


	char m = barmask[w & 3] ^ 0x55;
	
	w >>= 2;	

	char i = 0;
	char y = 0;
	while (i < w)
	{
		hp[y++] = 0x00;
		hp[y++] = 0xaa;
		hp[y++] = 0xff;
		hp[y++] = 0xaa;
		hp[y++] = 0xaa;
		hp[y++] = 0xaa;
		hp[y++] = 0xa2;
		hp[y++] = 0x00;
		i ++;
	}

	if (i < total)
	{
		hp[y++] = 0x00;
		hp[y++] = m;
		hp[y++] = 0xff;
		hp[y++] = m;
		hp[y++] = m;
		hp[y++] = m;
		hp[y++] = m & 0xf3;
		hp[y++] = 0x00;
		i ++;

		while (i < total)
		{
			hp[y++] = 0x00;
			hp[y++] = 0x55;
			hp[y++] = 0xff;
			hp[y++] = 0x55;
			hp[y++] = 0x55;
			hp[y++] = 0x55;
			hp[y++] = 0x51;
			hp[y++] = 0x00;
			i ++;
		}	
	}

	while (i < area)
	{
		for(char j=0; j<8; j++)
			hp[y++] = 0x00;
		i ++;
	}	
}

void disp_bar(char x, char y, char w, char back, char color)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = HiresRow[y] + x * 8;
	char * sp = ScreenRow[y] + x;
	char * cp = ColorRow[y] + x;

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

void disp_chars_msg(char y, const char * text, char n, char back, char color)
{
	__assume(y < 25);

	char * hp = HiresRow[y];
	char * sp = ScreenRow[y];

	for(char i=0; i<n; i++)
	{
		char c = text[i];
		if (c != ' ')
		{
			const char * shp = FontHiresData + 8 * c;
			const char * mhp = FontHiresMask + 8 * c;

			for(signed char j=7; j>=0; j--)
			{
				char c = shp[j] ^ 0xff;
				char m = mhp[j];
				hp[j] = (hp[j] & m) | c;
			}

			sp[i] = 0x1c;
		}
		hp += 8;
	}	
}


void disp_chars(char x, char y, const char * text, char n, char back, char color)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = HiresRow[y] + x * 8;
	char * sp = ScreenRow[y] + x;
	char * cp = ColorRow[y] + x;

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
	char * sp = ScreenRow[y] + x;
	char * cp = ColorRow[y] + x;

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

	const char * shp = FontHiresData + 8 * ch;
	#pragma unroll(full)
	for(char j=0; j<8; j++)
		hp[j] = shp[j];

	char * sp = ScreenRow[y];
	char * cp = ColorRow[y];

	sp[x] = color;
	cp[x] = back;
}

void disp_menu(char x, const char * text, char back, char color, char colork)
{
	__assume(x < 40);

	char * hp = Hires + 320 * 24 + x * 8;
	char * sp = Screen + 40 * 24 + x;
	char * cp = Color + 40 * 24 + x;

	char s = 0;
	for(char i=0; i<3; i++)
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
	for(char i=0; i<3; i++)
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
