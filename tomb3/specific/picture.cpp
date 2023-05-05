#include "../tomb3/pch.h"
#include "picture.h"
#include "hwrender.h"
#include "dxshell.h"
#include "time.h"
#include "texture.h"
#include "file.h"
#include "../3dsystem/3d_gen.h"
#include "winmain.h"
#include "../game/camera.h"
#include "../tomb3/tomb3.h"

long OldPicTexIndices[5];
long CurPicTexIndices[5];
long nLoadedPictures;
bool forceFadeDown;
bool pictureFading;
bool dontFadePicture;
bool bDontGreyOut;

void SetPictureToFade(long fade)
{
	pictureFading = fade;
}

void ForceFadeDown(long fade)
{
	forceFadeDown = fade;
}

#if (DIRECT3D_VERSION < 0x900)
void DoInventoryPicture()
{
	HWR_EnableZBuffer(0, 0);
	TRDrawPicture(0, CurPicTexIndices, f_zfar);
}

void FadePictureUp(long steps)
{
	if (nLoadedPictures > 1)
		CrossFadePicture();
	else
	{
		for (int i = 0; i < steps; i++)
		{
			HWR_BeginScene();
			TRDrawPicture(255 - (i * (256 / steps)), CurPicTexIndices, f_znear);
			HWR_EndScene();
			DXUpdateFrame(1, 0);
		}
	}

	HWR_BeginScene();
	TRDrawPicture(0, CurPicTexIndices, f_znear);
	HWR_EndScene();
	DXUpdateFrame(1, 0);
	TIME_Init();
}

void FadePictureDown(long steps)
{
	DIRECT3DINFO* d3dinfo;

	d3dinfo = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D];

	if (forceFadeDown || d3dinfo->Texture[App.lpDXConfig->D3DTF].bPalette || !d3dinfo->bAlpha)
	{
		for (int i = 0; i < steps; i++)
		{
			HWR_BeginScene();
			TRDrawPicture((256 % steps) - (i * (-256 / steps)), CurPicTexIndices, f_znear);
			HWR_EndScene();
			DXUpdateFrame(1, 0);
		}

		HWR_BeginScene();
		TRDrawPicture(255, CurPicTexIndices, f_znear);
		HWR_EndScene();
		DXUpdateFrame(1, 0);
		forceFadeDown = 0;
		nLoadedPictures = 0;
		FreePictureTextures(CurPicTexIndices);
		TIME_Init();
	}
	else
	{
		HWR_BeginScene();
		TRDrawPicture(0, CurPicTexIndices, f_znear);
		HWR_EndScene();
		DXUpdateFrame(1, 0);
	}
}

void CrossFadePicture()
{
	for (int i = 0, j = 255; i < 256; i += 8, j -= 8)
	{
		HWR_BeginScene();
		DrawPictureAlpha(i, CurPicTexIndices, f_znear);
		DrawPictureAlpha(j, OldPicTexIndices, f_zfar);
		HWR_EndScene();
		DXUpdateFrame(1, 0);
	}

	FreePictureTextures(CurPicTexIndices);
	CurPicTexIndices[0] = OldPicTexIndices[0];
	CurPicTexIndices[1] = OldPicTexIndices[1];
	CurPicTexIndices[2] = OldPicTexIndices[2];
	CurPicTexIndices[3] = OldPicTexIndices[3];
	CurPicTexIndices[4] = OldPicTexIndices[4];
	HWR_EnableColorKey(0);
	HWR_EnableAlphaBlend(0);
	HWR_EnableColorAddition(0);

	if (dontFadePicture)
		pictureFading = 0;
	else
		pictureFading = 1;

	HWR_BeginScene();
	TRDrawPicture(0, CurPicTexIndices, f_znear);
	HWR_EndScene();
	DXUpdateFrame(1, 0);
	TIME_Init();
}

