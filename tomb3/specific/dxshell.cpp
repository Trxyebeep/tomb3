#include "../tomb3/pch.h"
#include "dxshell.h"
#include "dd.h"
#include "winmain.h"
#include "hwrender.h"
#include "drawprimitive.h"
#include "display.h"
#include "picture.h"
#include "texture.h"
#include "../tomb3/tomb3.h"

static LPDIRECTDRAWX G_ddraw;
static LPDIRECT3DX G_d3d;
static HWND G_hwnd;
static bool MMXSupported;

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

bool DXSetVideoMode(LPDIRECTDRAWX ddx, long w, long h, long bpp)
{
	return ddx->SetDisplayMode(w, h, bpp, 0, 0) == DD_OK;
}

bool DXCreateSurface(LPDIRECTDRAWX ddx, LPDDSURFACEDESCX desc, LPDIRECTDRAWSURFACEX surf)
{
	LPDIRECTDRAWSURFACE s;
	HRESULT result;

	if (ddx->CreateSurface(desc, &s, 0) != DD_OK)
		return 0;

	result = s->QueryInterface(DDSGUID, (LPVOID*)surf);

	if (s)
		s->Release();

	return result == DD_OK;
}

bool DXGetAttachedSurface(LPDIRECTDRAWSURFACEX surf, LPDDSCAPSX caps, LPDIRECTDRAWSURFACEX* attached)
{
	return surf->GetAttachedSurface(caps, attached) == DD_OK;
}

bool DXAddAttachedSurface(LPDIRECTDRAWSURFACEX surf, LPDIRECTDRAWSURFACEX attach)
{
	return surf->AddAttachedSurface(attach) == DD_OK;
}

bool DXCreateDirect3DDevice(LPDIRECT3DX dd3x, GUID guid, LPDIRECTDRAWSURFACEX surf, LPDIRECT3DDEVICEX* device)
{
	return dd3x->CreateDevice(guid, (LPDIRECTDRAWSURFACE)surf, device) == DD_OK;
}

bool DXCreateViewPort(LPDIRECT3DX dd3x, LPDIRECT3DDEVICEX device, long w, long h, LPDIRECT3DVIEWPORTX* lpvp)
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

void DXGetSurfaceDesc(LPDIRECTDRAWSURFACEX surf, LPDDSURFACEDESCX desc)
{
	surf->GetSurfaceDesc(desc);
}

bool DXSurfBlt(LPDIRECTDRAWSURFACEX surf, LPRECT rect, long FillColor)
{
	DDBLTFX bfx;

	memset(&bfx, 0, sizeof(DDBLTFX));
	bfx.dwSize = sizeof(DDBLTFX);
	bfx.dwFillColor = FillColor;
	return surf->Blt(rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &bfx) == DD_OK;
}

void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count)
{
	uchar i;

	for (i = 0; !(mask & 1); i++)
		mask >>= 1;

	*shift = i;

	for (i = 0; mask & 1; i++)
		mask >>= 1;

	*count = i;
}

bool DXCreateDirectDraw(DEVICEINFO* dev, DXCONFIG* conf, LPDIRECTDRAWX* ddx)
{
	LPDIRECTDRAW dd;
	HRESULT result;

	if (DirectDrawCreate(dev->DDInfo[conf->nDD].lpGuid, &dd, 0) != DD_OK)
		return 0;

	result = dd->QueryInterface(DDGUID, (LPVOID*)ddx);

	if (dd)
		dd->Release();

	return result == DD_OK;
}

bool DXCreateDirect3D(LPDIRECTDRAWX ddx, LPDIRECT3DX* d3dx)
{
	return ddx->QueryInterface(D3DGUID, (LPVOID*)d3dx) == DD_OK;
}

bool DXSetCooperativeLevel(LPDIRECTDRAWX ddx, HWND hwnd, long flags)
{
	return ddx->SetCooperativeLevel(hwnd, flags) == DD_OK;
}

