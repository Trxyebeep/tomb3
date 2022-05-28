#pragma once
#include "../global/vars.h"

#define GetJointAbsPosition	( (void(__cdecl*)(ITEM_INFO*, PHD_VECTOR*, long)) 0x00468250 )
#define TestCollision	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x00467E40 )