void S_FadePicture()
{
	static long pos, max = 64;
	long nframes;

	if (!pictureFading || dontFadePicture)
		return;

	nframes = camera.number_frames;

	if (nframes > TICKS_PER_FRAME * 5)
		nframes = TICKS_PER_FRAME;

	DrawPictureAlpha(256 - 256 / max * (max - pos), CurPicTexIndices, f_znear);
	pos += nframes;

	if (pos >= max)
	{
		pos = 0;
		pictureFading = 0;
		nLoadedPictures = 0;
		FreePictureTextures(CurPicTexIndices);
	}
}

void S_FadeToBlack()
{
	DISPLAYMODE* dm;
	LPDIRECTDRAWSURFACEX buffer;

	dm = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].DisplayMode[App.lpDXConfig->nVMode];

	if (App.Windowed)
		buffer = App.BackBuffer;
	else
		buffer = App.FrontBuffer;

	if (dm->w == 640 && dm->h == 480)
		ConvertSurfaceToTextures(buffer);
	else
	{
		App.PictureBuffer->Blt(0, buffer, 0, DDBLT_WAIT, 0);
		ConvertSurfaceToTextures(App.PictureBuffer);
	}

	HWR_GetAllTextureHandles();

	for (int i = 0; i < nTextures; i++)
		HWR_SetCurrentTexture(TexturePtrs[i]);

	nLoadedPictures++;
	TIME_Init();
}

bool LoadPicture(const char* name, LPDIRECTDRAWSURFACEX surf)
{
	HANDLE img;
	BITMAP bitmap;
	HDC cdc;
	HDC hdc;

	img = LoadImage(0, GetFullPath(name), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!img)
		return 0;

	cdc = CreateCompatibleDC(0);
	SelectObject(cdc, img);
	GetObject(img, sizeof(BITMAP), &bitmap);

	surf->GetDC(&hdc);
	BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, cdc, 0, 0, SRCCOPY);
	surf->ReleaseDC(hdc);
	DeleteDC(cdc);
	ConvertSurfaceToTextures(surf);
	HWR_GetAllTextureHandles();
	nLoadedPictures++;
	return 1;
}

void FreePictureTextures(long* indices)
{
	DXTextureCleanup(indices[0], Textures);
	DXTextureCleanup(indices[1], Textures);
	DXTextureCleanup(indices[2], Textures);
	DXTextureCleanup(indices[3], Textures);
	DXTextureCleanup(indices[4], Textures);

	if (App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].Texture[App.lpDXConfig->D3DTF].bPalette)
	{
		DXFreeTPages();
		DXCreateMaxTPages(0);
	}

	HWR_GetAllTextureHandles();

	for (int i = 0; i < nTextures; i++)
		HWR_SetCurrentTexture(TexturePtrs[i]);
}

void CreateMonoScreen()
{
	DISPLAYMODE* dm;
	LPDIRECTDRAWSURFACEX buffer;

	if (bDontGreyOut)
	{
		if (tomb3.psx_mono)
			DXTextureSetGreyScale(0);
		else
			DXTextureSetGreyScale(1);

		bDontGreyOut = 0;
	}
	else
		DXTextureSetGreyScale(1);

	dm = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].DisplayMode[App.lpDXConfig->nVMode];

	if (App.Windowed)
		buffer = App.BackBuffer;
	else
		buffer = App.FrontBuffer;

	if (dm->w == 640 && dm->h == 480)
		ConvertSurfaceToTextures(buffer);
	else
	{
		App.PictureBuffer->Blt(0, buffer, 0, DDBLT_WAIT, 0);
		ConvertSurfaceToTextures(App.PictureBuffer);
	}

	HWR_GetAllTextureHandles();

	for (int i = 0; i < nTextures; i++)
		HWR_SetCurrentTexture(TexturePtrs[i]);

	DXTextureSetGreyScale(0);
	nLoadedPictures++;
	TIME_Init();
}

