#pragma once
#include "../global/vars.h"

void inject_setup(bool replace);

void GetAIPickups();
void GetCarriedItems();
void InitialiseLevelFlags();
void InitialiseGameFlags();
long InitialiseLevel(long level, long type);
void BuildOutsideTable();

#define InitialiseObjects	( (void(__cdecl*)()) 0x00463BC0 )
