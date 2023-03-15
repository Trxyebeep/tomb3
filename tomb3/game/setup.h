#pragma once
#include "../global/vars.h"

void inject_setup(bool replace);

void GetAIPickups();
void GetCarriedItems();
void InitialiseLevelFlags();
void InitialiseGameFlags();
long InitialiseLevel(long level, long type);

#define BuildOutsideTable	( (void(__cdecl*)()) 0x004666C0 )
#define InitialiseObjects	( (void(__cdecl*)()) 0x00463BC0 )
