#pragma once
#include "../global/vars.h"

void inject_drawprim(bool replace);

HRESULT HWDrawPrimitive(D3DPRIMITIVETYPE dptPrimitiveType, D3DVERTEXTYPE dwVertexTypeDesc, LPVOID lpvVertices, ulong dwVertexCount, ulong dwFlags);
HRESULT HWBeginScene();
HRESULT HWEndScene();
HRESULT HWSetRenderState(D3DRENDERSTATETYPE dwRenderStateType, ulong dwRenderState);
void InitDrawPrimitive(LPDIRECT3DDEVICEX lpD3DDev, LPDIRECTDRAWSURFACEX surf, bool hw);

#define CloseDrawPrimitive	( (void(__cdecl*)()) 0x00490700 )
