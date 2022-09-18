#include "../tomb3/pch.h"
#include "file.h"
#include "dd.h"
#include "init.h"
#include "hwrender.h"

long MyReadFile(HANDLE hFile, LPVOID lpBuffer, ulong nNumberOfBytesToRead, ulong* lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
	static ulong nBytesRead;

	nBytesRead += nNumberOfBytesToRead;

	if (nBytesRead > 0x4000)
	{
		nBytesRead = 0;
		DD_SpinMessageLoop(0);
	}

	return ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

bool LoadPalette(HANDLE file)
{
	uchar* pGP;
	ulong nBytesRead;

	MyReadFile(file, game_palette, sizeof(game_palette), &nBytesRead, 0);
	game_palette[0] = 0;
	game_palette[1] = 0;
	game_palette[2] = 0;

	for (int i = 3; i < sizeof(game_palette); i++)
		game_palette[i] <<= 2;

	MyReadFile(file, G_GouraudPalette, sizeof(G_GouraudPalette), &nBytesRead, 0);
	BlackGouraudIndex = 0;
	pGP = G_GouraudPalette;

	while (pGP[0] || pGP[1] || pGP[2])
	{
		pGP += 4;
		BlackGouraudIndex++;

		if (BlackGouraudIndex > sizeof(G_GouraudPalette) / 4)
			break;
	}

	bFixSkyColour = 1;
	return 1;
}

long LoadTexturePages(HANDLE file)
{
	char* p;
	ulong read;
	long nPages, size;
	bool _16bit;

	MyReadFile(file, &nPages, sizeof(long), &read, 0);

	if (!App.nRenderMode)
	{
		for (int i = 0; i < nPages; i++)
		{
			if (!texture_page_ptrs[i])
				texture_page_ptrs[i] = (char*)game_malloc(0x10000, 1);

			MyReadFile(file, texture_page_ptrs[i], 0x10000, &read, 0);
		}

		SetFilePointer(file, nPages << 17, 0, FILE_CURRENT);
		return 1;
	}

	_16bit = !App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].Texture[App.DXConfigPtr->D3DTF].bPalette;
	size = _16bit ? 0x20000 : 0x10000;
	p = (char*)GlobalAlloc(GMEM_FIXED, nPages * size);

	if (!p)
		return 0;

	if (_16bit)
	{
		SetFilePointer(file, nPages << 16, 0, FILE_CURRENT);

		for (int i = 0; i < nPages; i++)
			MyReadFile(file, p + (size * i), size, &read, 0);

		HWR_LoadTexturePages(nPages, p, 0);
	}
	else
	{
		for (int i = 0; i < nPages; i++)
			MyReadFile(file, p + (size * i), size, &read, 0);

		SetFilePointer(file, nPages << 17, 0, FILE_CURRENT);
		HWR_LoadTexturePages(nPages, p, game_palette);
	}

	GlobalFree(p);
	return 1;
}

void inject_file(bool replace)
{
	INJECT(0x00480D50, MyReadFile, replace);
	INJECT(0x00481CA0, LoadPalette, replace);
	INJECT(0x00480DA0, LoadTexturePages, replace);
}
