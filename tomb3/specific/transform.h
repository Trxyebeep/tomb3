#pragma once
#include "../global/types.h"

void InitZTable();
short* calc_object_vertices(short* objptr);
short* calc_object_verticesback(short* objptr);
short* calc_roomvert(short* objptr, long far_clip);

extern float ZTable[40960];
