#pragma once
#include "../global/types.h"

short T_GetStringLen(const char* string);
long T_RemovePrint(TEXTSTRING* textstring);
void T_BottomAlign(TEXTSTRING* string, short flag);
void T_RightAlign(TEXTSTRING* string, short flag);
void T_CentreV(TEXTSTRING* string, short flag);
void T_CentreH(TEXTSTRING* string, short flag);
void T_RemoveOutline(TEXTSTRING* string);
void T_AddOutline(TEXTSTRING* string, short unused, short colour, GOURAUD_OUTLINE* gourptr, ushort flags);
void T_RemoveBackground(TEXTSTRING* string);
void T_AddBackground(TEXTSTRING* string, short xsize, short ysize, short x, short y, short z, short color, GOURAUD_FILL* gourptr, ushort flags);
ulong GetTextScaleH(ulong h);
ulong GetTextScaleV(ulong v);
void draw_border(long x, long y, long z, long w, long h);
long T_GetTextWidth(TEXTSTRING* string);
void T_FlashText(TEXTSTRING* string, short flash, short rate);
void T_ChangeText(TEXTSTRING* string, char* pStr);
TEXTSTRING* T_Print(long x, long y, long z, const char* pStr);
void T_InitPrint();
void T_DrawThisText(TEXTSTRING* string);
void T_DrawText();
