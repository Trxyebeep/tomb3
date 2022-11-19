#include "../tomb3/pch.h"
#include "fmv.h"
#include "specific.h"
#include "winmain.h"
#include "file.h"
#ifdef TROYESTUFF
#include "../tomb3/tomb3.h"
#endif

long fmv_playing;

long FMV_Play(char* name)
{
#ifdef TROYESTUFF
	if (tomb3.Windowed)
		return 0;
#endif

	fmv_playing = 1;
	S_CDStop();
	ShowCursor(0);
	WinFreeDX(0);
	WinPlayFMV(GetFullPath(name), 1);
	WinStopFMV(1);
	fmv_playing = 0;

	if (!GtWindowClosed)
		WinDXInit(&App.DeviceInfo, &App.DXConfig, 0);

	ShowCursor(1);
	return GtWindowClosed;
}

long FMV_PlayIntro(char* name1, char* name2)
{
#ifdef TROYESTUFF
	if (tomb3.Windowed)
		return 0;
#endif

	fmv_playing = 1;
	ShowCursor(0);
	WinFreeDX(0);
	WinPlayFMV(GetFullPath(name1), 1);
	WinStopFMV(1);
	WinPlayFMV(GetFullPath(name2), 1);
	WinStopFMV(1);
	fmv_playing = 0;

	if (!GtWindowClosed)
		WinDXInit(&App.DeviceInfo, &App.DXConfig, 0);

	ShowCursor(1);
	return GtWindowClosed;
}

void inject_fmv(bool replace)
{
	INJECT(0x004834C0, FMV_Play, replace);
	INJECT(0x00483540, FMV_PlayIntro, replace);
}
