#pragma once
#include "../global/vars.h"

void inject_transform(bool replace);

void InitZTable();
short* calc_object_vertices(short* objptr);
short* calc_object_verticesback(short* objptr);

#define calc_roomvert	( (short*(__cdecl*)(short*, long)) 0x0048D9B0 )
