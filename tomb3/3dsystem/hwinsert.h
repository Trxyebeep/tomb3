#pragma once
#include "../global/types.h"

void HWI_InsertTrans8_Sorted(PHD_VBUF* buf, short shade);
void SubdivideEdge(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* dest);
void SubdivideGT4(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHD_VBUF* v4, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided, long num);
void SubdivideGT3(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided, long num);
void HWI_InsertGT4_Sorted(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHD_VBUF* v4, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided);
void HWI_InsertGT3_Sorted(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHDTEXTURESTRUCT* pTex, ushort* uv1, ushort* uv2, ushort* uv3, sort_type nSortType, ushort double_sided);
void HWI_InsertTransQuad_Sorted(long x, long y, long w, long h, long z);
void HWI_InsertGourQuad_Sorted(long x0, long y0, long x1, long y1, long z, ulong c0, ulong c1, ulong c2, ulong c3, bool add);
void InitUVTable();
long GETR(ulong col);
long GETG(ulong col);
long GETB(ulong col);
void PHD_VBUF_To_D3DTLVTX_WITHUV(PHD_VBUF* phdV, D3DTLVERTEX* v, ushort* uv);
void PHD_VBUF_To_VERTEX_INFO(PHD_VBUF* phdV, VERTEX_INFO* v);
long visible_zclip(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2);
long FindBucket(DXTEXTURE* TPage);
void DrawBuckets();
void HWI_InsertClippedPoly_Textured(long nPoints, float zdepth, long nDrawType, long nTPage);
void HWI_InsertGT3_Poly(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2, PHDTEXTURESTRUCT* pTex, ushort* uv0, ushort* uv1, ushort* uv2, sort_type nSortType, ushort double_sided);
void HWI_InsertLine_Sorted(long x1, long y1, long x2, long y2, long z, long c0, long c1);
void HWI_InsertGT4_Poly(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided);
void HWI_InsertPoly_Gouraud(long nPoints, float zdepth, long r, long g, long b, long nDrawType);
void HWI_InsertPoly_GouraudRGB(long nPoints, float zdepth, long nDrawType);
void HWI_InsertFlatRect_Sorted(long x1, long y1, long x2, long y2, long zdepth, long col);
void HWI_InsertSprite_Sorted(long zdepth, long x1, long y1, long x2, long y2, long nSprite, ulong shade, ulong shade1, long nDrawType, long offset);
void HWI_InsertAlphaSprite_Sorted(long x1, long y1, long z1, long shade1, long x2, long y2, long z2, long shade2,
	long x3, long y3, long z3, long shade3, long x4, long y4, long z4, long shade4,
	long nSprite, long nDrawtype, long double_sided);
short* HWI_InsertObjectG3_Sorted(short* pFaceInfo, long nFaces, sort_type nSortType);
short* HWI_InsertObjectGT3_Sorted(short* pFaceInfo, long nFaces, sort_type nSortType);
short* HWI_InsertObjectG4_Sorted(short* pFaceInfo, long nFaces, sort_type nSortType);
short* HWI_InsertObjectGT4_Sorted(short* pFaceInfo, long nFaces, sort_type nSortType);
long RoomZedClipper(long n, POINT_INFO* in, VERTEX_INFO* out);
long RoomXYGUVClipper(long n, VERTEX_INFO* in);
long XYGClipper(long n, VERTEX_INFO* in);
long XYClipper(long n, VERTEX_INFO* in);

extern PHD_VBUF vbuf[MAX_VBUF];
extern TEXTUREBUCKET Buckets[MAX_BUCKETS];
extern long GlobalAlpha;
extern long nDrawnPoints;
