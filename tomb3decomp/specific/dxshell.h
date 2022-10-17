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
bool DXSurfBlt(LPDIRECTDRAWSURFACE3 surf, LPRECT rect, long FillColor);
void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count);
bool DXCreateDirectDraw(DEVICEINFO* dev, DXCONFIG* conf, LPDIRECTDRAW2* ddx);
bool DXCreateDirect3D(LPDIRECTDRAW2 ddx, LPDIRECT3D2* d3dx);
bool DXSetCooperativeLevel(LPDIRECTDRAW2 ddx, HWND hwnd, long flags);
BOOL CALLBACK DXEnumDirectInput(LPCDIDEVICEINSTANCE lpDevInst, LPVOID lpContext);
HRESULT CALLBACK DXEnumDisplayModes(LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext);
bool DXCreateZBuffer(DEVICEINFO* device, DXCONFIG* config);
BOOL CALLBACK DXEnumDirectDraw(GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext);
HRESULT CALLBACK DXEnumTextureFormats(LPDDSURFACEDESC lpDDPixFmt, LPVOID lpContext);
HRESULT CALLBACK DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);

#define DXSwitchVideoMode	( (bool(__cdecl*)(long, long, bool)) 0x004B3550 )
#define DXEnumDirect3D	( (HRESULT(CALLBACK*)(LPGUID, LPSTR, LPSTR, LPD3DDEVICEDESC, LPD3DDEVICEDESC, LPVOID)) 0x0048F3C0 )
