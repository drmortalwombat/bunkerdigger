#include "display.h"

const char FontHiresData[] = {
	#embed ctm_chars "colorfont.ctm"
};

void disp_fill(char x, char y, char w, char h, char back)
{
	__assume(y < 25);
	__assume(x < 40);

	char * hp = Hires + 320 * y + x * 8;
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

	char * hp = Hires + 320 * y + x * 8;
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
