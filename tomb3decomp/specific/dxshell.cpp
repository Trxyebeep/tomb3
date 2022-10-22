#include "../tomb3/pch.h"
#include "dxshell.h"
#include "dd.h"
#include "winmain.h"
#include "hwrender.h"
#include "mmx.h"

//statics
#define G_ddraw	VAR_(0x006CA0F8, LPDIRECTDRAW2)
#define G_d3d	VAR_(0x006CA100, LPDIRECT3D2)
#define SoftwareRenderer	VAR_(0x006CA104, bool)

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

void DXGetSurfaceDesc(LPDIRECTDRAWSURFACE3 surf, LPDDSURFACEDESC desc)
{
	surf->GetSurfaceDesc(desc);
}

bool DXSurfBlt(LPDIRECTDRAWSURFACE3 surf, LPRECT rect, long FillColor)
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

bool DXCreateDirectDraw(DEVICEINFO* dev, DXCONFIG* conf, LPDIRECTDRAW2* ddx)
{
	LPDIRECTDRAW dd;
	HRESULT result;

	if (DirectDrawCreate(dev->DDInfo[conf->nDD].lpGuid, &dd, 0) != DD_OK)
		return 0;

	result = dd->QueryInterface(IID_IDirectDraw2, (LPVOID*)ddx);

	if (dd)
		dd->Release();

	return result == DD_OK;
}

bool DXCreateDirect3D(LPDIRECTDRAW2 ddx, LPDIRECT3D2* d3dx)
{
	return ddx->QueryInterface(IID_IDirect3D2, (LPVOID*)d3dx) == DD_OK;
}

bool DXSetCooperativeLevel(LPDIRECTDRAW2 ddx, HWND hwnd, long flags)
{
	return ddx->SetCooperativeLevel(hwnd, flags) == DD_OK;
}

__inline void* AddStruct(void* p, long num, long size)	//Note: this function wasn't present/was inlined in the original (taken from TR4)
{
	void* ptr;

	if (!num)
		ptr = MALLOC(size);
	else
		ptr = REALLOC(p, size * (num + 1));

	memset((char*)ptr + size * num, 0, size);
	return ptr;
}

BOOL CALLBACK DXEnumDirectInput(LPCDIDEVICEINSTANCE lpDevInst, LPVOID lpContext)
{
	DEVICEINFO* dinfo;
	DXDIRECTINPUTINFO* info;

	dinfo = (DEVICEINFO*)lpContext;
	dinfo->DIInfo = (DXDIRECTINPUTINFO*)AddStruct(dinfo->DIInfo, dinfo->nDIInfo, sizeof(DXDIRECTINPUTINFO));
	info = &dinfo->DIInfo[dinfo->nDIInfo];

	if (lpDevInst == (LPCDIDEVICEINSTANCE)-4)	//todo, fix me: properly check if guidInstance is valid
		info->lpGuid = 0;
	else
	{
		info->lpGuid = &info->Guid;
		info->Guid = lpDevInst->guidInstance;
	}

	lstrcpy(info->About, lpDevInst->tszProductName);
	lstrcpy(info->Name, lpDevInst->tszInstanceName);
	dinfo->nDIInfo++;
	return DIENUM_CONTINUE;
}