__inline void* AddStruct(void* p, long num, long size)	//Note: this function wasn't present/was inlined in the original (taken from TR4)
{
	void* ptr;

	if (!num)
		ptr = malloc(size);
	else
		ptr = realloc(p, size * (num + 1));

	memset((char*)ptr + size * num, 0, size);
	return ptr;
}

HRESULT CALLBACK DXEnumDisplayModes(LPDDSURFACEDESCX lpDDSurfaceDesc, LPVOID lpContext)
{
	DIRECTDRAWINFO* ddinfo;
	DISPLAYMODE* dm;

	ddinfo = (DIRECTDRAWINFO*)lpContext;
	ddinfo->DisplayMode = (DISPLAYMODE*)AddStruct(ddinfo->DisplayMode, ddinfo->nDisplayMode, sizeof(DISPLAYMODE));
	dm = &ddinfo->DisplayMode[ddinfo->nDisplayMode];

	dm->w = lpDDSurfaceDesc->dwWidth;
	dm->h = lpDDSurfaceDesc->dwHeight;
	dm->bpp = lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount;
	dm->bPalette = lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8;
	memcpy(&dm->ddsd, lpDDSurfaceDesc, sizeof(DDSURFACEDESCX));

	if (!dm->bPalette)
	{
		DXBitMask2ShiftCnt(lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask, &dm->rshift, &dm->rbpp);
		DXBitMask2ShiftCnt(lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask, &dm->gshift, &dm->gbpp);
		DXBitMask2ShiftCnt(lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask, &dm->bshift, &dm->bbpp);

		if (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBAlphaBitMask)
			DXBitMask2ShiftCnt(lpDDSurfaceDesc->ddpfPixelFormat.dwRGBAlphaBitMask, &dm->ashift, &dm->abpp);
	}

	ddinfo->nDisplayMode++;
	return DDENUMRET_OK;
}

bool DXCreateZBuffer(DEVICEINFO* device, DXCONFIG* config)
{
	DIRECT3DINFO** dinfopp;
	DDSURFACEDESCX desc;

	if (!config->bZBuffer)
	{
		App.ZBuffer = 0;
		return 1;
	}
	
	dinfopp = &device->DDInfo[config->nDD].D3DInfo;
	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_ZBUFFERBITDEPTH;
	desc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
	desc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	desc.dwWidth = (*dinfopp)[config->nD3D].DisplayMode[config->nVMode].w;
	desc.dwHeight = (*dinfopp)[config->nD3D].DisplayMode[config->nVMode].h;
	desc.dwMipMapCount = 16;

	if (!DXCreateSurface(App.DDraw, &desc, (LPDIRECTDRAWSURFACEX)&App.ZBuffer))
		return 0;

	if (!DXAddAttachedSurface(App.BackBuffer, App.ZBuffer))
		return 0;

	return 1;
}

BOOL CALLBACK DXEnumDirectDraw(GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext)
{
	DEVICEINFO* dinfo;
	DIRECTDRAWINFO* info;
	LPDIRECTDRAW ddraw;

	dinfo = (DEVICEINFO*)lpContext;
	dinfo->DDInfo = (DIRECTDRAWINFO*)AddStruct(dinfo->DDInfo, dinfo->nDDInfo, sizeof(DIRECTDRAWINFO));
	info = &dinfo->DDInfo[dinfo->nDDInfo];

	if (lpGUID)
	{
		info->lpGuid = &info->Guid;
		info->Guid = *lpGUID;
	}
	else
		info->lpGuid = 0;

	lstrcpy(info->About, lpDriverDescription);
	lstrcpy(info->Name, lpDriverName);
	DirectDrawCreate(lpGUID, &ddraw, 0);
	ddraw->QueryInterface(DDGUID, (LPVOID*)&G_ddraw);

	if (ddraw)
	{
		ddraw->Release();
		ddraw = 0;
	}

	memset(&info->DDCaps, 0, sizeof(DDCAPS));
	info->DDCaps.dwSize = sizeof(DDCAPS);
	G_ddraw->GetCaps(&info->DDCaps, 0);
	G_ddraw->SetCooperativeLevel(0, DDSCL_FULLSCREEN | DDSCL_NOWINDOWCHANGES | DDSCL_NORMAL | DDSCL_ALLOWMODEX);
	G_ddraw->EnumDisplayModes(0, 0, (LPVOID)info, DXEnumDisplayModes);
	G_ddraw->QueryInterface(D3DGUID, (LPVOID*)&G_d3d);
	G_d3d->EnumDevices(DXEnumDirect3D, info);
	G_ddraw->SetCooperativeLevel(0, DDSCL_NORMAL);
	G_d3d->Release();
	G_ddraw->Release();
	dinfo->nDDInfo++;
	return DDENUMRET_OK;
}

