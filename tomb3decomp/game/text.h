#pragma once
#include "../global/vars.h"

void inject_text(bool replace);

short T_GetStringLen(char* string);
long T_RemovePrint(TEXTSTRING* textstring);
void T_BottomAlign(TEXTSTRING* string, short flag);
void T_RightAlign(TEXTSTRING* string, short flag);
void T_CentreV(TEXTSTRING* string, short flag);
void T_CentreH(TEXTSTRING* string, short flag);
void T_RemoveOutline(TEXTSTRING* string);
void T_AddOutline(TEXTSTRING* string, short unused, short colour, ushort* gourptr, ushort flags);
void T_RemoveBackground(TEXTSTRING* string);
void T_AddBackground(TEXTSTRING* string, short xsize, short ysize, short x, short y, short z, short color, ushort* gourptr, ushort flags);
ulong GetTextScaleH(ulong h);
ulong GetTextScaleV(ulong v);

#define T_Print	( (TEXTSTRING*(__cdecl*)(long, long, long, char*)) 0x0046ACA0 )
#define T_DrawText	( (void(__cdecl*)()) 0x0046B0F0 )
#define T_ChangeText	( (void(__cdecl*)(TEXTSTRING*, char*)) 0x0046AD90 )
