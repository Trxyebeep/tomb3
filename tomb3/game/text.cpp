#include "../tomb3/pch.h"
#include "text.h"
#include "../specific/option.h"
#include "../3dsystem/3d_gen.h"
#include "../3dsystem/scalespr.h"
#include "objects.h"
#include "../specific/frontend.h"
#include "health.h"
#include "../specific/output.h"
#include "camera.h"
#include "../tomb3/tomb3.h"

static char T_remapASCII[95] =
{
	0, 64, 66, 78, 77, 74, 78, 79, 69, 70, 92, 72, 63, 71, 62, 68, 52,
	53, 54, 55, 56, 57, 58, 59, 60, 61, 73, 73, 66, 74, 75, 65, 0, 0,
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 80, 76, 81, 97, 98, 77, 26, 27, 28, 29, 30,
	31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 100, 101, 102, 67
};

static char T_textSpacing[110] =
{
	14, 13, 14, 14, 13, 12, 15, 15, 9, 12, 15, 14, 17, 14, 15, 13, 15, 14, 11, 13, 13,
	15, 17, 14, 14, 13, 10, 11, 9, 10, 9, 9, 10, 11, 6, 7, 12, 7, 16, 11, 10, 11, 11,
	9, 8, 8, 11, 10, 15, 10, 10, 10, 12, 7, 10, 10, 10, 10, 10, 9, 10, 10, 5, 5, 5,
	11, 9, 10, 8, 6, 6, 7, 7, 3, 11, 8, 13, 16, 9, 4, 12, 12, 7, 5, 7, 7, 7, 7, 7, 7,
	7, 7, 16, 14, 14, 14, 14, 14, 14, 14, 14, 12, 14, 8, 8, 8, 8, 8, 8, 8
};

static TEXTSTRING T_textStrings[64];
static char T_theStrings[4096];
static short T_numStrings;

short T_GetStringLen(const char* string)
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

void T_AddOutline(TEXTSTRING* string, short unused, short colour, GOURAUD_OUTLINE* gourptr, ushort flags)
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

void T_AddBackground(TEXTSTRING* string, short xsize, short ysize, short x, short y, short z, short color, GOURAUD_FILL* gourptr, ushort flags)
{
	ulong h, v;

	h = GetTextScaleH(string->scaleH);
	v = GetTextScaleV(string->scaleV);

	if (string)
	{
		string->flags |= T_ADDBACKGROUND;
		string->bgndSizeX = xsize;
		string->bgndSizeY = ysize;
		string->bgndOffX = x;
		string->bgndOffY = y;
		string->bgndOffZ = z;
		string->bgndGour = gourptr;
		string->bgndColour = color;
		string->bgndflags = flags;
	}
}

ulong GetTextScaleH(ulong h)
{
	return GetRenderScale(h);
}

ulong GetTextScaleV(ulong v)
{
	return GetRenderScale(v);
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

	h = string->scaleH;
	v = string->scaleV;
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

	return width;
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

TEXTSTRING* T_Print(long x, long y, long z, const char* pStr)
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
	GOURAUD_FILL* bgndGour;
	char* pStr;
	ulong h, v, letter;
	long x, y, z, w, bX, bY, bW, bH, sprite;
	long sx, sy, sh, sv;

	h = string->scaleH;
	v = string->scaleV;
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
		x += (GetRenderWidthDownscaled() - w) / 2;
	else if (string->flags & T_RIGHTALIGN)
		x += GetRenderWidthDownscaled() - w;
	else if (string->flags & T_RIGHTJUSTIFY)
		x -= w;

	if (string->flags & T_CENTRE_V)
		y += GetRenderHeightDownscaled() / 2;
	else if (string->flags & T_BOTTOMALIGN)
		y += GetRenderHeightDownscaled();

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

		if (x > 0 && x < GetRenderWidthDownscaled() && y > 0 && y < GetRenderHeightDownscaled())
		{
			sx = GetTextScaleH(x);
			sy = GetTextScaleV(y);
			sh = GetTextScaleH(h);
			sv = GetTextScaleV(v);
			S_DrawScreenSprite2d(sx, sy, z, sh, sv, short(objects[ALPHABET].mesh_index + sprite), string->Colour, string->textflags);
		}

		if (letter == '(' || letter == ')' || letter == '$' || letter == '~')
			continue;

		if (letter >= '0' && letter <= '9')
		{
			x += (h * (12 - (12 - T_textSpacing[sprite]) / 2)) >> 16;
			continue;
		}

		x += (h * (string->letterSpacing + T_textSpacing[sprite])) >> 16;
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

	sx = GetTextScaleH(bX);
	sy = GetTextScaleV(bY);
	sh = GetTextScaleH(bW);
	sv = GetTextScaleV(bH);

	if (string->flags & T_ADDBACKGROUND)
	{
		if (string->bgndGour)
			bgndGour = string->bgndGour;
		else
			bgndGour = 0;

		S_DrawScreenFBox(sx, sy, string->bgndOffZ + z + 2, sh, sv, string->bgndColour, bgndGour, string->bgndflags);
	}

	if (string->flags & T_ADDOUTLINE)
	{
		if (tomb3.psx_boxes)
			S_DrawBorder(sx, sy, string->bgndOffZ + z + 2, sh, sv, (char)string->outlColour, string->outlGour, string->outlflags);
		else
			draw_border(sx, sy, 0, sh, sv);
	}
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
