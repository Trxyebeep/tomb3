#include "../tomb3/pch.h"
#include "dxshell.h"

long BPPToDDBD(long BPP)
{
	switch (BPP)
	{
	case 1:
		return DDBD_1;

	case 2:
		return DDBD_2;

	case 4:
		return DDBD_4;

	case 8:
		return DDBD_8;

	case 16:
		return DDBD_16;

	case 24:
		return DDBD_24;

	case 32:
		return DDBD_32;

	default:
		return 0;
	}
}

bool DXSetVideoMode(LPDIRECTDRAW2 ddx, long w, long h, long bpp)
{
	return ddx->SetDisplayMode(w, h, bpp, 0, 0) == DD_OK;
}

bool DXCreateSurface(LPDIRECTDRAW2 ddx, LPDDSURFACEDESC desc, LPDIRECTDRAWSURFACE3 surf)
{
	LPDIRECTDRAWSURFACE s;
	HRESULT result;

	if (ddx->CreateSurface(desc, &s, 0) != DD_OK)
		return 0;

	result = s->QueryInterface(IID_IDirectDrawSurface3, (LPVOID*)surf);

	if (s)
		s->Release();

	return result == DD_OK;
}

bool DXGetAttachedSurface(LPDIRECTDRAWSURFACE3 surf, LPDDSCAPS caps, LPDIRECTDRAWSURFACE3* attached)
{
	return surf->GetAttachedSurface(caps, attached) == DD_OK;
}

bool DXAddAttachedSurface(LPDIRECTDRAWSURFACE3 surf, LPDIRECTDRAWSURFACE3 attach)
{
	return surf->AddAttachedSurface(attach) == DD_OK;
}

bool DXCreateDirect3DDevice(LPDIRECT3D2 dd3x, GUID guid, LPDIRECTDRAWSURFACE3 surf, LPDIRECT3DDEVICE2* device)
{
	return dd3x->CreateDevice(guid, (LPDIRECTDRAWSURFACE)surf, device) == DD_OK;
}

bool DXCreateViewPort(LPDIRECT3D2 dd3x, LPDIRECT3DDEVICE2 device, long w, long h, LPDIRECT3DVIEWPORT2* lpvp)
{
	D3DVIEWPORT2 vp;

	if (dd3x->CreateViewport(lpvp, 0) != DD_OK)
		return 0;

	if (device->AddViewport(*lpvp) != DD_OK)
		return 0;

	vp.dwSize = sizeof(D3DVIEWPORT2);
	vp.dwX = 0;
	vp.dwY = 0;
	vp.dwWidth = w;
	vp.dwHeight = h;
	vp.dvClipX = -1.0F;
	vp.dvClipY = (float)h / (float)w;
	vp.dvClipWidth = 2.0F;
	vp.dvClipHeight = vp.dvClipY + vp.dvClipY;
	vp.dvMinZ = 0;
	vp.dvMaxZ = 1.0F;

	if ((*lpvp)->SetViewport2(&vp) != DD_OK)
		return 0;

	device->SetCurrentViewport(*lpvp);
	return 1;
}

void inject_dxshell(bool replace)
{
	INJECT(0x0048FDB0, BPPToDDBD, replace);
	INJECT(0x0048FEE0, DXSetVideoMode, replace);
	INJECT(0x0048FF10, DXCreateSurface, replace);
	INJECT(0x0048FF60, DXGetAttachedSurface, replace);
	INJECT(0x0048FF80, DXAddAttachedSurface, replace);
	INJECT(0x0048FFA0, DXCreateDirect3DDevice, replace);
	INJECT(0x0048FFC0, DXCreateViewPort, replace);
}