void DrawMonoScreen(long r, long g, long b)	//do not call this function with color values higher than 127
{
	long x[4];
	long y[4];
	static long screenX[4] = { 0, 256, 512, 640 };
	static long screenY[3] = { 0, 256, 480 };
	long col;

	if (tomb3.psx_mono)
	{
		r <<= 1;
		g <<= 1;	//compensate for PSX contrast
		b <<= 1;

		col = RGBA(r, g, b, 0xFF);

		for (int i = 0; i < 3; i++)
		{
			x[i] = phd_winxmin + phd_winwidth * screenX[i] / 640;
			y[i] = phd_winymin + phd_winheight * screenY[i] / 480;
		}

		x[3] = phd_winxmin + phd_winwidth * screenX[3] / 640;

		HWR_EnableAlphaBlend(0);
		HWR_EnableColorAddition(0);
		DrawTile(x[0], y[0], x[1] - x[0], y[1] - y[0], CurPicTexIndices[0], 0, 0, 256, 256, col, col, col, col, f_zfar);
		DrawTile(x[1], y[0], x[2] - x[1], y[1] - y[0], CurPicTexIndices[1], 0, 0, 256, 256, col, col, col, col, f_zfar);
		DrawTile(x[2], y[0], x[3] - x[2], y[1] - y[0], CurPicTexIndices[2], 0, 0, 128, 256, col, col, col, col, f_zfar);
		DrawTile(x[0], y[1], x[1] - x[0], y[2] - y[1], CurPicTexIndices[3], 0, 0, 256, 224, col, col, col, col, f_zfar);
		DrawTile(x[1], y[1], x[2] - x[1], y[2] - y[1], CurPicTexIndices[4], 0, 0, 256, 224, col, col, col, col, f_zfar);
		DrawTile(x[2], y[1], x[3] - x[2], y[2] - y[1], CurPicTexIndices[2], 128, 0, 128, 224, col, col, col, col, f_zfar);
	}
	else
		TRDrawPicture(0, CurPicTexIndices, f_zfar);
}

void RemoveMonoScreen(long fade)
{
	if (fade)
	{
		nLoadedPictures = 1;
		FadePictureDown(32);
	}
	else
	{
		FreePictureTextures(CurPicTexIndices);
		nLoadedPictures = 0;
	}
}

static void MemBlt(char* dest, long x, long y, long w, long h, long sz, char* source, long x2, long y2, DDSURFACEDESCX desc)
{
	ulong stride;

	stride = desc.ddpfPixelFormat.dwRGBBitCount >> 3;
	dest += stride * (x + y * sz);
	source += y2 * desc.lPitch + stride * x2;

	while (h)
	{
		memcpy(dest, source, stride * w);
		dest += stride * sz;
		source += desc.lPitch;
		h--;
	}
}

