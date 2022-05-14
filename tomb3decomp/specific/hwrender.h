#pragma once
#include "../global/vars.h"

void inject_hwrender(bool replace);

void HWR_EnableZBuffer(bool write, bool compare);
void HWR_EnableColorKey(bool enable);
void HWR_EnableAlphaBlend(bool enable);
void HWR_EnableColorAddition(bool enable);
void HWR_ResetZBuffer();
void HWR_ResetColorKey();
void HWR_EnablePerspCorrect(bool enable);
void HWR_EnableFilter(bool enable);
void HWR_ResetCurrentTexture();
void HWR_BeginScene();
void HWR_EndScene();

#define HWR_SetCurrentTexture	( (void(__cdecl*)(DXTEXTURE*)) 0x00484C30 )
#define HWR_GetAllTextureHandles	( (void(__cdecl*)()) 0x00485A10 )

#define SetRenderState	( *(HRESULT(__cdecl**)(D3DRENDERSTATETYPE, long)) 0x006CA1B8 )	//here FOR NOW
#define DrawPrimitive	( *(HRESULT(__cdecl**)(D3DPRIMITIVETYPE, D3DVERTEXTYPE, LPVOID, ulong, ulong)) 0x006CA1C0 )
#define BeginScene	( *(HRESULT(__cdecl**)()) 0x006CA1C8 )
#define EndScene	( *(HRESULT(__cdecl**)()) 0x006CA1A0 )
