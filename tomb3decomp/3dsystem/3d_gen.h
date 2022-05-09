#pragma once
#include "../global/vars.h"

void inject_3dgen(bool replace);

void phd_PutPolygons(short* objptr, long clip);
void phd_RotX(short angle);
void phd_RotY(short angle);
void phd_RotZ(short angle);
void phd_RotYXZ(short y, short x, short z);
void phd_RotYXZpack(long angles);
long phd_TranslateRel(long x, long y, long z);
void phd_TranslateAbs(long x, long y, long z);
void AlterFOV(short fov);
void phd_PushMatrix();
void phd_PushUnitMatrix();

__inline void phd_PopMatrix()
{
	phd_mxptr -= indices_count;
}

#define phd_LookAt	( (void(__cdecl*)(long, long, long, long, long, long, short)) 0x004011D0 )

#define InsertLine	( *(void(__cdecl**)(long, long, long, long, long, long, long)) 0x005A6AF8 )
#define InsertObjectGT4	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005A6A34 )
#define InsertObjectGT3	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005A6A30 )
#define InsertObjectG4	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005BAB0C )
#define InsertObjectG3	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005BAB50 )

#define InsertTrans8	( *(void(__cdecl**)(PHD_VBUF*, short)) 0x004E34A4 )