HRESULT CALLBACK DXEnumDisplayModes(LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext)
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
	memcpy(&dm->ddsd, lpDDSurfaceDesc, sizeof(DDSURFACEDESC));

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
	DDSURFACEDESC desc;

	if (!config->bZBuffer)
	{
		App.lpZBuffer = 0;
		return 1;
	}
	
	dinfopp = &device->DDInfo[config->nDD].D3DInfo;
	memset(&desc, 0, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_ZBUFFERBITDEPTH;
	desc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;

	if ((*dinfopp)[config->nD3D].bHardware)
		desc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	else
		desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	desc.dwWidth = (*dinfopp)[config->nD3D].DisplayMode[config->nVMode].w;
	desc.dwHeight = (*dinfopp)[config->nD3D].DisplayMode[config->nVMode].h;
	desc.dwMipMapCount = 16;

	if (!DXCreateSurface(App.lpDD, &desc, (LPDIRECTDRAWSURFACE3)&App.lpZBuffer))
		return 0;

	if (!DXAddAttachedSurface(App.lpBackBuffer, App.lpZBuffer))
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
	ddraw->QueryInterface(IID_IDirectDraw2, (LPVOID*)&G_ddraw);

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
	G_ddraw->QueryInterface(IID_IDirect3D2, (LPVOID*)&G_d3d);
	SoftwareRenderer = 0;
	G_d3d->EnumDevices(DXEnumDirect3D, info);
	G_ddraw->SetCooperativeLevel(0, DDSCL_NORMAL);
	G_d3d->Release();
	G_ddraw->Release();
	dinfo->nDDInfo++;
	return DDENUMRET_OK;
}

HRESULT CALLBACK DXEnumTextureFormats(LPDDSURFACEDESC lpDDPixFmt, LPVOID lpContext)
{
	DIRECT3DINFO* d3dinfo;
	D3DTEXTUREINFO* tex;

	d3dinfo = (DIRECT3DINFO*)lpContext;

	d3dinfo->Texture = (D3DTEXTUREINFO*)AddStruct(d3dinfo->Texture, d3dinfo->nTexture, sizeof(D3DTEXTUREINFO));
	tex = &d3dinfo->Texture[d3dinfo->nTexture];
	memcpy(&tex->ddsd, lpDDPixFmt, sizeof(DDSURFACEDESC));

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

HRESULT CALLBACK DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
	DEVICEINFO* device;
	DXDIRECTSOUNDINFO* sinfo;

	device = (DEVICEINFO*)lpContext;
	device->DSInfo = (DXDIRECTSOUNDINFO*)AddStruct(device->DSInfo, device->nDDInfo, sizeof(DXDIRECTSOUNDINFO));
	sinfo = &device->DSInfo[device->nDDInfo];

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
			FREE(dinfo->D3DInfo[j].DisplayMode);
			FREE(dinfo->D3DInfo[j].Texture);
		}

		FREE(dinfo->D3DInfo);
		FREE(dinfo->DisplayMode);
	}

	FREE(device->DDInfo);

	if (device->DSInfo)
		FREE(device->DSInfo);

	if (device->DIInfo)
		FREE(device->DIInfo);

	memset(device, 0, sizeof(DEVICEINFO));
}

void DXSaveScreen(LPDIRECTDRAWSURFACE3 surf)
{
	FILE* file;
	DDSURFACEDESC desc;
	ushort* pSurf;
	short* pDest;
#ifdef TROYESTUFF
	char* pM;
#endif
	long r, g, b;
	static long num;
	ushort c;
	char buf[16];
	static char tga_header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 1, 0, 1, 16, 0 };

	memset(&desc, 0, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);

	if (FAILED(surf->GetSurfaceDesc(&desc)))
		return;

	memset(&desc, 0, sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);

	if (FAILED(DD_LockSurface(surf, desc, DDLOCK_WAIT | DDLOCK_WRITEONLY)))
		return;

	pSurf = (ushort*)desc.lpSurface;
	num++;
	sprintf(buf, "tomb%04d.tga", num);
	file = fopen(buf, "wb");

	if (file)
	{
		*(short*)&tga_header[12] = (short)desc.dwWidth;
		*(short*)&tga_header[14] = (short)desc.dwHeight;
		fwrite(tga_header, sizeof(tga_header), 1, file);

#ifdef TROYESTUFF
		pM = (char*)malloc(2 * desc.dwWidth * desc.dwHeight);
		pDest = (short*)pM;
#else
		pDest = (short*)malloc_ptr;
#endif
		pSurf += desc.dwHeight * (desc.lPitch / 2);

		for (ulong h = 0; h < desc.dwHeight; h++)
		{
			for (ulong w = 0; w < desc.dwWidth; w++)
			{
				c = pSurf[w];

				if (desc.ddpfPixelFormat.dwRBitMask == 0xF800)
				{
					r = c >> 11;
					g = (c >> 6) & 0x1F;
					b = c & 0x1F;
					*pDest++ = short(r << 10 | g << 5 | b);
				}
				else
					*pDest++ = (short)c;
			}

			pSurf -= desc.lPitch / 2;
		}

#ifdef TROYESTUFF
		fwrite(pM, 2 * desc.dwWidth * desc.dwHeight, 1, file);
		free(pM);
#else
		fwrite(malloc_ptr, 2 * desc.dwWidth * desc.dwHeight, 1, file);
#endif
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
	while (App.lpFrontBuffer->GetFlipStatus(DDGFS_ISFLIPDONE) == DDERR_WASSTILLDRAWING);
}

