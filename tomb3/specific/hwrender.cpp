#include "../tomb3/pch.h"
#include "../tomb3/pch.h"
#include "hwrender.h"
#include "texture.h"
#include "../3dsystem/hwinsert.h"
#include "init.h"
#include "winmain.h"
#include "output.h"
#include "drawbars.h"
#include "../tomb3/tomb3.h"

#if (DIRECT3D_VERSION >= 0x900)
HRESULT (*DrawPrimitive)(D3DPRIMITIVETYPE, LPVOID, ulong);
HRESULT (*SetTextureStageState)(ulong, D3DTEXTURESTAGESTATETYPE, ulong);
HRESULT (*SetSamplerState)(ulong, D3DSAMPLERSTATETYPE, ulong);
HRESULT (*SetTexture)(ulong, TEXHANDLE);
#else
HRESULT(*DrawPrimitive)(D3DPRIMITIVETYPE, D3DVERTEXTYPE, LPVOID, ulong, ulong);
#endif

HRESULT (*SetRenderState)(D3DRENDERSTATETYPE, ulong);
HRESULT (*BeginScene)();
HRESULT (*EndScene)();
void (*DrawRoutine)(long, D3DTLVERTEX*, long, long);

float GammaOption = 3.0F;
uchar ColorTable[256];

static D3DPRIMITIVETYPE dpPrimitiveType;
static bool zBufWriteEnabled;
static bool zBufCompareEnabled;
static bool AlphaBlendEnabled;
bool bAlphaTesting;

void HWR_EnableZBuffer(bool write, bool compare)
{
	if (!App.lpDXConfig->bZBuffer)
		return;

	if (write != zBufWriteEnabled)
	{
#if (DIRECT3D_VERSION >= 0x900)
		SetRenderState(D3DRS_ZWRITEENABLE, write ? D3DZB_TRUE : D3DZB_FALSE);
#else
		SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, write);
#endif
		zBufWriteEnabled = write;
	}

	if (compare != zBufCompareEnabled)
	{
#if (DIRECT3D_VERSION >= 0x900)
		SetRenderState(D3DRS_ZFUNC, compare ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS);
		SetRenderState(D3DRS_ZENABLE, compare ? D3DZB_TRUE : D3DZB_FALSE);
#else
		if (App.ZBuffer)
			SetRenderState(D3DRENDERSTATE_ZFUNC, compare ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS);
		else
			SetRenderState(D3DRENDERSTATE_ZENABLE, compare);
#endif

		zBufCompareEnabled = compare;
	}
}

void HWR_EnableColorKey(bool enable)
{
#if (DIRECT3D_VERSION >= 0x900)
	SetRenderState(D3DRS_ALPHABLENDENABLE, enable);
	AlphaBlendEnabled = enable;
#else
	static bool enabled;

	if (tomb3.disable_ckey)
		SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, 0);
	else if (enable)
	{
		if (!enabled)
		{
			SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, 1);
			enabled = 1;
		}
	}
	else if (enabled)
	{
		SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, 0);
		enabled = 0;
	}
#endif
}

void HWR_EnableAlphaBlend(bool enable)
{
	if (enable)
	{
		if (!AlphaBlendEnabled)
		{
#if (DIRECT3D_VERSION >= 0x900)
			SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
#else
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
#endif
			AlphaBlendEnabled = 1;
		}
	}
	else if (AlphaBlendEnabled)
	{
#if (DIRECT3D_VERSION >= 0x900)
		SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
#else
		SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
#endif
		AlphaBlendEnabled = 0;
	}
}

void HWR_EnableColorAddition(bool enable)
{
	if (enable)
	{
#if (DIRECT3D_VERSION >= 0x900)
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
#else
		SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
#endif
	}
	else
	{
#if (DIRECT3D_VERSION >= 0x900)
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
#else
		SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
#endif
	}
}

void HWR_EnableColorSubtraction(bool enable)
{
	if (enable)
	{
#if (DIRECT3D_VERSION >= 0x900)
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
#else
		SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
		SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR);
#endif
	}
	else
	{
#if (DIRECT3D_VERSION >= 0x900)
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
#else
		SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
#endif
	}
}

