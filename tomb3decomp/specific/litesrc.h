#pragma once
#include "../global/vars.h"

void inject_litesrc(bool replace);

void S_CalculateStaticLight(short adder);
void S_CalculateStaticMeshLight(long x, long y, long z, short shade, short shadeB, ROOM_INFO* ri);
short* calc_vertice_light(short* objptr, short* objptr1);
