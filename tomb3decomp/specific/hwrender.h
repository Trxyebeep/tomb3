#pragma once
#include "../global/vars.h"

void inject_hwrender(bool replace);

void HWR_EnableZBuffer(bool write, bool compare);
void HWR_EnableColorKey(bool enable);
void HWR_EnableAlphaBlend(bool enable);
void HWR_EnableColorAddition(bool enable);
#ifdef TROYESTUFF
void HWR_EnableColorSubtraction(bool enable);
#endif
void HWR_ResetZBuffer();
void HWR_ResetColorKey();
void HWR_EnablePerspCorrect(bool enable);
void HWR_EnableFilter(bool enable);
void HWR_ResetCurrentTexture();
void HWR_BeginScene();
void HWR_EndScene();
void HWR_DrawRoutines(long nVtx, D3DTLVERTEX* vtx, long nDrawType, long TPage);
void HWR_DrawRoutinesStippledAlpha(long nVtx, D3DTLVERTEX* vtx, long nDrawType, long TPage);
void HWR_DrawRoutinesNoAlpha(long nVtx, D3DTLVERTEX* vtx, long nDrawType, long TPage);
#ifdef TROYESTUFF
__inline void HWR_InitGamma(float gamma);
#endif
void HWR_InitState();
bool HWR_Init();
void HWR_DrawPolyList(long num, long* pSort);
void HWR_DrawPolyListBF(long num, long* pSort);
void HWR_FreeTexturePages();

#define HWR_SetCurrentTexture	( (void(__cdecl*)(DXTEXTURE*)) 0x00484C30 )
#define HWR_GetAllTextureHandles	( (void(__cdecl*)()) 0x00485A10 )
#define HWR_LoadTexturePages	( (void(__cdecl*)(long, char*, uchar*)) 0x00485900 )

#define SetRenderState	( *(HRESULT(__cdecl**)(D3DRENDERSTATETYPE, long)) 0x006CA1B8 )	//here FOR NOW
#define DrawPrimitive	( *(HRESULT(__cdecl**)(D3DPRIMITIVETYPE, D3DVERTEXTYPE, LPVOID, ulong, ulong)) 0x006CA1C0 )
#define BeginScene	( *(HRESULT(__cdecl**)()) 0x006CA1C8 )
#define EndScene	( *(HRESULT(__cdecl**)()) 0x006CA1A0 )
#define DrawRoutine	( *(void(__cdecl**)(long, D3DTLVERTEX*, long, long)) 0x00633F68 )
