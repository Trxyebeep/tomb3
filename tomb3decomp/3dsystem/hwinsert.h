#pragma once
#include "../global/vars.h"

void inject_hwinsert(bool replace);

void HWI_InsertTrans8_Sorted(PHD_VBUF* buf, short shade);
void SubdivideEdge(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* dest);
void SubdivideGT4(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHD_VBUF* v4, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided, long num);
void SubdivideGT3(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided, long num);
void HWI_InsertGT4_Sorted(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHD_VBUF* v4, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided);
void HWI_InsertGT3_Sorted(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHDTEXTURESTRUCT* pTex, ushort* uv1, ushort* uv2, ushort* uv3, sort_type nSortType, ushort double_sided);
void HWI_InsertTransQuad_Sorted(long x, long y, long w, long h, long z);
void InitUVTable();
long GETR(long col);
long GETG(long col);
long GETB(long col);
void PHD_VBUF_To_D3DTLVTX(PHD_VBUF* phdV, D3DTLVERTEX* v, ushort* uv);
void PHD_VBUF_To_VERTEX_INFO(PHD_VBUF* phdV, VERTEX_INFO* v);
long visible_zclip(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2);
long FindBucket(DXTEXTURE* TPage);
void DrawBuckets();
void HWI_InsertClippedPoly_Textured(long nPoints, float zdepth, long nDrawType, long nTPage);
void HWI_InsertGT3_Poly(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2, PHDTEXTURESTRUCT* pTex, ushort* uv0, ushort* uv1, ushort* uv2, sort_type nSortType, ushort double_sided);

#define XYClipper	( (long(__cdecl*)(long, VERTEX_INFO*)) 0x0040CA50 )
#define HWI_InsertPoly_Gouraud	( (void(__cdecl*)(long, float, long, long, long, long)) 0x00408940 )
#define HWI_InsertGT4_Poly	( (void(__cdecl*)(PHD_VBUF*, PHD_VBUF*, PHD_VBUF*, PHD_VBUF*, PHDTEXTURESTRUCT*, sort_type, ushort)) 0x004070E0 )
#define RoomZedClipper	( (long(__cdecl*)(long, POINT_INFO*, VERTEX_INFO*)) 0x0040AA00 )
#define RoomXYGUVClipper	( (long(__cdecl*)(long, VERTEX_INFO*)) 0x0040ABE0 )
