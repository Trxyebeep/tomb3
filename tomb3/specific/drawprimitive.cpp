#include "../tomb3/pch.h"
#include "drawprimitive.h"

#define D3DDev	VAR_(0x006CA124, LPDIRECT3DDEVICEX)

HRESULT HWDrawPrimitive(D3DPRIMITIVETYPE dptPrimitiveType, D3DVERTEXTYPE dwVertexTypeDesc, LPVOID lpvVertices, ulong dwVertexCount, ulong dwFlags)
{
	return	D3DDev->DrawPrimitive(dptPrimitiveType, dwVertexTypeDesc, lpvVertices, dwVertexCount, dwFlags);
}

void inject_drawprim(bool replace)
{
	INJECT(0x00490AE0, HWDrawPrimitive, replace);
}
