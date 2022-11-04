#include "../tomb3/pch.h"
#include "../tomb3/pch.h"
#include "hwrender.h"
#include "texture.h"
#ifdef TROYESTUFF
#include "drawbars.h"
#include "../tomb3/tomb3.h"
#endif

bool zBufWriteEnabled;
bool zBufCompareEnabled;

void HWR_EnableZBuffer(bool write, bool compare)
{
	if (App.lpZBuffer)
	{
		if (write != zBufWriteEnabled)
		{
			SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, write);
			zBufWriteEnabled = write;
		}

		if (compare != zBufCompareEnabled)
		{
			if (App.lpZBuffer)
				SetRenderState(D3DRENDERSTATE_ZFUNC, compare ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS);
			else
				SetRenderState(D3DRENDERSTATE_ZENABLE, compare);

			zBufCompareEnabled = compare;
		}
	}
}

void HWR_EnableColorKey(bool enable)
{
	static bool enabled;

#ifdef TROYESTUFF
	if (tomb3.disable_ckey)
		SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, 0);
	else
#endif
	{
		if (enable)
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
	}
}

void HWR_EnableAlphaBlend(bool enable)
{
	static bool enabled;

	if (enable)
	{
		if (!enabled)
		{
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
			enabled = 1;
		}
	}
	else if (enabled)
	{
		SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
		enabled = 0;
	}
}

#ifdef TROYESTUFF
void HWR_EnableColorAddition(bool enable)
{
	if (enable)
	{
		SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
	}
	else
	{
		SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
}

void HWR_EnableColorSubtraction(bool enable)
{
	if (enable)
	{
		SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
		SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR);
	}
	else
	{
		SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
}
#else
void HWR_EnableColorAddition(bool enable)
{
	static bool enabled;

	if (enable)
	{
		if (!enabled)
		{
			enabled = 1;
			SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
		}
	}
	else if (enabled)
	{
		enabled = 0;
		SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
}
#endif

void HWR_ResetZBuffer()
{
	zBufWriteEnabled = 0;
	zBufCompareEnabled = 0;

	if (App.lpZBuffer)
		SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
	else
	{
		SetRenderState(D3DRENDERSTATE_ZENABLE, 0);
		SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);
	}
}

void HWR_ResetColorKey()
{
	HWR_EnableAlphaBlend(0);
	HWR_EnableColorKey(0);
	HWR_EnableColorAddition(0);
}

void HWR_EnablePerspCorrect(bool enable)
{
	static bool enabled;

	if (App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware)
		SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1);
	else if (enable)
	{
		if (!enabled)
		{
			enabled = 1;
			SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1);
		}
	}
	else if (enabled)
	{
		enabled = 0;
		SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 0);
	}
}

