#pragma once
#include "../global/vars.h"

void inject_di(bool replace);

void DI_ReadKeyboard(uchar* KeyMap);

#define DI_ReadJoystick	( (long(__cdecl*)(long&, long&)) 0x004754B0 )
#define DI_Finish	( (void(__cdecl*)()) 0x004756F0 )