void HWR_ResetZBuffer()
{
	zBufWriteEnabled = 0;
	zBufCompareEnabled = 0;

#if (DIRECT3D_VERSION >= 0x900)
	SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_FALSE);
#else
	if (App.ZBuffer)
		SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
	else
	{
		SetRenderState(D3DRENDERSTATE_ZENABLE, 0);
		SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);
	}
#endif
}

void HWR_ResetColorKey()
{
	HWR_EnableAlphaBlend(0);
	HWR_EnableColorKey(0);
	HWR_EnableColorAddition(0);
}

void HWR_ResetCurrentTexture()
{
	HWR_SetCurrentTexture(0);
}

void HWR_BeginScene()
{
	bBlueEffect = 0;
	HWR_GetAllTextureHandles();
	HWR_EnableAlphaBlend(0);
	HWR_EnableColorKey(0);
	HWR_EnableColorAddition(0);
	HWR_ResetCurrentTexture();
	BeginScene();
	nDrawnPoints = 0;

	if (App.lpDXConfig->bZBuffer)
	{
		for (int i = 0; i < MAX_BUCKETS; i++)
		{
			Buckets[i].TPage = (DXTEXTURE*)-1;
			Buckets[i].nVtx = 0;
		}
	}
}

void HWR_EndScene()
{
	EndScene();
}

