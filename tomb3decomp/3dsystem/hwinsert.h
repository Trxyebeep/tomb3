#pragma once
#include "../global/vars.h"

void inject_hwinsert(bool replace);

void HWI_InsertTrans8_Sorted(PHD_VBUF* buf, short shade);
void SubdivideEdge(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* dest);

#define XYClipper	( (long(__cdecl*)(long, VERTEX_INFO*)) 0x0040CA50 )
#define HWI_InsertPoly_Gouraud	( (void(__cdecl*)(long, float, long, long, long, long)) 0x00408940 )