HRESULT CALLBACK DXEnumTextureFormats(LPDDSURFACEDESCX lpDDPixFmt, LPVOID lpContext)
{
	DIRECT3DINFO* d3dinfo;
	D3DTEXTUREINFO* tex;

	d3dinfo = (DIRECT3DINFO*)lpContext;

	d3dinfo->Texture = (D3DTEXTUREINFO*)AddStruct(d3dinfo->Texture, d3dinfo->nTexture, sizeof(D3DTEXTUREINFO));
	tex = &d3dinfo->Texture[d3dinfo->nTexture];
	memcpy(&tex->ddsd, lpDDPixFmt, sizeof(DDSURFACEDESCX));

	if (lpDDPixFmt->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
	{
		tex->bPalette = 1;
		tex->bpp = 8;
	}
	else if (!(lpDDPixFmt->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4))
	{
		tex->bPalette = 0;
		tex->bpp = lpDDPixFmt->ddpfPixelFormat.dwRGBBitCount;
		DXBitMask2ShiftCnt(lpDDPixFmt->ddpfPixelFormat.dwRBitMask, &tex->rshift, &tex->rbpp);
		DXBitMask2ShiftCnt(lpDDPixFmt->ddpfPixelFormat.dwGBitMask, &tex->gshift, &tex->gbpp);
		DXBitMask2ShiftCnt(lpDDPixFmt->ddpfPixelFormat.dwBBitMask, &tex->bshift, &tex->bbpp);

		if (lpDDPixFmt->ddpfPixelFormat.dwRGBAlphaBitMask)
		{
			DXBitMask2ShiftCnt(lpDDPixFmt->ddpfPixelFormat.dwRGBAlphaBitMask, &tex->ashift, &tex->abpp);
			tex->bAlpha = 1;
		}
	}

	d3dinfo->nTexture++;
	return D3DENUMRET_OK;
}

BOOL CALLBACK DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
	DEVICEINFO* device;
	DXDIRECTSOUNDINFO* sinfo;

	device = (DEVICEINFO*)lpContext;
	device->DSInfo = (DXDIRECTSOUNDINFO*)AddStruct(device->DSInfo, device->nDSInfo, sizeof(DXDIRECTSOUNDINFO));
	sinfo = &device->DSInfo[device->nDSInfo];

	if (lpGuid)
	{
		sinfo->lpGuid = &sinfo->Guid;
		sinfo->Guid = *lpGuid;
	}
	else
		sinfo->lpGuid = 0;

	lstrcpy(sinfo->About, lpcstrDescription);
	lstrcpy(sinfo->Name, lpcstrModule);
	device->nDSInfo++;
	return 1;
}

void DXFreeDeviceInfo(DEVICEINFO* device)
{
	DIRECTDRAWINFO* dinfo;

	for (int i = 0; i < device->nDDInfo; i++)
	{
		dinfo = &device->DDInfo[i];

		for (int j = 0; j < dinfo->nD3DInfo; j++)
		{
			free(dinfo->D3DInfo[j].DisplayMode);
			free(dinfo->D3DInfo[j].Texture);
		}

		free(dinfo->D3DInfo);
		free(dinfo->DisplayMode);
	}

	free(device->DDInfo);

	if (device->DSInfo)
		free(device->DSInfo);

	memset(device, 0, sizeof(DEVICEINFO));
}