void HWR_EnableFilter(bool enable)
{
	static bool enabled;

	if (!App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware)
	{
		if (enable)
		{
			if (!enabled)
			{
				enabled = 1;
				SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_MIPNEAREST);
			}
		}
		else if (enabled)
		{
			enabled = 0;
			SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_NEAREST);
		}
	}
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

	if (App.lpZBuffer)
	{
		for (int i = 0; i < 6; i++)
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

void HWR_DrawRoutines(long nVtx, D3DTLVERTEX* vtx, long nDrawType, long TPage)
{
	switch (nDrawType)
	{
	case DT_POLY_GT:
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorKey(0);
		HWR_SetCurrentTexture(TPages[TPage]);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_WGT:
		HWR_SetCurrentTexture(TPages[TPage]);
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

	case DT_POLY_GA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorAddition(0);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_WGTA:
		HWR_EnableZBuffer(0, 1);
		HWR_SetCurrentTexture(TPages[TPage]);
		HWR_EnableColorAddition(1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

#ifdef TROYESTUFF
	case DT_POLY_COLSUB:
		HWR_EnableZBuffer(0, 1);
		HWR_SetCurrentTexture(TPages[TPage]);
		HWR_EnableColorSubtraction(1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;
#endif

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
		HWR_SetCurrentTexture(TPages[TPage]);
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorKey(0);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_WGT:
		HWR_SetCurrentTexture(TPages[TPage]);
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

	case DT_POLY_GA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorAddition(0);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 0);
		return;

	case DT_POLY_WGTA:
		HWR_SetCurrentTexture(TPages[TPage]);
		HWR_EnableColorAddition(1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 0);
		return;

#ifdef TROYESTUFF
	case DT_POLY_COLSUB:
		HWR_SetCurrentTexture(TPages[TPage]);
		HWR_EnableColorSubtraction(1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		SetRenderState(D3DRENDERSTATE_STIPPLEDALPHA, 0);
		return;
#endif

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
		HWR_SetCurrentTexture(TPages[TPage]);
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorKey(0);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_WGT:
		HWR_SetCurrentTexture(TPages[TPage]);
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

	case DT_POLY_GA:
		HWR_SetCurrentTexture(0);
		HWR_EnableAlphaBlend(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

	case DT_POLY_WGTA:
		HWR_SetCurrentTexture(TPages[TPage]);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;

#ifdef TROYESTUFF
	case DT_POLY_COLSUB:	//idk
		HWR_SetCurrentTexture(TPages[TPage]);
		HWR_EnableColorSubtraction(1);
		HWR_EnableAlphaBlend(1);
		HWR_EnableColorKey(1);
		DrawPrimitive(dpPrimitiveType, D3DVT_TLVERTEX, vtx, nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
		return;
#endif
	}
}

#ifdef TROYESTUFF
__inline void HWR_InitGamma(float gamma)
{
	gamma = 1.0F / (gamma / 10.0F * 4.0F);

	for (int i = 0; i < 256; i++)
		ColorTable[i] = uchar(pow((double)i / 256.0F, gamma) * 256.0F);
}
#endif

void HWR_InitState()
{
	DIRECT3DINFO* d3dinfo;
	bool blendOne, stippledAlpha, blendAlpha;

#ifdef TROYESTUFF
	if (tomb3.disable_gamma)
		GammaOption = 2.5F;
#endif

	HWR_InitGamma(GammaOption);		//og has the code directly here

	if (!SetRenderState)
		return;

	HWR_ResetZBuffer();
	SetRenderState(D3DRENDERSTATE_FILLMODE, HWConfig.nFillMode);
	SetRenderState(D3DRENDERSTATE_SHADEMODE, HWConfig.nShadeMode);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAG, HWConfig.nFilter);
	SetRenderState(D3DRENDERSTATE_TEXTUREMIN, HWConfig.nFilter);
	SetRenderState(D3DRENDERSTATE_DITHERENABLE, HWConfig.Dither);
	HWR_EnablePerspCorrect(HWConfig.Perspective);
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

	d3dinfo = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D];

	if (!d3dinfo->bHardware)
	{
		GlobalAlpha = 0xFF000000;
		DrawRoutine = HWR_DrawRoutines;
		return;
	}

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
#ifdef TROYESTUFF
	long x0, y0, x1, y1, bar, p;
#endif
	short nVtx, nDrawType, TPage;

	dpPrimitiveType = D3DPT_TRIANGLEFAN;

	if (App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware)
	{
		for (int i = 0; i < num; i++)
		{
			pInfo = (short*)pSort[0];

#ifdef TROYESTUFF
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
#endif

			nDrawType = pInfo[0];
			TPage = pInfo[1];
			nVtx = pInfo[2];
			vtx = *((D3DTLVERTEX**)(pInfo + 3));
			DrawRoutine(nVtx, vtx, nDrawType, TPage);
			pSort += 3;
		}
	}
	else
	{
		for (int i = 0; i < num; i++)
		{
			pInfo = (short*)pSort[0];
			polyType = pSort[2];
			nDrawType = pInfo[0];
			TPage = pInfo[1];
			nVtx = pInfo[2];
			vtx = *((D3DTLVERTEX**)(pInfo + 3));

			switch (polyType)
			{
			case 1:
			case 2:
			case 5:
				HWR_EnablePerspCorrect(0);
				HWR_EnableFilter(0);
				break;

			case 3:
				HWR_EnablePerspCorrect(1);
				HWR_EnableFilter(1);
				break;

			default:
				HWR_EnablePerspCorrect(1);
				HWR_EnableFilter(0);
				break;
			}

			DrawRoutine(nVtx, vtx, nDrawType, TPage);
			pSort += 3;
		}
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

		if (nDrawType1 == DT_LINE_SOLID)
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
		if (PictureTextures[i].dwFlags & 8)
			DXTextureCleanup(i, PictureTextures);
	}

	if (DXPalette)
	{
		DXPalette->Release();
		DXPalette = 0;
	}
}

void HWR_GetAllTextureHandles()
{
	DXTEXTURE* tex;
	long n;

	memset(TPages, 0, sizeof(TPages));
	n = 0;

	for (int i = 0; i < MAX_TPAGES; i++)
	{
		tex = DXRestoreSurfaceIfLost(i, PictureTextures);

		if (tex->dwFlags & 8)
		{
			TPages[n] = tex;
			n++;
		}
	}

	for (int i = 0; i < MAX_TPAGES; i++)
	{
		tex = DXRestoreSurfaceIfLost(i, PictureTextures);

		if (tex->dwFlags & 16)
		{
			TPages[n] = tex;
			n++;
		}
	}
}

void HWR_LoadTexturePages(long nPages, uchar* src, uchar* palette)
{
	HWR_FreeTexturePages();

	if (palette)
	{
		DXTextureNewPalette(palette);
		DXFreeTPages();
		DXCreateMaxTPages(1);
	}

	for (int i = 0; i < nPages; i++)
	{
		if (palette)
		{
			DXTextureAddPal(256, 256, src, PictureTextures, 8);
			src += 0x10000;
		}
		else
		{
			DXTextureAdd(256, 256, src, PictureTextures, 555, 8);
			src += 0x20000;
		}
	}

	HWR_GetAllTextureHandles();

	for (int i = 0; i < nTPages; i++)
		HWR_SetCurrentTexture(TPages[i]);
}

void HWR_SetCurrentTexture(DXTEXTURE* tex)
{
	TEXTURE* tdata;
	TEXTURE* temp;
	LPDIRECT3DTEXTUREX d3dtex;
	D3DTEXTUREHANDLE handle;
	static D3DTEXTUREHANDLE lastTextureHandle;
	ulong n;

	handle = 0;

	if (App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware)
	{
		if (!nTPages)
			return;

		if (tex)
		{
			for (int i = 0; i < MAX_TPAGES; i++)
			{
				if (Textures[i].DXTex == tex)
				{
					handle = Textures[i].handle;
					Textures[i].nFrames = App.nFrames;
					break;
				}
			}

			if (!handle)
			{
				n = 0;
				tdata = (TEXTURE*)tex;

				for (int i = 0; i < nTPages; i++)
				{
					temp = &Textures[i];

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
	}
	else if (tex)
		SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, (ulong)tex->pData);
	else
		SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, 0);

	if (handle != lastTextureHandle)
	{
		SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, handle);
		lastTextureHandle = handle;
	}
}

void inject_hwrender(bool replace)
{
	INJECT(0x00484E20, HWR_EnableZBuffer, replace);
	INJECT(0x00484DE0, HWR_EnableColorKey, replace);
	INJECT(0x00484BF0, HWR_EnableAlphaBlend, replace);
	INJECT(0x00484A80, HWR_EnableColorAddition, replace);
	INJECT(0x00484A40, HWR_ResetZBuffer, replace);
	INJECT(0x00484A20, HWR_ResetColorKey, replace);
	INJECT(0x00484AE0, HWR_EnablePerspCorrect, replace);
	INJECT(0x00484B70, HWR_EnableFilter, replace);
	INJECT(0x00484A10, HWR_ResetCurrentTexture, replace);
	INJECT(0x00484EB0, HWR_BeginScene, replace);
	INJECT(0x00484F00, HWR_EndScene, replace);
	INJECT(0x00484F10, HWR_DrawRoutines, replace);
	INJECT(0x00485130, HWR_DrawRoutinesStippledAlpha, replace);
	INJECT(0x00485350, HWR_DrawRoutinesNoAlpha, replace);
	INJECT(0x00484740, HWR_InitState, replace);
	INJECT(0x00485A90, HWR_Init, replace);
	INJECT(0x004854C0, HWR_DrawPolyList, replace);
	INJECT(0x004855C0, HWR_DrawPolyListBF, replace);
	INJECT(0x004859C0, HWR_FreeTexturePages, replace);
	INJECT(0x00485A10, HWR_GetAllTextureHandles, replace);
	INJECT(0x00485900, HWR_LoadTexturePages, replace);
	INJECT(0x00484C30, HWR_SetCurrentTexture, replace);
}
