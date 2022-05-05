#pragma once
#include "../global/vars.h"

void inject_3dgen(bool replace);

void phd_PutPolygons(short* objptr, long clip);

#define AlterFOV	( (void(__cdecl*)(short)) 0x00402030 )


#define InsertLine	( *(void(__cdecl**)(long, long, long, long, long, long, long)) 0x005A6AF8 )
#define InsertObjectGT4	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005A6A34 )
#define InsertObjectGT3	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005A6A30 )
#define InsertObjectG4	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005BAB0C )
#define InsertObjectG3	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005BAB50 )