void DXSaveScreen(LPDIRECTDRAWSURFACEX surf)
{
	FILE* file;
	DDSURFACEDESCX desc;
	ushort* pSurf;
	short* pDest;
	char* pM;
	long r, g, b;
	static long num;
	ushort c;
	char buf[16];
	static char tga_header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 1, 0, 1, 16, 0 };

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);

	if (FAILED(surf->GetSurfaceDesc(&desc)))
		return;

	if (FAILED(surf->Lock(0, &desc, DDLOCK_WAIT, 0)))
		return;

	pSurf = (ushort*)desc.lpSurface;
	sprintf(buf, "tomb%04d.tga", num);
	num++;
	file = fopen(buf, "wb");

	if (file)
	{
		*(short*)&tga_header[12] = (short)desc.dwWidth;
		*(short*)&tga_header[14] = (short)desc.dwHeight;
		fwrite(tga_header, sizeof(tga_header), 1, file);
		pM = (char*)malloc(2 * desc.dwWidth * desc.dwHeight);
		pDest = (short*)pM;
		pSurf += desc.dwHeight * (desc.lPitch / 2);

		for (ulong h = 0; h < desc.dwHeight; h++)
		{
			for (ulong w = 0; w < desc.dwWidth; w++)
			{
				c = pSurf[w];

				if (desc.ddpfPixelFormat.dwRBitMask == 0xF800)
				{
					r = (c >> 11) & 0x1F;
					g = (c >> 6) & 0x1F;
					b = c & 0x1F;
					*pDest++ = short(r << 10 | g << 5 | b);
				}
				else
					*pDest++ = (short)c;
			}

			pSurf -= desc.lPitch / 2;
		}

		fwrite(pM, 2 * desc.dwWidth * desc.dwHeight, 1, file);
		free(pM);
		fclose(file);

		buf[7]++;

		if (buf[7] > '9')
		{
			buf[7] = '0';
			buf[6]++;
		}
	}

	DD_UnlockSurface(surf, desc);
}

void DXDoFlipWait()
{
	while (App.FrontBuffer->GetFlipStatus(DDGFS_ISFLIPDONE) == DDERR_WASSTILLDRAWING);
}

bool DXCheckForLostSurfaces()
{
	bool pass;

	if (!App.FrontBuffer)
		S_ExitSystem("Oops... no front buffer");

	pass = SUCCEEDED(DD_EnsureSurfaceAvailable(App.FrontBuffer, 0, 1)) ||
		SUCCEEDED(DD_EnsureSurfaceAvailable(App.BackBuffer, App.FrontBuffer, 1));

	if (App.ZBuffer)
		pass = pass || SUCCEEDED(DD_EnsureSurfaceAvailable(App.ZBuffer, 0, 0));

	pass = pass || SUCCEEDED(DD_EnsureSurfaceAvailable(App.PictureBuffer, 0, 0));

	if (pass && !GtWindowClosed)
		HWR_GetAllTextureHandles();

	return pass;
}

void DXClearBuffers(ulong flags, ulong color)
{
	DISPLAYMODE* dm;
	RECT r;
	D3DRECT vr;
	ulong sflags;

	dm = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].DisplayMode[App.lpDXConfig->nVMode];
	r.top = 0;
	r.left = 0;
	r.right = dm->w;
	r.bottom = dm->h;

	sflags = 0;

	if (flags & 2)
		sflags = 1;

	if (flags & 8)
		sflags |= 2;

	if (sflags)
	{
		vr.x1 = 0;
		vr.y1 = 0;
		vr.x2 = dm->w;
		vr.y2 = dm->h;
		App.D3DView->Clear(1, &vr, sflags);
	}

	if (flags & 1)
		DD_ClearSurface(App.FrontBuffer, &r, color);

	if (flags & 0x20)
	{
		r.top = 0;
		r.left = 0;
		r.right = 640;
		r.bottom = 480;
		DD_ClearSurface(App.PictureBuffer, &r, color);
	}
}

