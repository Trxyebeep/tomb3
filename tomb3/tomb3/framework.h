#pragma once

//#define RANDO_STUFF	1
#define TROYESTUFF	1
//#define DO_LOG	1

#define DIRECT3D_VERSION	0x0500
#define DIRECTDRAW_VERSION	0x0500
#define DIRECTSOUND_VERSION	0x0500
#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#if (DIRECT3D_VERSION >= 0x900)
#include <d3d9.h>
#else
#include <d3d.h>
#endif
#include <cmath>
#include <stdio.h>
#include <dinput.h>
#include <dsound.h>
#include <mmreg.h>
#include <mmeapi.h>
#include <ctime>
#include <MSAcm.h>
