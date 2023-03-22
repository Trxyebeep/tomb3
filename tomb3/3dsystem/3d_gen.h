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

extern void (*InsertLine)(long x1, long y1, long x2, long y2, long z, long c0, long c1);
extern short* (*InsertObjectGT4)(short* pFaceInfo, long nFaces, sort_type nSortType);
extern short* (*InsertObjectGT3)(short* pFaceInfo, long nFaces, sort_type nSortType);
extern short* (*InsertObjectG4)(short* pFaceInfo, long nFaces, sort_type nSortType);
extern short* (*InsertObjectG3)(short* pFaceInfo, long nFaces, sort_type nSortType);
extern short* (*RoomInsertObjectGT4)(short* pFaceInfo, long nFaces, sort_type nSortType);
extern short* (*RoomInsertObjectGT3)(short* pFaceInfo, long nFaces, sort_type nSortType);
extern void (*InsertFlatRect)(long x1, long y1, long x2, long y2, long zdepth, long col);
extern void (*InsertTrans8)(PHD_VBUF* buf, short shade);
extern void (*InsertSprite)(long zdepth, long x1, long y1, long x2, long y2, long nSprite, long shade, long shade1, long nDrawType, long offset);
extern void (*InsertTransQuad)(long x, long y, long w, long h, long z);

extern long sort3d_bufferbf[MAX_SORTLISTS];
extern long sort3d_bufferfb[MAX_SORTLISTS];
extern short info3d_bufferbf[MAX_SORTLISTS];
extern short info3d_bufferfb[MAX_SORTLISTS];

extern long w2v_matrix[indices_count];
extern long matrix_stack[40 * indices_count];
