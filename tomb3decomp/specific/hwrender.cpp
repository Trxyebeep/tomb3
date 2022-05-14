#include "../tomb3/pch.h"
#include "hwrender.h"

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
}
