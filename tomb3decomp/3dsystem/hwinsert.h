#pragma once
#include "../global/vars.h"

void inject_hwinsert(bool replace);

void HWI_InsertTrans8_Sorted(PHD_VBUF* buf, short shade);
void SubdivideEdge(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* dest);
void SubdivideGT4(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHD_VBUF* v4, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided, long num);
void SubdivideGT3(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided, long num);
void HWI_InsertGT4_Sorted(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHD_VBUF* v4, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided);

#define XYClipper	( (long(__cdecl*)(long, VERTEX_INFO*)) 0x0040CA50 )
#define HWI_InsertPoly_Gouraud	( (void(__cdecl*)(long, float, long, long, long, long)) 0x00408940 )
#define HWI_InsertGT3_Poly	( (void(__cdecl*)(PHD_VBUF*, PHD_VBUF*, PHD_VBUF*, PHDTEXTURESTRUCT*, ushort*, ushort*, ushort*, sort_type, ushort)) 0x00405A80 )
#define HWI_InsertGT4_Poly	( (void(__cdecl*)(PHD_VBUF*, PHD_VBUF*, PHD_VBUF*, PHD_VBUF*, PHDTEXTURESTRUCT*, sort_type, ushort)) 0x004070E0 )
