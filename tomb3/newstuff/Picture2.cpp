#include "../tomb3/pch.h"
#if (DIRECT3D_VERSION >= 0x900)
#include "Picture2.h"
#include "../specific/winmain.h"
#include "../specific/texture.h"
#include "../specific/picture.h"
#include "../specific/hwrender.h"
#include "../specific/file.h"
#include "../specific/dxshell.h"
#include "../specific/time.h"
#include "../game/camera.h"
#include "../3dsystem/3d_gen.h"
#include "../tomb3/tomb3.h"

long bMonoScreen;

/*mono*/
static void CreateMonoScreenTexture()
{
	DISPLAYMODE* dm;
	LPDIRECTDRAWSURFACEX surf;
	DDSURFACEDESCX desc;
	long* pIndices;
	char* source;
	char* dest;

	if (nLoadedPictures)
		pIndices = OldPicTexIndices;
	else
		pIndices = CurPicTexIndices;

	dm = &App.lpDeviceInfo->D3DInfo[App.lpDXConfig->nD3D].DisplayMode[App.lpDXConfig->nVMode];
	surf = App.CaptureBuffer;

	dest = (char*)malloc((dm->w * dm->h) * (32 >> 3));
	memset(&desc, 0, sizeof(DDSURFACEDESCX));

	surf->LockRect(&desc, 0, D3DLOCK_READONLY);
	source = (char*)desc.pBits;
	MemBlt(dest, 0, 0, dm->w, dm->h, dm->w, source, 0, 0, &desc);
	surf->UnlockRect();
	pIndices[0] = DXTextureAdd(dm->w, dm->h, (uchar*)dest, Textures, 8888, TF_PICTEX);

	/*hack*/
	pIndices[1] = DXTextureAdd(1, 1, (uchar*)dest, Textures, 8888, TF_PICTEX);
	pIndices[2] = DXTextureAdd(1, 1, (uchar*)dest, Textures, 8888, TF_PICTEX);
	pIndices[3] = DXTextureAdd(1, 1, (uchar*)dest, Textures, 8888, TF_PICTEX);
	pIndices[4] = DXTextureAdd(1, 1, (uchar*)dest, Textures, 8888, TF_PICTEX);

	free(dest);
}

void CreateMonoScreen()
{
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

	CreateMonoScreenTexture();
	HWR_GetAllTextureHandles();
	DXTextureSetGreyScale(0);

	if (nLoadedPictures)
		bMonoScreen = 2;
	else
		bMonoScreen = 1;

	nLoadedPictures++;
	TIME_Init();
}

static void DrawMono(long col, float z)
{
	col = 0xFF - col;

	if (tomb3.psx_contrast)
		col >>= 1;

	col = RGBA(col, col, col, 0xFF);

	HWR_EnableAlphaBlend(0);
	HWR_EnableColorAddition(0);
	DrawTile(0, 0, phd_winxmax, phd_winymax, CurPicTexIndices[0], 0, 0, 256, 256, col, col, col, col, f_zfar);
}

static void DrawMonoAlpha(long col, long* indices, float z)
{
	col = 255 - col;

	if (tomb3.psx_contrast)
		col >>= 1;

	col = RGBA(col, col, col, 0xFF);

	HWR_EnableAlphaBlend(0);
	HWR_EnableColorAddition(0);
	DrawTile(0, 0, phd_winxmax, phd_winymax, indices[0], 0, 0, 256, 256, col, col, col, col, f_zfar);
}

void DrawMonoScreen(long r, long g, long b)
{
	long col;

	if (tomb3.psx_mono)
	{
		if (!tomb3.psx_contrast)
		{
			r <<= 1;
			g <<= 1;	//compensate for PSX contrast
			b <<= 1;
		}

		col = RGBA(r, g, b, 0xFF);

		HWR_EnableAlphaBlend(0);
		HWR_EnableColorAddition(0);
		DrawTile(0, 0, phd_winxmax, phd_winymax, CurPicTexIndices[0], 0, 0, 256, 256, col, col, col, col, f_zfar);
	}
	else
		DrawMono(0, f_zfar);
}

static long FadeMonoScreen(long steps)
{
	if (forceFadeDown)
	{
		for (int i = 0; i < steps; i++)
		{
			HWR_BeginScene();
			DrawMono((256 % steps) - (i * (-256 / steps)), f_znear);
			HWR_EndScene();
			DXUpdateFrame(1, 0);
		}

		HWR_BeginScene();
		DrawMono(255, f_znear);
		HWR_EndScene();
		DXUpdateFrame(1, 0);

		forceFadeDown = 0;
		nLoadedPictures = 0;
		DXTextureCleanup(CurPicTexIndices[0], Textures);
		DXTextureCleanup(CurPicTexIndices[1], Textures);
		DXTextureCleanup(CurPicTexIndices[2], Textures);
		DXTextureCleanup(CurPicTexIndices[3], Textures);
		DXTextureCleanup(CurPicTexIndices[4], Textures);
		HWR_GetAllTextureHandles();
		TIME_Init();
		return 1;
	}

	HWR_BeginScene();
	DrawMono(0, f_znear);
	HWR_EndScene();
	DXUpdateFrame(1, 0);
	return 0;
}

