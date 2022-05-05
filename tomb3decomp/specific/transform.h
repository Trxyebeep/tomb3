#pragma once
#include "../global/vars.h"

void inject_transform(bool replace);

void InitZTable();

#define calc_object_vertices	( (short*(__cdecl*)(short*)) 0x0048D6E0 )
