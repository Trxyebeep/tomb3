#pragma once
#include "../global/vars.h"

void inject_invfunc(bool replace);

void InitColours();

#define Inv_AddItem	( (long(__cdecl*)(long)) 0x004378B0 )
#define Inv_RemoveItem	( (long(__cdecl*)(long)) 0x00438420 )
#define Inv_RequestItem	( (long(__cdecl*)(long)) 0x00438380 )
