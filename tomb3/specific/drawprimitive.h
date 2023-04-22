#pragma once
#include "../global/types.h"

#if (DIRECT3D_VERSION >= 0x900)
HRESULT HWDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, LPVOID Vertices, ulong VertexCount);
HRESULT HWSetTextureStageState(ulong Stage, D3DTEXTURESTAGESTATETYPE Type, ulong Value);
HRESULT HWSetSamplerState(ulong Sampler, D3DSAMPLERSTATETYPE Type, ulong Value);
HRESULT HWSetTexture(ulong Stage, TEXHANDLE pTexture);
#else
HRESULT HWDrawPrimitive(D3DPRIMITIVETYPE dptPrimitiveType, D3DVERTEXTYPE dwVertexTypeDesc, LPVOID lpvVertices, ulong dwVertexCount, ulong dwFlags);
#endif
HRESULT HWBeginScene();
HRESULT HWEndScene();
HRESULT HWSetRenderState(D3DRENDERSTATETYPE dwRenderStateType, ulong dwRenderState);
void InitDrawPrimitive(LPDIRECT3DDEVICEX lpD3DDev);
