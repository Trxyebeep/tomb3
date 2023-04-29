#include "../tomb3/pch.h"
#include "drawprimitive.h"
#include "winmain.h"
#include "hwrender.h"

static LPDIRECT3DDEVICEX Dev;

#if (DIRECT3D_VERSION >= 0x900)
HRESULT HWDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, LPVOID Vertices, ulong VertexCount)
{
	HRESULT ret;
	LPVOID pVtx;
	static ulong index = 0;
	ulong flags;
	long nPrimitive;

	pVtx = 0;
	nPrimitive = 0;

	switch (PrimitiveType)
	{
	case D3DPT_TRIANGLELIST:
		nPrimitive = VertexCount / 3;
		break;

	case D3DPT_TRIANGLESTRIP:
	case D3DPT_TRIANGLEFAN:
		nPrimitive = VertexCount - 2;
		break;

	case D3DPT_POINTLIST:
		nPrimitive = VertexCount;
		break;

	case D3DPT_LINELIST:
		nPrimitive = VertexCount / 2;
		break;

	case D3DPT_LINESTRIP:
		nPrimitive = VertexCount - 1;
		break;
	}

	if (nPrimitive <= 0)
		return D3DERR_INVALIDCALL;

	flags = D3DLOCK_NOOVERWRITE;

	if (VertexCount + index > VTXBUF_LEN)
	{
		index = 0;
		flags = D3DLOCK_DISCARD;
	}

	if (FAILED(ret = App.DestVB->Lock(sizeof(D3DTLVERTEX) * index, sizeof(D3DTLVERTEX) * VertexCount, &pVtx, flags)))
		return ret;

	memcpy(pVtx, Vertices, sizeof(D3DTLVERTEX) * VertexCount);
	App.DestVB->Unlock();

	ret = Dev->DrawPrimitive(PrimitiveType, index, nPrimitive);
	index += VertexCount;
	return ret;
}

HRESULT HWSetTextureStageState(ulong Stage, D3DTEXTURESTAGESTATETYPE Type, ulong Value)
{
	return Dev->SetTextureStageState(Stage, Type, Value);
}

HRESULT HWSetSamplerState(ulong Sampler, D3DSAMPLERSTATETYPE Type, ulong Value)
{
	return Dev->SetSamplerState(Sampler, Type, Value);
}

HRESULT HWSetTexture(ulong Stage, TEXHANDLE pTexture)
{
	return Dev->SetTexture(Stage, pTexture);
}
#else
HRESULT HWDrawPrimitive(D3DPRIMITIVETYPE dptPrimitiveType, D3DVERTEXTYPE dwVertexTypeDesc, LPVOID lpvVertices, ulong dwVertexCount, ulong dwFlags)
{
	return Dev->DrawPrimitive(dptPrimitiveType, dwVertexTypeDesc, lpvVertices, dwVertexCount, dwFlags);
}
#endif

HRESULT HWBeginScene()
{
	WinFrameRate();
	return Dev->BeginScene();
}

HRESULT HWEndScene()
{
	return Dev->EndScene();
}

HRESULT HWSetRenderState(D3DRENDERSTATETYPE dwRenderStateType, ulong dwRenderState)
{
	return Dev->SetRenderState(dwRenderStateType, dwRenderState);
}

void InitDrawPrimitive(LPDIRECT3DDEVICEX lpD3DDev)
{
	Dev = lpD3DDev;
	DrawPrimitive = HWDrawPrimitive;
#if (DIRECT3D_VERSION >= 0x900)
	SetTextureStageState = HWSetTextureStageState;
	SetSamplerState = HWSetSamplerState;
	SetTexture = HWSetTexture;
#endif
	BeginScene = HWBeginScene;
	EndScene = HWEndScene;
	SetRenderState = HWSetRenderState;
}