bool DXUpdateFrame(bool runMessageLoop, LPRECT rect)
{
	DIRECT3DINFO* d3dinfo;
	ulong w;

	App.nFrames++;
	DXCheckForLostSurfaces();
	d3dinfo = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D];
	w = d3dinfo->DisplayMode[App.lpDXConfig->nVMode].w;

	if (tomb3.Windowed)
		App.FrontBuffer->Blt(&tomb3.rScreen, App.BackBuffer, &tomb3.rViewport, DDBLT_WAIT, 0);
	else
		App.FrontBuffer->Flip(0, DDFLIP_WAIT);

	if (runMessageLoop)
		return DD_SpinMessageLoop(0);

	return 1;
}

void DXGetDeviceInfo(DEVICEINFO* device, HWND hWnd, HINSTANCE hInstance)
{
	MMXSupported = 1;
	DirectDrawEnumerate(DXEnumDirectDraw, device);
	DirectSoundEnumerate(DXEnumDirectSound, device);
	//Original joystick enumeration stuff was here
}

HRESULT CALLBACK DXEnumDirect3D(LPGUID lpGuid, LPSTR description, LPSTR name, LPD3DDEVICEDESC lpHWDesc, LPD3DDEVICEDESC lpHELDesc, LPVOID lpContext)
{
	DIRECTDRAWINFO* ddinfo;
	DIRECT3DINFO* d3dinfo;
	static LPDIRECTDRAWSURFACEX surf;
	static LPDIRECT3DDEVICEX d3dDevice;
	DDSURFACEDESCX desc;

	ddinfo = (DIRECTDRAWINFO*)lpContext;

	if (!lpHWDesc->dwFlags)		//disable software
		return D3DENUMRET_OK;

	if (lpHWDesc->dwFlags && !lpHWDesc->dpcTriCaps.dwTextureCaps)
		return D3DENUMRET_OK;

	ddinfo->D3DInfo = (DIRECT3DINFO*)AddStruct(ddinfo->D3DInfo, ddinfo->nD3DInfo, sizeof(DIRECT3DINFO));
	d3dinfo = &ddinfo->D3DInfo[ddinfo->nD3DInfo];

	if (lpGuid)
	{
		d3dinfo->lpGuid = &d3dinfo->Guid;
		d3dinfo->Guid = *lpGuid;
	}
	else
		d3dinfo->lpGuid = 0;

	lstrcpy(d3dinfo->About, description);
	lstrcpy(d3dinfo->Name, name);
	memcpy(&d3dinfo->DeviceDesc, lpHWDesc, sizeof(D3DDEVICEDESC));

	d3dinfo->bAlpha = d3dinfo->DeviceDesc.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_ALPHAFLATBLEND;

	for (int i = 0; i < ddinfo->nDisplayMode; i++)
	{
		if (!(BPPToDDBD(ddinfo->DisplayMode[i].bpp) & d3dinfo->DeviceDesc.dwDeviceRenderBitDepth))
			continue;

		d3dinfo->DisplayMode = (DISPLAYMODE*)AddStruct(d3dinfo->DisplayMode, d3dinfo->nDisplayMode, sizeof(DISPLAYMODE));
		memcpy(&d3dinfo->DisplayMode[d3dinfo->nDisplayMode], &ddinfo->DisplayMode[i], sizeof(DISPLAYMODE));
		d3dinfo->nDisplayMode++;
	}

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	desc.dwFlags = DDSD_CAPS;
	desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;
	surf = 0;
	DXSetCooperativeLevel(G_ddraw, G_hwnd, DDSCL_FULLSCREEN | DDSCL_NOWINDOWCHANGES | DDSCL_EXCLUSIVE);
	DXCreateSurface(G_ddraw, &desc, (LPDIRECTDRAWSURFACE3)&surf);

	if (surf)
	{
		d3dDevice = 0;
		DXCreateDirect3DDevice(G_d3d, ddinfo->D3DInfo[ddinfo->nD3DInfo].Guid, surf, &d3dDevice);

		if (!d3dDevice)	//fail
		{
			if (surf)
			{
				surf->Release();
				surf = 0;
			}

			DXSetVideoMode(G_ddraw, d3dinfo->DisplayMode->w, d3dinfo->DisplayMode->h, d3dinfo->DisplayMode->bpp);
			DXCreateSurface(G_ddraw, &desc, (LPDIRECTDRAWSURFACE3)&surf);

			if (surf)
				DXCreateDirect3DDevice(G_d3d, d3dinfo->Guid, surf, &d3dDevice);
		}

		if (d3dDevice)	//did it work?
		{
			d3dinfo->nTexture = 0;
			d3dDevice->EnumTextureFormats(DXEnumTextureFormats, (LPVOID)d3dinfo);

			if (d3dDevice)
			{
				d3dDevice->Release();
				d3dDevice = 0;
			}
		}

		if (surf)
		{
			surf->Release();
			surf = 0;
		}

		DXSetCooperativeLevel(G_ddraw, G_hwnd, DDSCL_NORMAL);
	}

	ddinfo->nD3DInfo++;
	return D3DENUMRET_OK;
}

