#include "../tomb3/pch.h"
#include "drawprimitive.h"
#include "winmain.h"

#define D3DDev	VAR_(0x006CA124, LPDIRECT3DDEVICEX)

HRESULT HWDrawPrimitive(D3DPRIMITIVETYPE dptPrimitiveType, D3DVERTEXTYPE dwVertexTypeDesc, LPVOID lpvVertices, ulong dwVertexCount, ulong dwFlags)
{
	return	D3DDev->DrawPrimitive(dptPrimitiveType, dwVertexTypeDesc, lpvVertices, dwVertexCount, dwFlags);
}

HRESULT HWBeginScene()
{
	WinFrameRate();
	return D3DDev->BeginScene();
}

HRESULT HWEndScene()
{
	return D3DDev->EndScene();
}

HRESULT HWSetRenderState(D3DRENDERSTATETYPE dwRenderStateType, ulong dwRenderState)
{
	return D3DDev->SetRenderState(dwRenderStateType, dwRenderState);
}

void inject_drawprim(bool replace)
{
	INJECT(0x00490AE0, HWDrawPrimitive, replace);
	INJECT(0x00490970, HWBeginScene, replace);
	INJECT(0x00490990, HWEndScene, replace);
	INJECT(0x00490A00, HWSetRenderState, replace);
}
