#pragma once
#include "../global/vars.h"

#define DI_ReadKeyboard	( (void(__cdecl*)(uchar*)) 0x00475450 )
#define DI_ReadJoystick	( (long(__cdecl*)(long&, long&)) 0x004754B0 )
