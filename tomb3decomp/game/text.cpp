#include "../tomb3/pch.h"
#include "text.h"
#include "../specific/option.h"
#include "../3dsystem/3d_gen.h"

short T_GetStringLen(char* string)
{
	short len;

	len = 1;

	while (*string++)
	{
		len++;

		if (len > 64)
			return 64;
	}

	return len;
}

long T_RemovePrint(TEXTSTRING* string)
{
	if (!string || !(string->flags & T_ACTIVE))
		return 0;

	string->flags &= ~T_ACTIVE;
	T_numStrings--;
	return 1;
}

void T_BottomAlign(TEXTSTRING* string, short flag)
{
	if (!string)
		return;

	if (flag)
		string->flags |= T_BOTTOMALIGN;
	else
		string->flags &= ~T_BOTTOMALIGN;
}

void T_RightAlign(TEXTSTRING* string, short flag)
{
	if (!string)
		return;

	if (flag)
		string->flags |= T_RIGHTALIGN;
	else
		string->flags &= ~T_RIGHTALIGN;
}

void T_CentreV(TEXTSTRING* string, short flag)
{
	if (!string)
		return;

	if (flag)
		string->flags |= T_CENTRE_V;
	else
		string->flags &= ~T_CENTRE_V;
}

void T_CentreH(TEXTSTRING* string, short flag)
{
	if (!string)
		return;

	if (flag)
		string->flags |= T_CENTRE_H;
	else
		string->flags &= ~T_CENTRE_H;
}

void T_RemoveOutline(TEXTSTRING* string)
{
	if (!string)
		return;

	string->flags &= ~T_ADDOUTLINE;
}

void T_AddOutline(TEXTSTRING* string, short unused, short colour, ushort* gourptr, ushort flags)
{
	if (!string)
		return;

	string->flags |= T_ADDOUTLINE;
	string->outlColour = colour;
	string->outlGour = gourptr;
	string->outlflags = flags;
}

void T_RemoveBackground(TEXTSTRING* string)
{
	if (!string)
		return;

	string->flags &= ~T_ADDBACKGROUND;
}

void T_AddBackground(TEXTSTRING* string, short xsize, short ysize, short x, short y, short z, short color, ushort* gourptr, ushort flags)
{
	ulong h, v;

	h = GetTextScaleH(string->scaleH);
	v = GetTextScaleV(string->scaleV);

	if (string)
	{
		string->flags |= T_ADDBACKGROUND;
		string->bgndSizeX = (xsize * h) >> 16;
		string->bgndSizeY = (ysize * v) >> 16;
		string->bgndOffZ = z;
		string->bgndGour = gourptr;
		string->bgndOffY = (y * v) >> 16;
		string->bgndOffX = (x * h) >> 16;
		string->bgndColour = color;
		string->bgndflags = flags;
	}
}

ulong GetTextScaleH(ulong h)
{
	long w;

	w = GetRenderWidth();

	if (w < 640)
		w = 640;

	return (h >> 8) * (((w << 16) / w) >> 8);
}

ulong GetTextScaleV(ulong v)
{
	long h;

	h = GetRenderHeight();

	if (h < 480)
		h = 480;

	return (v >> 8) * (((h << 16) / h) >> 8);
}

void draw_border(long x, long y, long z, long w, long h)
{
	long c1, c2;

	c1 = 0x1040;
	c2 = 0x4080C0;
	z = phd_znear + 0xC000;
	x -= 2;
	y -= 2;
	w += 4;
	h += 4;

	InsertLine(x - 1, y - 1, x + 2 + w, y - 1, z, c1, c1);	//todo: explain this
	InsertLine(x, y, x + w, y, z, c2, c2);
	InsertLine(x + 1, y + 1, x + w, y - 1, z, c1, c1);
	InsertLine(x - 1, y, x - 1, y + 2 + h, z, c1, c1);
	InsertLine(x, y, x, y + h, z, c2, c2);
	InsertLine(x + 1, y, x + 1, y + h - 1, z, c1, c1);
	InsertLine(x + w - 1, y, x + w - 1, y + h, z, c1, c1);
	InsertLine(x + w, y, x + w, y + h + 1, z, c2, c2);
	InsertLine(x + w + 1, y, x + w + 1, y + 2 + h, z, c1, c1);
	InsertLine(x - 1, y + h - 1, x + w + 1, y + h - 1, z, c1, c1);
	InsertLine(x, y + h, x + w + 1, y + h, z, c2, c2);
	InsertLine(x, y + h + 1, x + w + 2, y + h + 1, z, c1, c1);
}

long T_GetTextWidth(TEXTSTRING* string)
{
	char* pStr;
	ulong h, v, letter, width;

	h = GetTextScaleH(string->scaleH);
	v = GetTextScaleV(string->scaleV);
	pStr = string->string;
	letter = 0;
	width = 0;

	while (*pStr)
	{
		letter = *pStr++;

		if (letter == '\x11' || letter == '\x12')
		{
			width += 14;
			continue;
		}

		if (letter > '\x81' || letter > '\n' && letter < ' ' || letter == '(' || letter == ')' || letter == '$' || letter == '~')
			continue;

		if (letter == ' ')
		{
			if (h == 0x10000)
				width += string->wordSpacing;
			else
				width += width += (h * string->wordSpacing) >> 16;

			continue;
		}

		if (letter >= '\x7F')
		{
			if (h == 0x10000)
				width += 16;
			else
				width += (16 * h) >> 16;

			continue;
		}

		if (letter >= '\v')
			letter = T_remapASCII[letter - 32];
		else
			letter += 81;

		if (letter < '0' || letter > '9')
		{
			if (h == 0x10000)
				width += string->letterSpacing + T_textSpacing[letter];
			else
				width += (h * string->letterSpacing + T_textSpacing[letter]) >> 16;
		}
		else
			width += (12 * h) >> 16;
	}

	return (width - string->letterSpacing) & 0xFFFE;
}

void inject_text(bool replace)
{
	INJECT(0x0046B0C0, T_GetStringLen, replace);
	INJECT(0x0046B090, T_RemovePrint, replace);
	INJECT(0x0046AF40, T_BottomAlign, replace);
	INJECT(0x0046AF20, T_RightAlign, replace);
	INJECT(0x0046AF00, T_CentreV, replace);
	INJECT(0x0046AEE0, T_CentreH, replace);
	INJECT(0x0046AED0, T_RemoveOutline, replace);
	INJECT(0x0046AEA0, T_AddOutline, replace);
	INJECT(0x0046AE90, T_RemoveBackground, replace);
	INJECT(0x0046AE00, T_AddBackground, replace);
	INJECT(0x0046B6F0, GetTextScaleH, replace);
	INJECT(0x0046B720, GetTextScaleV, replace);
	INJECT(0x0046B120, draw_border, replace);
	INJECT(0x0046AF60, T_GetTextWidth, replace);
}