bool DXCheckForLostSurfaces()
{
	bool pass;

	if (!App.lpFrontBuffer)
		S_ExitSystem("Oops... no front buffer");

	pass = SUCCEEDED(DD_EnsureSurfaceAvailable(App.lpFrontBuffer, 0, 1)) ||
		SUCCEEDED(DD_EnsureSurfaceAvailable(App.lpBackBuffer, App.lpFrontBuffer, 1));

	if (App.lpZBuffer)
		pass = pass || SUCCEEDED(DD_EnsureSurfaceAvailable(App.lpZBuffer, 0, 0));

	pass = pass || SUCCEEDED(DD_EnsureSurfaceAvailable(App.lpPictureBuffer, 0, 0));

	if (pass && !GtWindowClosed && App.nRenderMode == 1)
		HWR_GetAllTextureHandles();

	return pass;
}

void DXClearBuffers(ulong flags, ulong color)
{
	DIRECT3DINFO* d3d;
	DISPLAYMODE* dm;
	RECT r;
	D3DRECT vr;
	ulong sflags;

	dm = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].DisplayMode[App.DXConfigPtr->nVMode];
	r.top = 0;
	r.left = 0;
	r.right = dm->w;
	r.bottom = dm->h;

	if (App.nRenderMode == 1)
	{
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
			App.lpViewPort->Clear(1, &vr, sflags);
		}
	}
	else if (flags & 2)
		DD_ClearSurface(App.lpBackBuffer, &r, color);

	if (flags & 1)
		DD_ClearSurface(App.lpFrontBuffer, &r, color);

	if (flags & 0x20)
	{
		r.top = 0;
		r.left = 0;
		r.right = 640;
		r.bottom = 480;
		DD_ClearSurface(App.lpPictureBuffer, &r, color);
	}

	d3d = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D];

	if (!d3d->bHardware)
	{
		if (flags & 2)
		{
			dm = &d3d->DisplayMode[App.DXConfigPtr->nVMode];
			memset(App.unk, 0, 4 * dm->w * dm->h);
		}
	}
}

