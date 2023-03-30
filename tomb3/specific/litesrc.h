#pragma once
#include "../global/types.h"

void S_CalculateStaticLight(short adder);
void S_CalculateStaticMeshLight(long x, long y, long z, short shade, short shadeB, ROOM_INFO* ri);
short* calc_vertice_light(short* objptr, short* objptr1);
void S_CalculateLight(long x, long y, long z, short room_number, ITEM_LIGHT* il);

extern PHD_VECTOR LPos[3];
extern long LightPos[12];
extern long LightCol[12];
extern long smcr;
extern long smcg;
extern long smcb;
