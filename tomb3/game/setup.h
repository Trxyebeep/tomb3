#pragma once
#include "../global/vars.h"

void inject_setup(bool replace);

void GetAIPickups();
void InitialiseLevelFlags();
void InitialiseGameFlags();

#define dummyColFunc	( (void(__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x004837C0 )
#define BuildOutsideTable	( (void(__cdecl*)()) 0x004666C0 )
#define InitialiseObjects	( (void(__cdecl*)()) 0x00463BC0 )
#define InitialiseLevel	( (long(__cdecl*)(long, long)) 0x004638F0 )