bool DXUpdateFrame(bool runMessageLoop, LPRECT rect)
{
	DIRECT3DINFO* d3dinfo;
	LPDIRECTDRAWSURFACE3 surf;
	DDSURFACEDESC desc;
	DDSURFACEDESC backDesc;
	uchar* dest;
	ulong w;

	App.SomeCounter++;
	DXCheckForLostSurfaces();
	d3dinfo = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D];
	w = d3dinfo->DisplayMode[App.DXConfigPtr->nVMode].w;

	if (d3dinfo->bHardware)
		App.lpFrontBuffer->Flip(0, DDFLIP_WAIT);
	else
	{
		memset(&desc, 0, sizeof(DDSURFACEDESC));
		memset(&backDesc, 0, sizeof(DDSURFACEDESC));
		desc.dwSize = sizeof(DDSURFACEDESC);
		backDesc.dwSize = sizeof(DDSURFACEDESC);
		DXGetSurfaceDesc(App.lpBackBuffer, &backDesc);

		if (backDesc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
			surf = App.lpBackBuffer;
		else
			surf = App.lpFrontBuffer;

		surf->Lock(0, &desc, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
		dest = (uchar*)desc.lpSurface;

		if (App.DXConfig.MMX)
		{
			if (desc.ddpfPixelFormat.dwRGBBitCount == 32)
			{
				for (ulong i = 0, n = 0; i < desc.dwHeight; i++, n += w)
					memcpy(dest + desc.lPitch * i, &App.unk[n], w * 4);
			}

			if (desc.ddpfPixelFormat.dwRGBBitCount == 16 && desc.ddpfPixelFormat.dwGBitMask == 0x3E0)
			{
				for (ulong i = 0, n = 0; i < desc.dwHeight; i++, n += w)
					MMXBlit32to15(dest + desc.lPitch * i, &App.unk[n], w);
			}
			else if (desc.ddpfPixelFormat.dwRGBBitCount == 16 && desc.ddpfPixelFormat.dwGBitMask == 0x7E0)
			{
				for (ulong i = 0, n = 0; i < desc.dwHeight; i++, n += w)
					MMXBlit32to16(dest + desc.lPitch * i, &App.unk[n], w);
			}
			else if (desc.ddpfPixelFormat.dwRGBBitCount == 24)
			{
				for (ulong i = 0, n = 0; i < desc.dwHeight; i++, n += w)
					MMXBlit32to24(dest + desc.lPitch * i, &App.unk[n], w);
			}
		}
		else
		{
			if (desc.ddpfPixelFormat.dwGBitMask == 0x3E0)
			{
				for (ulong i = 0, n = 0; i < desc.dwHeight; i++, n += 2 * w)
					SWRBlit32to15((ulong*)(dest + desc.lPitch * i), (ulong*)((uchar*)App.unk + n), w);
			}
			else
			{
				for (ulong i = 0, n = 0; i < desc.dwHeight; i++, n += 2 * w)
					memcpy(dest + desc.lPitch * i, (uchar*)App.unk + n, 2 * w);
			}
		}

		if (backDesc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
		{
			App.lpBackBuffer->Unlock(0);
			App.lpFrontBuffer->Flip(0, DDFLIP_WAIT);
		}
		else
			App.lpFrontBuffer->Unlock(0);
	}

	if (runMessageLoop)
		return DD_SpinMessageLoop(0);

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
	INJECT(0x004900B0, DXGetSurfaceDesc, replace);
	INJECT(0x004900C0, DXSurfBlt, replace);
	INJECT(0x0048F1C0, DXBitMask2ShiftCnt, replace);
	INJECT(0x0048FE40, DXCreateDirectDraw, replace);
	INJECT(0x0048FEA0, DXCreateDirect3D, replace);
	INJECT(0x0048FEC0, DXSetCooperativeLevel, replace);
	INJECT(0x0048ECE0, DXEnumDirectInput, replace);
	INJECT(0x0048F1F0, DXEnumDisplayModes, replace);
	INJECT(0x004B2E80, DXCreateZBuffer, replace);
	INJECT(0x0048EFD0, DXEnumDirectDraw, replace);
	INJECT(0x0048FBB0, DXEnumTextureFormats, replace);
	INJECT(0x0048EDE0, DXEnumDirectSound, replace);
	INJECT(0x0048EEE0, DXFreeDeviceInfo, replace);
	INJECT(0x004B40A0, DXSaveScreen, replace);
	INJECT(0x004B3A40, DXDoFlipWait, replace);
	INJECT(0x004B3C50, DXCheckForLostSurfaces, replace);
	INJECT(0x004B3A70, DXClearBuffers, replace);
	INJECT(0x004B3D10, DXUpdateFrame, replace);
}
