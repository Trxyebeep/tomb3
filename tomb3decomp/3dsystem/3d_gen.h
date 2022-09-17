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
void SetZNear(long znear);
void SetZFar(long zfar);
void S_InsertRoom(short* objptr, long out);
short* calc_back_light(short* objptr);
void S_InsertBackground(short* objptr);
void phd_InitWindow(long x, long y, long w, long h, long znear, long zfar, long fov, long sw, long sh);
void phd_InitPolyList();
void do_quickysorty(long left, long right, long* list);

__inline void phd_PopMatrix()
{
	phd_mxptr -= indices_count;
}

#define phd_LookAt	( (void(__cdecl*)(long, long, long, long, long, long, short)) 0x004011D0 )
#define phd_SortPolyList	( (void(__cdecl*)(long, long*)) 0x00401F20 )
#define phd_GetVectorAngles	( (void(__cdecl*)(long, long, long, short*)) 0x004012D0 )
#define phd_GenerateW2V	( (void(__cdecl*)(PHD_3DPOS*)) 0x00401000 )

#define InsertLine	( *(void(__cdecl**)(long x1, long y1, long x2, long y2, long z, long c0, long c1)) 0x005A6AF8 )
#define InsertObjectGT4	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005A6A34 )
#define InsertObjectGT3	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005A6A30 )
#define InsertObjectG4	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005BAB0C )
#define InsertObjectG3	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005BAB50 )
#define RoomInsertObjectGT3	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005CBE44 )
#define RoomInsertObjectGT4	( *(short*(__cdecl**)(short*, long, sort_type)) 0x005CBE40 )
#define InsertFlatRect	( *(void(__cdecl**)(long, long, long, long, long, long)) 0x004F6D5C )
#define InsertTrans8	( *(void(__cdecl**)(PHD_VBUF*, short)) 0x004E34A4 )
#define InsertSprite	( *(void(__cdecl**)(long, long, long, long, long, long, long, long, long, long)) 0x00531718 )
#define InsertTransQuad	( *(void(__cdecl**)(long, long, long, long, long)) 0x004D7908 )
