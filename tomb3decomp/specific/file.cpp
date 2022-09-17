#include "../tomb3/pch.h"
#include "file.h"
#include "dd.h"

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

void inject_file(bool replace)
{
	INJECT(0x00480D50, MyReadFile, replace);
	INJECT(0x00481CA0, LoadPalette, replace);
}
