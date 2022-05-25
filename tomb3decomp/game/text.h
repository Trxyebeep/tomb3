#pragma once
#include "../global/vars.h"

void inject_text(bool replace);

short T_GetStringLen(char* string);
long T_RemovePrint(TEXTSTRING* textstring);

#define T_Print	( (TEXTSTRING*(__cdecl*)(long, long, long, char*)) 0x0046ACA0 )
#define T_RightAlign	( (void(__cdecl*)(TEXTSTRING*, short)) 0x0046AF20 )
#define T_DrawText	( (void(__cdecl*)()) 0x0046B0F0 )
#define T_ChangeText	( (void(__cdecl*)(TEXTSTRING*, char*)) 0x0046AD90 )
#define T_BottomAlign	( (void(__cdecl*)(TEXTSTRING*, short)) 0x0046AF40 )
