#include "../tomb3/pch.h"
#include "fmv.h"
#include "specific.h"
#include "winmain.h"
#include "file.h"
#include "input.h"
#include "audio.h"
#include "../tomb3/tomb3.h"

long fmv_playing;

static LPVOID MovieContext;
static LPVOID FmvContext;
static LPVOID FmvSoundContext;

#define GET_DLL_PROC(dll, proc) \
{ \
	*(FARPROC*)&(proc) = GetProcAddress(dll, #proc); \
	if(!proc) throw #proc; \
}

#if (DIRECT3D_VERSION < 0x900)
static long(__cdecl* Player_PassInDirectDrawObject)(LPDIRECTDRAWX);
#endif
static long(__cdecl* Player_InitMovie)(LPVOID, long, long, const char*, long);
static long(__cdecl* Player_InitVideo)(LPVOID, LPVOID, long, long, long, long, long, long, long, long, long, long, long);
static long(__cdecl* Player_InitPlaybackMode)(HWND, LPVOID, long, long);
static long(__cdecl* Player_BlankScreen)(long, long, long, long);
static long(__cdecl* Player_InitSoundSystem)(HWND);
static long(__cdecl* Player_GetDSErrorCode)();
static long(__cdecl* Player_InitSound)(LPVOID, long, long, long, long, long, long, long, long);
static long(__cdecl* Player_SetVolume)(LPVOID, long);
static long(__cdecl* Player_InitMoviePlayback)(LPVOID, LPVOID, LPVOID);
static long(__cdecl* Player_StartTimer)(LPVOID);
static long(__cdecl* Player_StopTimer)(LPVOID);
static long(__cdecl* Player_PlayFrame)(LPVOID, LPVOID, LPVOID, long, LPRECT, long, long, long);
static long(__cdecl* Player_ShutDownMovie)(LPVOID);
static long(__cdecl* Player_ShutDownSound)(LPVOID);
static long(__cdecl* Player_ShutDownSoundSystem)();
static long(__cdecl* Player_ShutDownVideo)(LPVOID);
static long(__cdecl* Player_ReturnPlaybackMode)();

static long(__cdecl* Movie_GetFormat)(LPVOID);
static long(__cdecl* Movie_GetXSize)(LPVOID);
static long(__cdecl* Movie_GetYSize)(LPVOID);
static long(__cdecl* Movie_GetSoundPrecision)(LPVOID);
static long(__cdecl* Movie_GetSoundRate)(LPVOID);
static long(__cdecl* Movie_GetSoundChannels)(LPVOID);
static long(__cdecl* Movie_SetSyncAdjust)(LPVOID, LPVOID, long);
static long(__cdecl* Movie_GetCurrentFrame)(LPVOID);
static long(__cdecl* Movie_GetTotalFrames)(LPVOID);

static HMODULE hWinPlay;

bool LoadWinPlay()
{
	hWinPlay = LoadLibrary("WINPLAY.DLL");

	if (!hWinPlay)
		return 0;

	try
	{
#if (DIRECT3D_VERSION < 0x900)
		GET_DLL_PROC(hWinPlay, Player_PassInDirectDrawObject);
#endif
		GET_DLL_PROC(hWinPlay, Player_InitMovie);
		GET_DLL_PROC(hWinPlay, Player_InitVideo);
		GET_DLL_PROC(hWinPlay, Player_InitPlaybackMode);
		GET_DLL_PROC(hWinPlay, Player_BlankScreen);
		GET_DLL_PROC(hWinPlay, Player_InitSoundSystem);
		GET_DLL_PROC(hWinPlay, Player_GetDSErrorCode);
		GET_DLL_PROC(hWinPlay, Player_InitSound);
		GET_DLL_PROC(hWinPlay, Player_SetVolume);
		GET_DLL_PROC(hWinPlay, Player_InitMoviePlayback);
		GET_DLL_PROC(hWinPlay, Player_StartTimer);
		GET_DLL_PROC(hWinPlay, Player_StopTimer);
		GET_DLL_PROC(hWinPlay, Player_PlayFrame);
		GET_DLL_PROC(hWinPlay, Player_ShutDownMovie);
		GET_DLL_PROC(hWinPlay, Player_ShutDownSound);
		GET_DLL_PROC(hWinPlay, Player_ShutDownSoundSystem);
		GET_DLL_PROC(hWinPlay, Player_ShutDownVideo);
		GET_DLL_PROC(hWinPlay, Player_ReturnPlaybackMode);

		GET_DLL_PROC(hWinPlay, Movie_GetFormat);
		GET_DLL_PROC(hWinPlay, Movie_GetXSize);
		GET_DLL_PROC(hWinPlay, Movie_GetYSize);
		GET_DLL_PROC(hWinPlay, Movie_GetSoundPrecision);
		GET_DLL_PROC(hWinPlay, Movie_GetSoundRate);
		GET_DLL_PROC(hWinPlay, Movie_GetSoundChannels);
		GET_DLL_PROC(hWinPlay, Movie_SetSyncAdjust);
		GET_DLL_PROC(hWinPlay, Movie_GetCurrentFrame);
		GET_DLL_PROC(hWinPlay, Movie_GetTotalFrames);
	}
	catch (LPCTSTR)
	{
		FreeLibrary(hWinPlay);
		hWinPlay = 0;
		return 0;
	}

	return 1;
}

void FreeWinPlay()
{
	if (hWinPlay)
	{
		FreeLibrary(hWinPlay);
		hWinPlay = 0;
	}
}

long FMV_Play(char* name)
{
#if (DIRECT3D_VERSION >= 0x900)
	return 0;
#endif

	if (App.Windowed || !App.WinPlayLoaded)
		return 0;

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
#if (DIRECT3D_VERSION >= 0x900)
	return 0;
#endif

	if (App.Windowed || !App.WinPlayLoaded)
		return 0;

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

void WinPlayFMV(const char* name, bool play)
{
	long xSize, ySize, xOffset, yOffset;
	long lp;
	RECT r;

#if (DIRECT3D_VERSION >= 0x900)
	return;
#endif

	r.left = 0;
	r.top = 0;
	r.right = 640;
	r.bottom = 480;

#if (DIRECT3D_VERSION < 0x900)
	if (Player_PassInDirectDrawObject(App.DDraw) || Player_InitMovie(&MovieContext, 0, 0, name, 0x200000) || Movie_GetFormat(MovieContext) != 130)
		return;
#endif

	xSize = Movie_GetXSize(MovieContext);
	ySize = Movie_GetYSize(MovieContext);
	xOffset = 0;// 320 - Movie_GetXSize(MovieContext);
	yOffset = 240 - Movie_GetYSize(MovieContext);

	if (Player_InitVideo(&FmvContext, MovieContext, xSize, ySize, xOffset, yOffset, 0, 0, 640, 480, 0, 1, 13) ||
		(play && Player_InitPlaybackMode(App.WindowHandle, FmvContext, 1, 0)))
		return;

	Player_BlankScreen(r.left, r.top, r.right, r.bottom);

	if (Player_InitSoundSystem(App.WindowHandle) || FAILED(Player_GetDSErrorCode()))
		return;

	if (Player_InitSound(&FmvSoundContext, 0x4000, Movie_GetSoundPrecision(MovieContext) == 4 ? 4 : 1,
		Movie_GetSoundPrecision(MovieContext) != 4, 4096, Movie_GetSoundChannels(MovieContext), Movie_GetSoundRate(MovieContext),
		Movie_GetSoundPrecision(MovieContext), 2))
		return;

	Player_SetVolume(FmvSoundContext, acm_volume);
	Movie_SetSyncAdjust(MovieContext, FmvSoundContext, 4);

	if (Player_InitMoviePlayback(MovieContext, FmvContext, FmvSoundContext))
		return;

	S_UpdateInput();
	Player_StartTimer(MovieContext);
	Player_BlankScreen(r.left, r.top, r.right, r.bottom);
	S_UpdateInput();

	lp = 0;

	while (Movie_GetCurrentFrame(MovieContext) < Movie_GetTotalFrames(MovieContext) && !lp)
	{
		lp = Player_PlayFrame(MovieContext, FmvContext, FmvSoundContext, 0, &r, 0, 0, 0);

		if (S_UpdateInput())
			break;

		if (input & IN_OPTION)
			break;
	}
}

void WinStopFMV(bool play)
{
#if (DIRECT3D_VERSION >= 0x900)
	return;
#endif
	Player_StopTimer(MovieContext);
	Player_ShutDownSound(&FmvSoundContext);
	Player_ShutDownVideo(&FmvContext);
	Player_ShutDownMovie(&MovieContext);
	Player_ShutDownSoundSystem();

	if (play)
		Player_ReturnPlaybackMode();
}