bool DXSwitchVideoMode(long needed, long current, bool disableZBuffer)
{
	DIRECT3DINFO* d3dinfo;
	DISPLAYMODE* dm;
	ulong currentBpp;
	bool change;

	d3dinfo = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D];

	dm = &d3dinfo->DisplayMode[App.lpDXConfig->nVMode];
	currentBpp = dm->bpp;

	App.DXConfig.nVMode = needed;
	dm = &d3dinfo->DisplayMode[App.lpDXConfig->nVMode];
	change = 1;

	if (needed != current && dm->bpp != currentBpp)
	{
		change = 0;

		if (needed > current)
		{
			while (++needed <= d3dinfo->nDisplayMode)
			{
				App.DXConfig.nVMode = needed;
				dm = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].DisplayMode[App.lpDXConfig->nVMode];

				if (dm->bpp == currentBpp)
				{
					change = 1;
					break;
				}
			}
		}
		else
		{
			while (--needed >= 0)
			{
				App.DXConfig.nVMode = needed;
				dm = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].DisplayMode[App.lpDXConfig->nVMode];

				if (dm->bpp == currentBpp)
				{
					change = 1;
					break;
				}
			}
		}
	}

	if (!change)
	{
		App.DXConfig.nVMode = current;	//abort!
		return 0;
	}

	DXSurfBlt(App.FrontBuffer, 0, 0);

	for (int i = 0; i < MAX_TPAGES; i++)
		PictureTextures[i].tex = 0;

	WinFreeDX(0);

	if (WinDXInit(App.lpDeviceInfo, App.lpDXConfig, 0))
		change = 1;
	else
	{
		WinFreeDX(0);
		App.DXConfig.nVMode = current;

		if (disableZBuffer)
			App.lpDXConfig->bZBuffer = 0;

		WinDXInit(App.lpDeviceInfo, App.lpDXConfig, 0);
		change = 0;
	}

	HWR_GetAllTextureHandles();

	for (int i = 0; i < nTPages; i++)
		HWR_SetCurrentTexture(TPages[i]);

	HWR_InitState();
	setup_screen_size();
	return change;
}

