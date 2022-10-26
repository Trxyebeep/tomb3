#pragma once
#include "../global/vars.h"

void inject_dxshell(bool replace);

long BPPToDDBD(long BPP);
bool DXSetVideoMode(LPDIRECTDRAWX ddx, long w, long h, long bpp);
bool DXCreateSurface(LPDIRECTDRAWX ddx, LPDDSURFACEDESCX desc, LPDIRECTDRAWSURFACEX surf);
bool DXGetAttachedSurface(LPDIRECTDRAWSURFACEX surf, LPDDSCAPSX caps, LPDIRECTDRAWSURFACEX* attached);
bool DXAddAttachedSurface(LPDIRECTDRAWSURFACEX surf, LPDIRECTDRAWSURFACEX attached);
bool DXCreateDirect3DDevice(LPDIRECT3DX dd3x, GUID guid, LPDIRECTDRAWSURFACEX surf, LPDIRECT3DDEVICEX* device);
bool DXCreateViewPort(LPDIRECT3DX dd3x, LPDIRECT3DDEVICEX device, long w, long h, LPDIRECT3DVIEWPORTX* lpvp);
void DXGetSurfaceDesc(LPDIRECTDRAWSURFACEX surf, LPDDSURFACEDESCX desc);
bool DXSurfBlt(LPDIRECTDRAWSURFACEX surf, LPRECT rect, long FillColor);
void DXBitMask2ShiftCnt(ulong mask, uchar* shift, uchar* count);
bool DXCreateDirectDraw(DEVICEINFO* dev, DXCONFIG* conf, LPDIRECTDRAWX* ddx);
bool DXCreateDirect3D(LPDIRECTDRAWX ddx, LPDIRECT3DX* d3dx);
bool DXSetCooperativeLevel(LPDIRECTDRAWX ddx, HWND hwnd, long flags);
BOOL CALLBACK DXEnumDirectInput(LPCDIDEVICEINSTANCE lpDevInst, LPVOID lpContext);
HRESULT CALLBACK DXEnumDisplayModes(LPDDSURFACEDESCX lpDDSurfaceDesc, LPVOID lpContext);
bool DXCreateZBuffer(DEVICEINFO* device, DXCONFIG* config);
BOOL CALLBACK DXEnumDirectDraw(GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext);
HRESULT CALLBACK DXEnumTextureFormats(LPDDSURFACEDESCX lpDDPixFmt, LPVOID lpContext);
BOOL CALLBACK DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);
void DXFreeDeviceInfo(DEVICEINFO* device);
void DXSaveScreen(LPDIRECTDRAWSURFACEX surf);
void DXDoFlipWait();
bool DXCheckForLostSurfaces();
void DXClearBuffers(ulong flags, ulong color);
bool DXUpdateFrame(bool runMessageLoop, LPRECT rect);
void DXGetDeviceInfo(DEVICEINFO* device, HWND hWnd, HINSTANCE hInstance);

#define DXSwitchVideoMode	( (bool(__cdecl*)(long, long, bool)) 0x004B3550 )
#define DXEnumDirect3D	( (HRESULT(CALLBACK*)(LPGUID, LPSTR, LPSTR, LPD3DDEVICEDESC, LPD3DDEVICEDESC, LPVOID)) 0x0048F3C0 )
#define SWRBlit32to15	( (void(__cdecl*)(ulong*, ulong*, long)) 0x004B4040 )