#if (DIRECT3D_VERSION >= 0x900)
void HWR_DrawRoutines(long nVtx, D3DTLVERTEX* vtx, long nDrawType, long TPage)
{
	switch (nDrawType)
	{
	case DT_POLY_GT:
		HWR_EnableAlphaBlend(0);
		HWR_EnableAlphaBlend(0);
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		DrawPrimitive(dpPrimitiveType, vtx, nVtx);
		return;

	case DT_POLY_WGT:
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableZBuffer(1, 1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorAddition(0);
		DrawPrimitive(dpPrimitiveType, vtx, nVtx);
		HWR_EnableZBuffer(0, 1);
		return;

	case DT_POLY_G:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(0);
		DrawPrimitive(dpPrimitiveType, vtx, nVtx);
		return;

	case DT_LINE_SOLID:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorAddition(0);
		DrawPrimitive(D3DPT_LINELIST, vtx, nVtx);
		return;

	case DT_LINE_ALPHA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorAddition(1);
		DrawPrimitive(D3DPT_LINELIST, vtx, nVtx);
		return;

	case DT_POLY_GA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorAddition(0);
		DrawPrimitive(dpPrimitiveType, vtx, nVtx);
		return;

	case DT_POLY_WGTA:
		HWR_EnableZBuffer(0, 1);
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableColorAddition(1);
		HWR_EnableAlphaBlend(1);
		DrawPrimitive(dpPrimitiveType, vtx, nVtx);
		return;

	case DT_POLY_COLSUB:
		HWR_EnableZBuffer(0, 1);
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableColorSubtraction(1);
		HWR_EnableAlphaBlend(1);
		DrawPrimitive(dpPrimitiveType, vtx, nVtx);
		return;

	case DT_POLY_GTA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorAddition(1);
		DrawPrimitive(dpPrimitiveType, vtx, nVtx);
		return;
	}
}
#else
void HWR_DrawRoutines(long nVtx, D3DTLVERTEX* vtx, long nDrawType, long TPage)
{
	switch (nDrawType)
	{
	case DT_POLY_GT:
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorKey(0);
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_WGT:
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableZBuffer(1, 1);
		HWR_EnableColorKey(1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorAddition(0);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		HWR_EnableZBuffer(0, 1);
		return;

	case DT_POLY_G:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorKey(0);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_LINE_SOLID:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(0);
		HWR_EnableColorAddition(0);
		DrawPrimitive(D3DPT_LINELIST, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_LINE_ALPHA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		HWR_EnableColorAddition(1);
		DrawPrimitive(D3DPT_LINELIST, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_GA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorAddition(0);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_WGTA:
		HWR_EnableZBuffer(0, 1);
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableColorAddition(1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_COLSUB:
		HWR_EnableZBuffer(0, 1);
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableColorSubtraction(1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_GTA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorAddition(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;
	}
}

void HWR_DrawRoutinesStippledAlpha(long nVtx, D3DTLVERTEX* vtx, long nDrawType, long TPage)
{
	switch (nDrawType)
	{
	case DT_POLY_GT:
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorKey(0);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_WGT:
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableZBuffer(1, 1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		HWR_EnableColorAddition(0);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		HWR_EnableZBuffer(0, 1);
		return;

	case DT_POLY_G:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorKey(0);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_LINE_SOLID:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(0);
		DrawPrimitive(D3DPT_LINELIST, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_LINE_ALPHA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		DrawPrimitive(D3DPT_LINELIST, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_GA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorAddition(0);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 0);
		return;

	case DT_POLY_WGTA:
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableColorAddition(1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 0);
		return;

	case DT_POLY_COLSUB:
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableColorSubtraction(1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 0);
		return;

	case DT_POLY_GTA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorAddition(1);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 0);
		return;
	}
}

void HWR_DrawRoutinesNoAlpha(long nVtx, D3DTLVERTEX* vtx, long nDrawType, long TPage)
{
	switch (nDrawType)
	{
	case DT_POLY_GT:
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorKey(0);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_WGT:
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_G:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorKey(0);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_LINE_SOLID:
		HWR_SetCurrentTexture(0);
		HWR_EnableColorKey(0);
		HWR_EnableAlphaBlend(1);
		DrawPrimitive(D3DPT_LINELIST, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_LINE_ALPHA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		DrawPrimitive(D3DPT_LINELIST, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_GA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_WGTA:
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_COLSUB:
		HWR_SetCurrentTexture(TexturePtrs[TPage]);
		HWR_EnableColorSubtraction(1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;
	}
}
#endif

__inline void HWR_InitGamma(float gamma)
{
#if (DIRECT3D_VERSION >= 0x900)
	if (tomb3.psx_contrast)
		gamma = 2.5F;
#endif
	gamma = 1.0F / (gamma / 10.0F * 4.0F);

	for (int i = 0; i < 256; i++)
		ColorTable[i] = uchar(pow((double)i / 256.0F, gamma) * 256.0F);
}

void HWR_InitState()
{
#if (DIRECT3D_VERSION >= 0x900)
	if (!SetRenderState || !SetTextureStageState || !SetSamplerState)
		return;

	SetRenderState(D3DRS_CLIPPING, 0);
	SetRenderState(D3DRS_FILLMODE, HWConfig.nFillMode);
	SetRenderState(D3DRS_SHADEMODE, HWConfig.nShadeMode);
	SetRenderState(D3DRS_DITHERENABLE, HWConfig.bDither);
	SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRS_SPECULARENABLE, 0);
	SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
	SetRenderState(D3DRS_ALPHAREF, 0);
	SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL);

	if (tomb3.psx_contrast)
		SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
	else
		SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	SetSamplerState(0, D3DSAMP_MAGFILTER, HWConfig.nFilter);
	SetSamplerState(0, D3DSAMP_MINFILTER, HWConfig.nFilter);
	SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	if (tomb3.disable_gamma)
		GammaOption = 2.5F;

	HWR_InitGamma(GammaOption);
	HWR_ResetCurrentTexture();
	HWR_ResetColorKey();
	HWR_ResetZBuffer();
	DrawRoutine = HWR_DrawRoutines;
	bAlphaTesting = 1;
	GlobalAlpha = 0xFF000000;
#else
	DIRECT3DINFO* d3dinfo;
	bool blendOne, stippledAlpha, blendAlpha;

	if (tomb3.disable_gamma)
		GammaOption = 2.5F;

	HWR_InitGamma(GammaOption);

	if (!SetRenderState)
		return;

	HWR_ResetZBuffer();
	SetRenderState(D3DRENDERSTATE_FILLMODE, HWConfig.nFillMode);
	SetRenderState(D3DRENDERSTATE_SHADEMODE, HWConfig.nShadeMode);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAG, HWConfig.nFilter);
	SetRenderState(D3DRENDERSTATE_TEXTUREMIN, HWConfig.nFilter);
	SetRenderState(D3DRENDERSTATE_DITHERENABLE, HWConfig.bDither);
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
	SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
	SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 0);
	SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 0);
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
	SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, 0);
	HWR_ResetCurrentTexture();
	HWR_ResetColorKey();

	d3dinfo = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D];

	if (d3dinfo->DeviceDesc.dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_NOTEQUAL)
	{
		SetRenderState(D3DRENDERSTATE_ALPHAREF, 0);
		SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_NOTEQUAL);
		bAlphaTesting = 1;
	}
	else
		bAlphaTesting = 0;

	blendOne = d3dinfo->DeviceDesc.dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_ONE;
	stippledAlpha = d3dinfo->DeviceDesc.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_ALPHAFLATSTIPPLED;
	blendAlpha = d3dinfo->DeviceDesc.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_ALPHAFLATBLEND;

	GlobalAlpha = 0xFF000000;

	if (stippledAlpha && !blendAlpha)
	{
		DrawRoutine = HWR_DrawRoutinesStippledAlpha;
		GlobalAlpha = 0x80000000;
	}
	else if (blendOne)
		DrawRoutine = HWR_DrawRoutines;
	else
	{
		DrawRoutine = HWR_DrawRoutinesNoAlpha;
		GlobalAlpha = 0x80000000;
	}
#endif
}

bool HWR_Init()
{
	VertexBuffer = 0;
	HWR_InitState();
	return 1;
}

void HWR_DrawPolyList(long num, long* pSort)
{
	D3DTLVERTEX* vtx;
	short* pInfo;
	long polyType;
	long x0, y0, x1, y1, bar, p;
	short nVtx, nDrawType, TPage;

	dpPrimitiveType = D3DPT_TRIANGLEFAN;

	for (int i = 0; i < num; i++)
	{
		pInfo = (short*)pSort[0];

		polyType = pSort[2];

		if (polyType == POLYTYPE_HEALTHBAR ||
			polyType == POLYTYPE_AIRBAR ||
			polyType == POLYTYPE_DASHBAR ||
			polyType == POLYTYPE_COLDBAR)
		{
			x0 = pInfo[0];
			y0 = pInfo[1];
			x1 = pInfo[2];
			y1 = pInfo[3];
			bar = pInfo[4];
			p = pInfo[5];

			if (polyType == POLYTYPE_HEALTHBAR)
				DoPSXHealthBar(x0, y0, x1, y1, bar, p);
			else if (polyType == POLYTYPE_DASHBAR)
				DoPSXDashBar(x0, y0, x1, y1, bar, p);
			else if (polyType == POLYTYPE_AIRBAR)
				DoPSXAirBar(x0, y0, x1, y1, bar, p);
			else if (polyType == POLYTYPE_COLDBAR)
				DoPSXColdBar(x0, y0, x1, y1, bar, p);

			pSort += 3;
			continue;
		}

		nDrawType = pInfo[0];
		TPage = pInfo[1];
		nVtx = pInfo[2];
		vtx = *((D3DTLVERTEX**)(pInfo + 3));
		DrawRoutine(nVtx, vtx, nDrawType, TPage);
		pSort += 3;
	}
}

void HWR_DrawPolyListBF(long num, long* pSort)
{
	D3DTLVERTEX* vtx;
	D3DTLVERTEX* URvtx;
	D3DTLVERTEX* fanStart;
	short* pInfo;
	short nVtx, nURVtx, nDrawType1, TPage1, nDrawType2, TPage2;

	dpPrimitiveType = D3DPT_TRIANGLELIST;

	if (!num)
		return;

	nURVtx = 0;
	pInfo = (short*)pSort[0];
	nDrawType1 = pInfo[0];
	TPage1 = pInfo[1];

	for (int i = 0; i < num; i++)
	{
		pInfo = (short*)pSort[0];
		nDrawType2 = pInfo[0];
		TPage2 = pInfo[1];
		nVtx = pInfo[2];
		vtx = *((D3DTLVERTEX**)(pInfo + 3));

		if (nDrawType1 != nDrawType2 || TPage1 != TPage2 || nURVtx > 256)
		{
			DrawRoutine(nURVtx, UnRollBuffer, nDrawType1, TPage1);
			nDrawType1 = nDrawType2;
			TPage1 = TPage2;
			nURVtx = 0;
		}

		if (nDrawType1 == DT_LINE_SOLID || nDrawType1 == DT_LINE_ALPHA)
		{
			URvtx = &UnRollBuffer[nURVtx];
			URvtx->sx = vtx->sx;
			URvtx->sy = vtx->sy;
			URvtx->sz = vtx->sz;
			URvtx->rhw = vtx->rhw;
			URvtx->tu = vtx->tu;
			URvtx->tv = vtx->tv;
			URvtx->color = vtx->color;
			URvtx->specular = vtx->specular;
			nURVtx++;
			vtx++;

			URvtx = &UnRollBuffer[nURVtx];
			URvtx->sx = vtx->sx;
			URvtx->sy = vtx->sy;
			URvtx->sz = vtx->sz;
			URvtx->rhw = vtx->rhw;
			URvtx->tu = vtx->tu;
			URvtx->tv = vtx->tv;
			URvtx->color = vtx->color;
			URvtx->specular = vtx->specular;
			nURVtx++;
		}
		else
		{
			fanStart = vtx;

			URvtx = &UnRollBuffer[nURVtx];
			URvtx->sx = vtx->sx;
			URvtx->sy = vtx->sy;
			URvtx->sz = vtx->sz;
			URvtx->rhw = vtx->rhw;
			URvtx->tu = vtx->tu;
			URvtx->tv = vtx->tv;
			URvtx->color = vtx->color;
			URvtx->specular = vtx->specular;
			nURVtx++;
			vtx++;

			URvtx = &UnRollBuffer[nURVtx];
			URvtx->sx = vtx->sx;
			URvtx->sy = vtx->sy;
			URvtx->sz = vtx->sz;
			URvtx->rhw = vtx->rhw;
			URvtx->tu = vtx->tu;
			URvtx->tv = vtx->tv;
			URvtx->color = vtx->color;
			URvtx->specular = vtx->specular;
			nURVtx++;
			vtx++;

			URvtx = &UnRollBuffer[nURVtx];
			URvtx->sx = vtx->sx;
			URvtx->sy = vtx->sy;
			URvtx->sz = vtx->sz;
			URvtx->rhw = vtx->rhw;
			URvtx->tu = vtx->tu;
			URvtx->tv = vtx->tv;
			URvtx->color = vtx->color;
			URvtx->specular = vtx->specular;
			nURVtx++;

			nVtx -= 3;

			for (int i = 0; i < nVtx; i++)
			{
				URvtx = &UnRollBuffer[nURVtx];
				URvtx->sx = fanStart->sx;
				URvtx->sy = fanStart->sy;
				URvtx->sz = fanStart->sz;
				URvtx->rhw = fanStart->rhw;
				URvtx->tu = fanStart->tu;
				URvtx->tv = fanStart->tv;
				URvtx->color = fanStart->color;
				URvtx->specular = fanStart->specular;
				nURVtx++;

				URvtx = &UnRollBuffer[nURVtx];
				URvtx->sx = vtx->sx;
				URvtx->sy = vtx->sy;
				URvtx->sz = vtx->sz;
				URvtx->rhw = vtx->rhw;
				URvtx->tu = vtx->tu;
				URvtx->tv = vtx->tv;
				URvtx->color = vtx->color;
				URvtx->specular = vtx->specular;
				nURVtx++;
				vtx++;

				URvtx = &UnRollBuffer[nURVtx];
				URvtx->sx = vtx->sx;
				URvtx->sy = vtx->sy;
				URvtx->sz = vtx->sz;
				URvtx->rhw = vtx->rhw;
				URvtx->tu = vtx->tu;
				URvtx->tv = vtx->tv;
				URvtx->color = vtx->color;
				URvtx->specular = vtx->specular;
				nURVtx++;
			}
		}

		pSort += 3;
	}

	if (nURVtx > 0)
		DrawRoutine(nURVtx, UnRollBuffer, nDrawType1, TPage1);
}

void HWR_FreeTexturePages()
{
	for (int i = 0; i < MAX_TPAGES; i++)
	{
		if (Textures[i].dwFlags & TF_LEVELTEX)
			DXTextureCleanup(i, Textures);
	}

#if (DIRECT3D_VERSION < 0x900)
	DXReleasePalette();
#endif
}

void HWR_GetAllTextureHandles()
{
	DXTEXTURE* tex;
	long n;

	memset(TexturePtrs, 0, sizeof(TexturePtrs));
	n = 0;

	for (int i = 0; i < MAX_TPAGES; i++)
	{
		tex = DXRestoreSurfaceIfLost(i, Textures);

		if (tex->dwFlags & TF_LEVELTEX)
		{
			TexturePtrs[n] = tex;
			n++;
		}
	}

	for (int i = 0; i < MAX_TPAGES; i++)
	{
		tex = DXRestoreSurfaceIfLost(i, Textures);

		if (tex->dwFlags & TF_PICTEX)
		{
			TexturePtrs[n] = tex;
			n++;
		}
	}
}

void HWR_LoadTexturePages(long nPages, uchar* src, uchar* palette)
{
	HWR_FreeTexturePages();

#if (DIRECT3D_VERSION < 0x900)
	if (palette)
	{
		DXTextureNewPalette(palette);
		DXFreeTPages();
		DXCreateMaxTPages(1);
	}
#endif

	for (int i = 0; i < nPages; i++)
	{
#if (DIRECT3D_VERSION < 0x900)
		if (palette)
		{
			DXTextureAddPal(256, 256, src, Textures, TF_LEVELTEX);
			src += 0x10000;
		}
		else
#endif
		{
			DXTextureAdd(256, 256, src, Textures, 555, TF_LEVELTEX);
			src += 0x20000;
		}
	}

	HWR_GetAllTextureHandles();

#if (DIRECT3D_VERSION < 0x900)
	for (int i = 0; i < nTextures; i++)
		HWR_SetCurrentTexture(TexturePtrs[i]);
#endif
}

void HWR_SetCurrentTexture(DXTEXTURE* tex)
{
	static TEXHANDLE lastTextureHandle;

#if (DIRECT3D_VERSION >= 0x900)
	if (!tex)
	{
		lastTextureHandle = 0;
		SetTexture(0, 0);
		return;
	}

	if (tex->tex != lastTextureHandle)
	{
		SetTexture(0, tex->tex);
		lastTextureHandle = tex->tex;
	}
#else
	TEXTURE* tdata;
	TEXTURE* temp;
	LPDIRECT3DTEXTUREX d3dtex;
	TEXHANDLE handle;
	ulong n;

	handle = 0;

	if (!nTextures)
		return;

	if (tex)
	{
		for (int i = 0; i < MAX_TPAGES; i++)
		{
			if (TextureSurfaces[i].DXTex == tex)
			{
				handle = TextureSurfaces[i].handle;
				TextureSurfaces[i].nFrames = App.nFrames;
				break;
			}
		}

		if (!handle)
		{
			n = 0;
			tdata = (TEXTURE*)tex;

			for (int i = 0; i < nTextures; i++)
			{
				temp = &TextureSurfaces[i];

				if (!temp->DXTex && tex->bpp == temp->bpp)
				{
					tdata = temp;
					break;
				}

				if (App.nFrames - temp->nFrames >= n && tex->bpp == temp->bpp)
				{
					n = App.nFrames - temp->nFrames;
					tdata = &temp[-1];
				}
			}

			handle = tdata->handle;

			if (!n)
				SetRenderState(D3DRENDERSTATE_FLUSHBATCH, 0);

			if (tdata->pSurf->IsLost() == DDERR_SURFACELOST)
				tdata->pSurf->Restore();

			if (tex->pSystemSurface)
			{
				d3dtex = DXTextureGetInterface(tex->pSystemSurface);

				if (tdata->pTexture->Load(d3dtex) != D3D_OK)
				{
					d3dtex = DXTextureGetInterface(tex->pSystemSurface);
					tdata->pTexture->Load(d3dtex);
				}
			}

			tdata->DXTex = tex;
			tdata->nFrames = App.nFrames;
			tex->tex = tdata;
		}
	}

	if (handle != lastTextureHandle)
	{
		SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, handle);
		lastTextureHandle = handle;
	}
#endif
}