long DXToggleFullScreen()
{
	if (tomb3.Windowed)
	{
		Log("DXToggleFullScreen: Switching to Fullscreen");
		tomb3.Windowed = 0;
	}
	else
	{
		Log("DXToggleFullScreen: Switching to Windowed");
		tomb3.Windowed = 1;
	}

	if (WinDXInit(&App.DeviceInfo, &App.DXConfig, 0))
	{
		Log("DXToggleFullScreen: Switched successfully");
		WinSetStyle(!tomb3.Windowed, tomb3.WindowStyle);
		return 1;
	}

	Log("DXToggleFullScreen: Switching failed, try to revert");
	tomb3.Windowed = !tomb3.Windowed;

	if (WinDXInit(&App.DeviceInfo, &App.DXConfig, 0))
	{
		Log("DXToggleFullScreen: reverted fine");
		return 0;
	}

	S_ExitSystem("Failed to reinit DX after DXToggleFullScreen");
	return -1;
}

void DXMove(long x, long y)
{
	DISPLAYMODE* dm;

	if (!tomb3.Windowed)
		return;

	dm = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].DisplayMode[App.lpDXConfig->nVMode];
	SetRect(&tomb3.rScreen, x, y, x + dm->w, y + dm->h);
}

bool DXStartRenderer(DEVICEINFO* device, DXCONFIG* config, bool createNew, bool windowed)
{
	DISPLAYMODE* dm;
	DIRECT3DINFO* d3d;
	DDSURFACEDESCX desc;
	D3DMATERIALX m;
	DDSCAPSX caps;
	D3DMATERIALHANDLE handle;
	LPDIRECTDRAWCLIPPER clipper;
	RECT r;
	HWND desktop;
	DEVMODE dev;
	HDC hDC;

	Log("Starting DXStartRenderer");

	if (createNew)
	{
		if (!DXCreateDirectDraw(device, config, &App.DDraw) || !DXCreateDirect3D(App.DDraw, &App.D3D))
		{
			Log("Failed to create DirectDraw or Direct3D, exitting..");
			return 0;
		}
	}

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	dm = &device->DDInfo[config->nDD].D3DInfo[config->nD3D].DisplayMode[config->nVMode];

	if (windowed)
	{
		Log("Creating windowed");

		if (!DXSetCooperativeLevel(App.DDraw, App.WindowHandle, DDSCL_NORMAL))
		{
			Log("DXSetCooperativeLevel failed: DDSCL_NORMAL, exitting..");
			return 0;
		}

		desktop = GetDesktopWindow();
		hDC = GetDC(desktop);
		ReleaseDC(desktop, hDC);
		dev.dmBitsPerPel = dm->bpp;
		dev.dmSize = sizeof(DEVMODE);
		dev.dmFields = DM_BITSPERPEL;
		ChangeDisplaySettings(&dev, 0);

		d3d = &device->DDInfo[config->nDD].D3DInfo[config->nD3D];
		dm = &d3d->DisplayMode[config->nVMode];
		r.top = 0;
		r.left = 0;
		r.right = dm->w;
		r.bottom = dm->h;
		AdjustWindowRect(&r, tomb3.WindowStyle, 0);
		SetWindowPos(App.WindowHandle, 0, 0, 0, r.right - r.left, r.bottom - r.top, SWP_NOMOVE | SWP_NOZORDER);
		GetClientRect(App.WindowHandle, &tomb3.rViewport);
		GetClientRect(App.WindowHandle, &tomb3.rScreen);
		ClientToScreen(App.WindowHandle, (LPPOINT)&tomb3.rScreen);
		ClientToScreen(App.WindowHandle, (LPPOINT)&tomb3.rScreen.right);
		desc.dwFlags = DDSD_CAPS;
		desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		if (!DXCreateSurface(App.DDraw, &desc, (LPDIRECTDRAWSURFACEX)&App.FrontBuffer))
		{
			Log("DXCreateSurface failed to create front buffer (windowed mode), exitting..");
			return 0;
		}

		if (FAILED(App.DDraw->CreateClipper(0, &clipper, 0)))
		{
			Log("Failed to CreateClipper");
			return 0;
		}

		clipper->SetHWnd(0, App.WindowHandle);
		App.FrontBuffer->SetClipper(clipper);
		clipper->Release();
		clipper = 0;

		desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		desc.dwWidth = dm->w;
		desc.dwHeight = dm->h;
		desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

		if (!DXCreateSurface(App.DDraw, &desc, (LPDIRECTDRAWSURFACEX)&App.BackBuffer))
		{
			Log("DXCreateSurface failed to create back buffer (windowed mode), exitting..");
			return 0;
		}
	}
	else
	{
		Log("Creating Fullscreen");

		if (!DXSetCooperativeLevel(App.DDraw, App.WindowHandle, DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE))
		{
			Log("DXSetCooperativeLevel failed: DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE, exitting..");
			return 0;
		}

		if (!DXSetVideoMode(App.DDraw, dm->w, dm->h, dm->bpp))
		{
			Log("DXSetVideoMode failed, exitting..");
			return 0;
		}

		desc.dwBackBufferCount = 1;
		desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		desc.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

		if (!DXCreateSurface(App.DDraw, &desc, (LPDIRECTDRAWSURFACEX)&App.FrontBuffer))
		{
			Log("DXCreateSurface failed to create front buffer, exitting..");
			return 0;
		}

		App.FrontBuffer->GetSurfaceDesc(&desc);
		d3d = &device->DDInfo[config->nDD].D3DInfo[config->nD3D];
		dm = &d3d->DisplayMode[config->nVMode];

		DXBitMask2ShiftCnt(desc.ddpfPixelFormat.dwRBitMask, &dm->rshift, &dm->rbpp);
		DXBitMask2ShiftCnt(desc.ddpfPixelFormat.dwGBitMask, &dm->gshift, &dm->gbpp);
		DXBitMask2ShiftCnt(desc.ddpfPixelFormat.dwBBitMask, &dm->bshift, &dm->bbpp);

		caps.dwCaps = DDSCAPS_BACKBUFFER;

		if (!DXGetAttachedSurface(App.FrontBuffer, &caps, &App.BackBuffer))
		{
			Log("DXGetAttachedSurface failed to get back buffer, exitting..");
			return 0;
		}

		tomb3.rViewport.top = 0;
		tomb3.rViewport.left = 0;
		tomb3.rViewport.right = dm->w;
		tomb3.rViewport.bottom = dm->h;
	}

	if (!DXCreateZBuffer(device, config))
	{
		Log("DXCreateZBuffer failed, exitting..");
		return 0;
	}

	if (!DXCreateDirect3DDevice(App.D3D, d3d->Guid, App.BackBuffer, &App.D3DDev))
	{
		Log("DXCreateDirect3DDevice failed, exitting..");
		return 0;
	}

	dm = &device->DDInfo[config->nDD].D3DInfo[config->nD3D].DisplayMode[config->nVMode];

	if (!DXCreateViewPort(App.D3D, App.D3DDev, dm->w, dm->h, &App.D3DView))
	{
		Log("DXCreateViewPort failed, exitting..");
		return 0;
	}

	memset(&m, 0, sizeof(D3DMATERIALX));
	m.dwSize = sizeof(D3DMATERIALX);

	App.D3D->CreateMaterial(&App.D3DMaterial, 0);
	App.D3DMaterial->SetMaterial(&m);
	App.D3DMaterial->GetHandle(App.D3DDev, &handle);
	App.D3DView->SetBackground(handle);

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	desc.dwWidth = 640;
	desc.dwHeight = 480;
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	DXCreateSurface(App.DDraw, &desc, (LPDIRECTDRAWSURFACEX)&App.PictureBuffer);
	DXClearBuffers(11, 0);
	InitDrawPrimitive(App.D3DDev, App.BackBuffer);
	HWR_InitState();
	DXCreateMaxTPages(1);

	if (!nTPages)
	{
		Log("nTPages is 0, DXCreateMaxTPages failed, exitting..");
		return 0;
	}

	Log("DXStartRenderer finished successfully");
	return 1;
}
