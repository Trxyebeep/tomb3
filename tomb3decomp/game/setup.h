#pragma once
#include "../global/vars.h"

void inject_setup(bool replace);

void GetAIPickups();

#define dummyColFunc	( (void(__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x004837C0 )
