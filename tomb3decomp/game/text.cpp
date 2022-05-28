#include "../tomb3/pch.h"
#include "text.h"
#include "../specific/option.h"
#include "../3dsystem/3d_gen.h"
#include "../3dsystem/scalespr.h"
#include "objects.h"
#include "../specific/frontend.h"
#include "health.h"

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

	InsertLine(x - 1, y - 1, x + w + 2, y - 1, z, c1, c1);
	InsertLine(x, y, x + w, y, z, c2, c2);
	InsertLine(x + 1, y + 1, x + w, y + 1, z, c1, c1);
	InsertLine(x - 1, y, x - 1, y + h + 2, z, c1, c1);
	InsertLine(x, y, x, y + h, z, c2, c2);
	InsertLine(x + 1, y, x + 1, y + h - 1, z, c1, c1);
	InsertLine(x + w - 1, y, x + w - 1, y + h, z, c1, c1);
	InsertLine(x + w, y, x + w, y + h + 1, z, c2, c2);
	InsertLine(x + w + 1, y, x + w + 1, y + h + 2, z, c1, c1);
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

void T_FlashText(TEXTSTRING* string, short flash, short rate)
{
	if (!string)
		return;

	if (flash)
	{
		string->flags |= T_FLASH;
		string->flashRate = rate;
		string->flashCount = rate;
	}
	else
		string->flags &= ~T_FLASH;
}

void T_ChangeText(TEXTSTRING* string, char* pStr)
{
	if (!pStr || !string || !(string->flags & T_ACTIVE))
		return;

	if (T_GetStringLen(pStr) > 64)
		pStr[63] = 0;

	memcpy(string->string, pStr, 64);
}

TEXTSTRING* T_Print(long x, long y, long z, char* pStr)
{
	TEXTSTRING* string;
	long num, length;

	if (!pStr || T_numStrings >= 64)
		return 0;

	string = T_textStrings;
	num = 0;

	for (num = 0; num < 64; num++)
	{
		if (!(string->flags & T_ACTIVE))
			break;

		string++;
	}

	if (num >= 64)
		return 0;

	length = T_GetStringLen(pStr);

	if (length >= 64)
		length = 63;

	string->xpos = (short)x;
	string->ypos = (short)y;
	string->zpos = 0;
	string->Colour = (short)z;
	string->scaleV = 0x10000;
	string->scaleH = 0x10000;
	string->letterSpacing = 0;
	string->string = &T_theStrings[64 * num];
	string->wordSpacing = 6;
	memcpy(string->string, pStr, length + 1);
	T_numStrings++;
	string->textflags = 0;
	string->outlflags = 0;
	string->bgndflags = 0;
	string->bgndSizeX = 0;
	string->bgndSizeY = 0;
	string->bgndOffX = 0;
	string->bgndOffY = 0;
	string->bgndOffZ = 0;
	string->flags = T_ACTIVE;
	return string;
}

void T_InitPrint()
{
	TEXTSTRING* string;

	DisplayModeInfo(0);
	string = T_textStrings;

	for (int i = 0; i < 64; i++)
	{
		string->flags = 0;
		string++;
	}

	T_numStrings = 0;
}

void T_DrawThisText(TEXTSTRING* string)
{
	ushort* bgndGour;
	char* pStr;
	ulong h, v, letter;
	long x, y, z, w, bX, bY, bW, bH, sprite;

	h = GetTextScaleH(string->scaleH);
	v = GetTextScaleV(string->scaleV);
	bW = 0;
	bH = 0;
	
	if (string->flags & T_FLASH)
	{
		string->flashCount -= (short)camera.number_frames;

		if (string->flashCount <= -string->flashRate)
			string->flashCount = string->flashRate;
		else if (string->flashCount < 0)
			return;
	}

	x = string->xpos;
	y = string->ypos;
	z = string->zpos;
	pStr = string->string;
	w = T_GetTextWidth(string);

	if (string->flags & T_CENTRE_H)
		x += (GetRenderWidth() - w) / 2;
	else if (string->flags & T_RIGHTALIGN)
		x += GetRenderWidth() - w;
	else if (string->flags & T_RIGHTJUSTIFY)
		x -= w;

	if (string->flags & T_CENTRE_V)
		y += GetRenderHeight() / 2;
	else if (string->flags & T_BOTTOMALIGN)
		y += GetRenderHeight();

	bX = x + string->bgndOffX - ((2 * h) >> 16);
	bY = y + string->bgndOffY - ((4 * v) >> 16) - ((11 * v) >> 16);
	letter = 0;

	while (*pStr)
	{
		letter = *pStr++;

		if (letter > '\x12' && letter < ' ')
			continue;

		if (letter == ' ')
		{
			x += (h * string->wordSpacing) >> 16;
			continue;
		}

		if (letter >= '\x7F')
		{
			if (letter <= '\x81')
				x += (16 * h) >> 16;

			continue;
		}

		if (letter < '\v')
			sprite = letter + 81;
		else if (letter <= '\x12')
			sprite = letter + 91;
		else
			sprite = T_remapASCII[letter - 32];

		if (letter >= '0' && letter <= '9')
			x += (h * ((12 - T_textSpacing[sprite]) / 2)) >> 16;

		if (x > 0 && x < GetRenderWidth() && y > 0 && y < GetRenderHeight())
			S_DrawScreenSprite2d(x, y, z, h, v, short(objects[ALPHABET].mesh_index + sprite), string->Colour, string->textflags);

		if (letter == '(' || letter == ')' || letter == '$' || letter == '~')
			continue;

		if (letter >= '0' && letter <= '9')
		{
			x += (h * (12 - (12 - T_textSpacing[sprite]) / 2)) >> 16;
			continue;
		}

		if (h == 0x10000)
		{
			if (sprite == 108 || sprite == 109)
				x += 14;
			else
				x += string->letterSpacing + T_textSpacing[sprite];
		}
		else
			x += h * (string->letterSpacing + T_textSpacing[sprite]);
	}

	if (string->flags & T_ADDBACKGROUND || string->flags & T_ADDOUTLINE)
	{
		if (string->bgndSizeX)
		{
			bX += w / 2;
			w = string->bgndSizeX;
			bX -= w / 2;
		}

		bW = w + 4;

		if (string->bgndSizeY)
			bH = string->bgndSizeY - 2;
		else
			bH = (16 * v) >> 16;
	}

	if (string->flags & T_ADDBACKGROUND)
	{
		if (string->bgndGour)
			bgndGour = string->bgndGour;
		else
			bgndGour = 0;

		S_DrawScreenFBox(bX, bY, string->bgndOffZ + z + 2, bW, bH, string->bgndColour, bgndGour, string->bgndflags);
	}

	if (string->flags & T_ADDOUTLINE)
		draw_border(bX, bY, 0, bW, bH);
}

void T_DrawText()
{
	TEXTSTRING* string;

	string = T_textStrings;

	for (int i = 0; i < 64; i++)
	{
		if (string && string->flags & T_ACTIVE)
			T_DrawThisText(string);

		string++;
	}
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
	INJECT(0x0046ADD0, T_FlashText, replace);
	INJECT(0x0046AD90, T_ChangeText, replace);
	INJECT(0x0046ACA0, T_Print, replace);
	INJECT(0x0046AC70, T_InitPrint, replace);
	INJECT(0x0046B340, T_DrawThisText, replace);
	INJECT(0x0046B0F0, T_DrawText, replace);
}