void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACEX surf)
{
	DDSURFACEDESCX desc;
	DDSURFACEDESCX desc2;
	long* pIndices;
	char* source;
	char* dest;
	long bitcnt;
	uchar rshift, gshift, bshift, rcount, gcount, bcount;

	if (App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D].Texture[App.lpDXConfig->D3DTF].bPalette)
	{
		DXFreeTPages();
		DXCreateMaxTPages(1);
	}

	if (nLoadedPictures)
		pIndices = OldPicTexIndices;
	else
		pIndices = CurPicTexIndices;

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	surf->Lock(0, &desc, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
	source = (char*)desc.lpSurface;
	dest = (char*)malloc((256 * 256) * (desc.ddpfPixelFormat.dwRGBBitCount >> 3));
	DXBitMask2ShiftCnt(desc.ddpfPixelFormat.dwRBitMask, &rshift, &rcount);
	DXBitMask2ShiftCnt(desc.ddpfPixelFormat.dwGBitMask, &gshift, &gcount);
	DXBitMask2ShiftCnt(desc.ddpfPixelFormat.dwBBitMask, &bshift, &bcount);

	if (desc.ddpfPixelFormat.dwRGBBitCount == 32)
		bitcnt = 8888;
	else
		bitcnt = bcount + 10 * (gcount + 10 * rcount);

	memcpy(&desc2, &desc, sizeof(DDSURFACEDESC));
	MemBlt(dest, 0, 0, 256, 256, 256, source, 0, 0, desc2);
	surf->Unlock(0);
	pIndices[0] = DXTextureAdd(256, 256, (uchar*)dest, Textures, bitcnt, TF_PICTEX);

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	surf->Lock(0, &desc, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
	memcpy(&desc2, &desc, sizeof(DDSURFACEDESCX));
	source = (char*)desc.lpSurface;
	MemBlt(dest, 0, 0, 256, 256, 256, source, 256, 0, desc2);
	surf->Unlock(0);
	pIndices[1] = DXTextureAdd(256, 256, (uchar*)dest, Textures, bitcnt, TF_PICTEX);

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	surf->Lock(0, &desc, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
	memcpy(&desc2, &desc, sizeof(DDSURFACEDESCX));
	MemBlt(dest, 0, 0, 128, 256, 256, source, 512, 0, desc2);
	memcpy(&desc2, &desc, sizeof(DDSURFACEDESCX));
	MemBlt(dest, 128, 0, 128, 224, 256, source, 512, 256, desc2);
	surf->Unlock(0);
	pIndices[2] = DXTextureAdd(256, 256, (uchar*)dest, Textures, bitcnt, TF_PICTEX);

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	surf->Lock(0, &desc, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
	memcpy(&desc2, &desc, sizeof(DDSURFACEDESCX));
	source = (char*)desc.lpSurface;
	MemBlt(dest, 0, 0, 256, 224, 256, source, 0, 256, desc2);
	surf->Unlock(0);
	pIndices[3] = DXTextureAdd(256, 256, (uchar*)dest, Textures, bitcnt, TF_PICTEX);

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	surf->Lock(0, &desc, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
	memcpy(&desc2, &desc, sizeof(DDSURFACEDESCX));
	source = (char*)desc.lpSurface;
	MemBlt(dest, 0, 0, 256, 224, 256, source, 256, 256, desc2);
	surf->Unlock(0);
	pIndices[4] = DXTextureAdd(256, 256, (uchar*)dest, Textures, bitcnt, TF_PICTEX);

	free(dest);
}

void DrawTile(long x, long y, long w, long h, long tpage, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3, float z)
{
	D3DTLVERTEX v[4];
	float u1, v1, u2, v2;

	u1 = float(tU * (1.0F / 256.0F));
	v1 = float(tV * (1.0F / 256.0F));
	u2 = float((tW + tU) * (1.0F / 256.0F));
	v2 = float((tH + tV) * (1.0F / 256.0F));

	v[0].sx = (float)x;
	v[0].sy = (float)y;
	v[0].sz = 0.995F;
	v[0].tu = u1;
	v[0].tv = v1;
	v[0].rhw = one / z;
	v[0].color = c0;
	v[0].specular = 0xFF000000;

	v[1].sx = float(w + x);
	v[1].sy = (float)y;
	v[1].sz = 0.995F;
	v[1].tu = u2;
	v[1].tv = v1;
	v[1].rhw = one / z;
	v[1].color = c1;
	v[1].specular = 0xFF000000;

	v[2].sx = float(w + x);
	v[2].sy = float(h + y);
	v[2].sz = 0.995F;
	v[2].tu = u2;
	v[2].tv = v2;
	v[2].rhw = one / z;
	v[2].color = c3;
	v[2].specular = 0xFF000000;

	v[3].sx = (float)x;
	v[3].sy = float(h + y);
	v[3].sz = 0.995F;
	v[3].tu = u1;
	v[3].tv = v2;
	v[3].rhw = one / z;
	v[3].color = c2;
	v[3].specular = 0xFF000000;

	HWR_SetCurrentTexture(&Textures[tpage]);
	HWR_EnableColorKey(0);
	HWR_EnableColorAddition(0);
	HWR_EnableZBuffer(0, 0);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_NEAREST);
	SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_NEAREST);
	DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX, v, 4, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAG, HWConfig.nFilter);
	SetRenderState(D3DRENDERSTATE_TEXTUREMIN, HWConfig.nFilter);
}

void DrawPictureAlpha(long col, long* indices, float z)
{
	long x[4];
	long y[4];
	static long screenX[4] = { 0, 256, 512, 640 };
	static long screenY[3] = { 0, 256, 480 };
	long maxcol;

#if (DIRECT3D_VERSION >= 0x900)
	if (tomb3.psx_contrast)
		maxcol = 0x808080;
	else
#endif
		maxcol = 0xFFFFFF;

	col = (0xFF000000 * (col + 1)) | maxcol;

	for (int i = 0; i < 3; i++)
	{
		x[i] = phd_winxmin + phd_winwidth * screenX[i] / 640;
		y[i] = phd_winymin + phd_winheight * screenY[i] / 480;
	}

	x[3] = phd_winxmin + phd_winwidth * screenX[3] / 640;

	HWR_EnableAlphaBlend(1);
	HWR_EnableColorAddition(0);
	DrawTile(x[0], y[0], x[1] - x[0], y[1] - y[0], indices[0], 0, 0, 256, 256, col, col, col, col, z);
	DrawTile(x[1], y[0], x[2] - x[1], y[1] - y[0], indices[1], 0, 0, 256, 256, col, col, col, col, z);
	DrawTile(x[2], y[0], x[3] - x[2], y[1] - y[0], indices[2], 0, 0, 128, 256, col, col, col, col, z);
	DrawTile(x[0], y[1], x[1] - x[0], y[2] - y[1], indices[3], 0, 0, 256, 224, col, col, col, col, z);
	DrawTile(x[1], y[1], x[2] - x[1], y[2] - y[1], indices[4], 0, 0, 256, 224, col, col, col, col, z);
	DrawTile(x[2], y[1], x[3] - x[2], y[2] - y[1], indices[2], 128, 0, 128, 224, col, col, col, col, z);
}

void TRDrawPicture(long col, long* indices, float z)
{
	long x[4];
	long y[4];
	static long screenX[4] = { 0, 256, 512, 640 };
	static long screenY[3] = { 0, 256, 480 };

	col = 255 - col;

#if (DIRECT3D_VERSION >= 0x900)
	if (tomb3.psx_contrast)
		col >>= 1;
#endif

	col = RGBA(col, col, col, 0xFF);

	for (int i = 0; i < 3; i++)
	{
		x[i] = phd_winxmin + phd_winwidth * screenX[i] / 640;
		y[i] = phd_winymin + phd_winheight * screenY[i] / 480;
	}

	x[3] = phd_winxmin + phd_winwidth * screenX[3] / 640;

	HWR_EnableAlphaBlend(0);
	HWR_EnableColorAddition(0);
	DrawTile(x[0], y[0], x[1] - x[0], y[1] - y[0], indices[0], 0, 0, 256, 256, col, col, col, col, z);
	DrawTile(x[1], y[0], x[2] - x[1], y[1] - y[0], indices[1], 0, 0, 256, 256, col, col, col, col, z);
	DrawTile(x[2], y[0], x[3] - x[2], y[1] - y[0], indices[2], 0, 0, 128, 256, col, col, col, col, z);
	DrawTile(x[0], y[1], x[1] - x[0], y[2] - y[1], indices[3], 0, 0, 256, 224, col, col, col, col, z);
	DrawTile(x[1], y[1], x[2] - x[1], y[2] - y[1], indices[4], 0, 0, 256, 224, col, col, col, col, z);
	DrawTile(x[2], y[1], x[3] - x[2], y[2] - y[1], indices[2], 128, 0, 128, 224, col, col, col, col, z);
}
#endif
