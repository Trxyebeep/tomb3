#pragma once
#include "../global/vars.h"

void inject_litesrc(bool replace);

void S_CalculateStaticLight(short adder);
void S_CalculateStaticMeshLight(long x, long y, long z, short shade, short shadeB, ROOM_INFO* ri);

#define calc_vertice_light	( (short*(__cdecl*)(short*, short*)) 0x004866E0 )
