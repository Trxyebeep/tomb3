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
void phd_SortPolyList(long count, long* list);
void phd_NormaliseVector(long x, long y, long z, long* dest);
void phd_GetVectorAngles(long x, long y, long z, short* angles);
void phd_GenerateW2V(PHD_3DPOS* viewPos);
void phd_LookAt(long sx, long sy, long sz, long tx, long ty, long tz, short roll);

__inline void phd_PopMatrix()
{
	phd_mxptr -= indices_count;
}

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
