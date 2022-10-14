#pragma once
#include "../global/vars.h"

void inject_dxshell(bool replace);

long BPPToDDBD(long BPP);
bool DXSetVideoMode(LPDIRECTDRAW2 ddx, long w, long h, long bpp);
bool DXCreateSurface(LPDIRECTDRAW2 ddx, LPDDSURFACEDESC desc, LPDIRECTDRAWSURFACE3 surf);
bool DXGetAttachedSurface(LPDIRECTDRAWSURFACE3 surf, LPDDSCAPS caps, LPDIRECTDRAWSURFACE3* attached);
bool DXAddAttachedSurface(LPDIRECTDRAWSURFACE3 surf, LPDIRECTDRAWSURFACE3 attached);
bool DXCreateDirect3DDevice(LPDIRECT3D2 dd3x, GUID guid, LPDIRECTDRAWSURFACE3 surf, LPDIRECT3DDEVICE2* device);
bool DXCreateViewPort(LPDIRECT3D2 dd3x, LPDIRECT3DDEVICE2 device, long w, long h, LPDIRECT3DVIEWPORT2* lpvp);
void DXGetSurfaceDesc(LPDIRECTDRAWSURFACE3 surf, LPDDSURFACEDESC desc);

#define DXBitMask2ShiftCnt	( (void(__cdecl*)(ulong, uchar*, uchar*)) 0x0048F1C0 )
#define DXSwitchVideoMode	( (bool(__cdecl*)(long, long, bool)) 0x004B3550 )