void RemoveMonoScreen(long fade)
{
	if (fade)
	{
		nLoadedPictures = 1;
		
		if (FadeMonoScreen(32))
			bMonoScreen = 0;
	}
	else
	{
		DXTextureCleanup(CurPicTexIndices[0], Textures);
		DXTextureCleanup(CurPicTexIndices[1], Textures);
		DXTextureCleanup(CurPicTexIndices[2], Textures);
		DXTextureCleanup(CurPicTexIndices[3], Textures);
		DXTextureCleanup(CurPicTexIndices[4], Textures);
		HWR_GetAllTextureHandles();
		nLoadedPictures = 0;
		bMonoScreen = 0;
	}
}

/*picture*/
static void CreatePictureTextures()
{
	LPDIRECTDRAWSURFACEX surf;
	DDSURFACEDESCX desc;
	long* pIndices;
	char* source;
	char* dest;

	surf = App.PictureBuffer;

	if (nLoadedPictures)
		pIndices = OldPicTexIndices;
	else
		pIndices = CurPicTexIndices;

	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	dest = (char*)malloc((256 * 256) * (32 >> 3));

	surf->LockRect(&desc, 0, D3DLOCK_READONLY);
	source = (char*)desc.pBits;
	MemBlt(dest, 0, 0, 256, 256, 256, source, 0, 0, &desc);
	surf->UnlockRect();
	pIndices[0] = DXTextureAdd(256, 256, (uchar*)dest, Textures, 8888, TF_PICTEX);

	surf->LockRect(&desc, 0, D3DLOCK_READONLY);
	source = (char*)desc.pBits;
	MemBlt(dest, 0, 0, 256, 256, 256, source, 256, 0, &desc);
	surf->UnlockRect();
	pIndices[1] = DXTextureAdd(256, 256, (uchar*)dest, Textures, 8888, TF_PICTEX);

	surf->LockRect(&desc, 0, D3DLOCK_READONLY);
	source = (char*)desc.pBits;
	MemBlt(dest, 0, 0, 128, 256, 256, source, 512, 0, &desc);
	MemBlt(dest, 128, 0, 128, 224, 256, source, 512, 256, &desc);
	surf->UnlockRect();
	pIndices[2] = DXTextureAdd(256, 256, (uchar*)dest, Textures, 8888, TF_PICTEX);

	surf->LockRect(&desc, 0, D3DLOCK_READONLY);
	source = (char*)desc.pBits;
	MemBlt(dest, 0, 0, 256, 224, 256, source, 0, 256, &desc);
	surf->UnlockRect();
	pIndices[3] = DXTextureAdd(256, 256, (uchar*)dest, Textures, 8888, TF_PICTEX);

	surf->LockRect(&desc, 0, D3DLOCK_READONLY);
	source = (char*)desc.pBits;
	MemBlt(dest, 0, 0, 256, 224, 256, source, 256, 256, &desc);
	surf->UnlockRect();
	pIndices[4] = DXTextureAdd(256, 256, (uchar*)dest, Textures, 8888, TF_PICTEX);

	free(dest);
}

void LoadPicture(const char* name)
{
	LPDIRECTDRAWSURFACEX surf;
	HANDLE img;
	BITMAP bitmap;
	HDC cdc;
	HDC hdc;

	surf = App.PictureBuffer;
	img = LoadImage(0, GetFullPath(name), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!img)
		return;

	cdc = CreateCompatibleDC(0);
	SelectObject(cdc, img);
	GetObject(img, sizeof(BITMAP), &bitmap);

	surf->GetDC(&hdc);
	BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, cdc, 0, 0, SRCCOPY);
	surf->ReleaseDC(hdc);
	DeleteDC(cdc);
	CreatePictureTextures();
	HWR_GetAllTextureHandles();
	nLoadedPictures++;
}

void FreePictureTextures(long* indices)
{
	DXTextureCleanup(indices[0], Textures);
	DXTextureCleanup(indices[1], Textures);
	DXTextureCleanup(indices[2], Textures);
	DXTextureCleanup(indices[3], Textures);
	DXTextureCleanup(indices[4], Textures);
	HWR_GetAllTextureHandles();
}

void FreePictureTextures()
{
	FreePictureTextures(CurPicTexIndices);
	FreePictureTextures(OldPicTexIndices);
}

