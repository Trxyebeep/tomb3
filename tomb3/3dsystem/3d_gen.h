#pragma once
#include "../global/types.h"

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
void phd_SetTrans(long x, long y, long z);
void phd_PopMatrix();
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

extern void (*InsertLine)(long x1, long y1, long x2, long y2, long z, long c0, long c1);
extern short* (*InsertObjectGT4)(short* pFaceInfo, long nFaces, sort_type nSortType);
extern short* (*InsertObjectGT3)(short* pFaceInfo, long nFaces, sort_type nSortType);
extern short* (*InsertObjectG4)(short* pFaceInfo, long nFaces, sort_type nSortType);
extern short* (*InsertObjectG3)(short* pFaceInfo, long nFaces, sort_type nSortType);
extern void (*InsertFlatRect)(long x1, long y1, long x2, long y2, long zdepth, long col);
extern void (*InsertTrans8)(PHD_VBUF* buf, short shade);
extern void (*InsertSprite)(long zdepth, long x1, long y1, long x2, long y2, long nSprite, ulong shade, ulong shade1, long nDrawType, long offset);
extern void (*InsertTransQuad)(long x, long y, long w, long h, long z);
extern void (*InsertGourQuad)(long x0, long y0, long x1, long y1, long z, ulong c0, ulong c1, ulong c2, ulong c3, bool add);
extern bool (*IsVisible)(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2);
extern bool (*IsInvisible)(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2);

extern float outsideBackgroundTop;
extern long BlackGouraudIndex;
extern bool bFixSkyColour;

extern long sort3d_bufferbf[MAX_SORTLISTS];
extern short info3d_bufferbf[MAX_SORTLISTS];
extern long* sort3dptrbf;
extern short* info3dptrbf;
extern long surfacenumbf;

extern long sort3d_bufferfb[MAX_SORTLISTS];
extern short info3d_bufferfb[MAX_SORTLISTS];
extern long* sort3dptrfb;
extern short* info3dptrfb;
extern long surfacenumfb;

extern long* phd_mxptr;
extern long w2v_matrix[indices_count];
extern long matrix_stack[40 * indices_count];

extern float one;

extern PHD_VECTOR CamPos;
extern PHD_VECTOR CamRot;

extern float f_centerx;
extern float f_centery;
extern float f_znear;
extern float f_zfar;
extern float f_persp;
extern float f_oneopersp;
extern float f_perspoznear;
extern float f_oneoznear;
extern float f_a;
extern float f_b;
extern float f_boo;

extern long phd_centerx;
extern long phd_centery;
extern long phd_znear;
extern long phd_zfar;
extern long phd_persp;

extern RECT phd_WindowRect;
extern float phd_leftfloat;
extern float phd_topfloat;
extern float phd_rightfloat;
extern float phd_bottomfloat;
extern long phd_winwidth;
extern long phd_winheight;
extern long phd_right;
extern long phd_left;
extern long phd_bottom;
extern long phd_top;
extern long phd_scrwidth;
extern long phd_scrheight;
extern long phd_viewdist;
extern short phd_winxmin;
extern short phd_winxmax;
extern short phd_winymin;
extern short phd_winymax;