void DrawPicture(long col, long* indices, float z)
{
	long x[4];
	long y[4];
	static long screenX[4] = { 0, 256, 512, 640 };
	static long screenY[3] = { 0, 256, 480 };

	col = 255 - col;

	if (tomb3.psx_contrast)
		col >>= 1;

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

static void DrawPictureAlpha(long col, long* indices, float z)
{
	long x[4];
	long y[4];
	static long screenX[4] = { 0, 256, 512, 640 };
	static long screenY[3] = { 0, 256, 480 };
	long maxcol;

	if (tomb3.psx_contrast)
		maxcol = 0x808080;
	else
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

static void CrossFadePicture()
{
	long steps, step;

	steps = 32;
	step = 256 / steps;

	for (int i = 0, j = 255; i < 256; i += step, j -= step)
	{
		HWR_BeginScene();

		if (bMonoScreen == 2)
		{
			DrawPictureAlpha(i, CurPicTexIndices, f_znear);
			DrawMonoAlpha(j, OldPicTexIndices, f_zfar);
		}
		else if (bMonoScreen)
		{
			DrawMonoAlpha(i, CurPicTexIndices, f_znear);
			DrawPictureAlpha(j, OldPicTexIndices, f_zfar);
		}
		else
		{
			DrawPictureAlpha(i, CurPicTexIndices, f_znear);
			DrawPictureAlpha(j, OldPicTexIndices, f_zfar);
		}

		HWR_EndScene();
		DXUpdateFrame(1, 0);
	}

	FreePictureTextures(CurPicTexIndices);
	CurPicTexIndices[0] = OldPicTexIndices[0];
	CurPicTexIndices[1] = OldPicTexIndices[1];
	CurPicTexIndices[2] = OldPicTexIndices[2];
	CurPicTexIndices[3] = OldPicTexIndices[3];
	CurPicTexIndices[4] = OldPicTexIndices[4];
	HWR_EnableAlphaBlend(0);
	HWR_EnableColorAddition(0);

	if (dontFadePicture)
		pictureFading = 0;
	else
		pictureFading = 1;

	HWR_BeginScene();
	DrawPicture(0, CurPicTexIndices, f_znear);
	HWR_EndScene();
	DXUpdateFrame(1, 0);
	TIME_Init();
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
			DrawPicture(255 - (i * (256 / steps)), CurPicTexIndices, f_znear);
			HWR_EndScene();
			DXUpdateFrame(1, 0);
		}
	}

	HWR_BeginScene();
	DrawPicture(0, CurPicTexIndices, f_znear);
	HWR_EndScene();
	DXUpdateFrame(1, 0);
	TIME_Init();
}

void FadePictureDown(long steps)
{
	if (forceFadeDown)
	{
		for (int i = 0; i < steps; i++)
		{
			HWR_BeginScene();
			DrawPicture((256 % steps) - (i * (-256 / steps)), CurPicTexIndices, f_znear);
			HWR_EndScene();
			DXUpdateFrame(1, 0);
		}

		HWR_BeginScene();
		DrawPicture(255, CurPicTexIndices, f_znear);
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
		DrawPicture(0, CurPicTexIndices, f_znear);
		HWR_EndScene();
		DXUpdateFrame(1, 0);
	}
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
	CreatePictureTextures();
	HWR_GetAllTextureHandles();
	nLoadedPictures++;
	TIME_Init();
}

void DoInventoryPicture()
{
	HWR_EnableZBuffer(0, 0);

	if (bMonoScreen)
		DrawMono(0, f_zfar);
	else
		DrawPicture(0, CurPicTexIndices, f_zfar);
}

/*shared*/
void MemBlt(char* dest, long x, long y, long w, long h, long sz, char* source, long x2, long y2, LPDDSURFACEDESCX desc)
{
	ulong stride;

	stride = 32 >> 3;
	dest += stride * (x + y * sz);
	source += y2 * desc->Pitch + stride * x2;

	while (h)
	{
		memcpy(dest, source, stride * w);
		dest += stride * sz;
		source += desc->Pitch;
		h--;
	}
}

void DrawTile(long x, long y, long w, long h, long tpage, long tU, long tV, long tW, long tH, long c0, long c1, long c2, long c3, float z)
{
	D3DTLVERTEX v[4];
	float u1, v1, u2, v2;
	long lp;

	u1 = float(tU * (1.0F / 256.0F));
	v1 = float(tV * (1.0F / 256.0F));
	u2 = float((tW + tU) * (1.0F / 256.0F));
	v2 = float((tH + tV) * (1.0F / 256.0F));

	v[0].sx = (float)x;
	v[0].sy = (float)y;
	v[0].tu = u1;
	v[0].tv = v1;
	v[0].color = c0;

	v[1].sx = float(w + x);
	v[1].sy = (float)y;
	v[1].tu = u2;
	v[1].tv = v1;
	v[1].color = c1;

	v[2].sx = float(w + x);
	v[2].sy = float(h + y);
	v[2].tu = u2;
	v[2].tv = v2;
	v[2].color = c2;

	v[3].sx = (float)x;
	v[3].sy = float(h + y);
	v[3].tu = u1;
	v[3].tv = v2;
	v[3].color = c3;

	for (lp = 0; lp < 4; lp++)
	{
		v[lp].sz = 0.995F;
		v[lp].rhw = one / z;
		v[lp].specular = 0xFF000000;
	}

	HWR_SetCurrentTexture(&Textures[tpage]);
	HWR_EnableAlphaBlend(1);
	HWR_EnableZBuffer(0, 0);

	SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	DrawPrimitive(D3DPT_TRIANGLEFAN, v, 4);
	SetSamplerState(0, D3DSAMP_MAGFILTER, HWConfig.nFilter);
	SetSamplerState(0, D3DSAMP_MINFILTER, HWConfig.nFilter);
}
#endif
