#include "../tomb3/pch.h"
#include "mmx.h"

void MMXBlit32to15(uchar* dest, ulong* src, long w)
{
	__int64 m0 = 0x2000000820000008;	//mul factor
	__int64 m1 = 0x00F800F800F800F8;	//rgb mask1
	__int64 m2 = 0x0000F8000000F800;	//rgb mask2

	__asm
	{
		mov eax, [w]
		mov ebx, [src]
		sub eax, 1
		mov edx, [dest]
		and eax, 0FFFFFFF8h
		movq mm7, [m0]
		movq mm6, [m2]
		movq mm2, [ebx + eax * 4 + 8]
		movq mm0, [ebx + eax * 4]
		movq mm3, mm2
		pand mm3, [m1]
		movq mm1, mm0
		pand mm1, [m1]
		pmaddwd mm3, mm7
		pmaddwd mm1, mm7
		pand mm2, mm6

	lp:
		movq mm4, [ebx + eax * 4 + 18h]
		pand mm0, mm6
		movq mm5, [ebx + eax * 4 + 10h]
		por mm3, mm2
		psrld mm3, 6
		por mm1, mm0
		movq mm0, mm4
		psrld mm1, 6
		pand mm0, [m1]
		packssdw mm1, mm3
		movq mm3, mm5
		pmaddwd mm0, mm7
		pand mm3, [m1]
		pand mm4, mm6
		movq[edx + eax * 2], mm1
		pmaddwd mm3, mm7
		sub eax, 8
		por mm4, mm0
		pand mm5, mm6
		psrld mm4, 6
		movq mm2, [ebx + eax * 4 + 8]
		por mm5, mm3
		movq mm0, [ebx + eax * 4]
		psrld mm5, 06
		movq mm3, mm2
		movq mm1, mm0
		pand mm3, [m1]
		packssdw mm5, mm4
		pand mm1, [m1]
		pand mm2, mm6
		movq[edx + eax * 2 + 18h], mm5
		pmaddwd mm3, mm7
		pmaddwd mm1, mm7
		jg lp
		movq mm4, [ebx + eax * 4 + 18h]
		pand mm0, mm6
		movq mm5, [ebx + eax * 4 + 10h]
		por mm3, mm2
		psrld mm3, 6
		por mm1, mm0
		movq mm0, mm4
		psrld mm1, 6
		pand mm0, [m1]
		packssdw mm1, mm3
		movq mm3, mm5
		pmaddwd mm0, mm7
		pand mm3, [m1]
		pand mm4, mm6
		movq[edx + eax * 2], mm1
		pmaddwd mm3, mm7
		sub eax, 8
		por mm4, mm0
		pand mm5, mm6
		psrld mm4, 6
		por mm5, mm3
		psrld mm5, 6
		packssdw mm5, mm4
		movq[edx + eax * 2 + 18h], mm5
		emms
	}
}

void MMXBlit32to16(uchar* dest, ulong* src, long w)
{
	__int64 rm = 0x00F8000000F80000;	//R mask
	__int64 gm = 0x0000FC000000FC00;	//G mask
	__int64 bm = 0x000000F8000000F8;	//B mask

	__asm
	{
		mov eax, w
		shr eax, 2
		mov esi, [src]
		mov edi, [dest]
		movq mm3, [rm]
		movq mm7, [gm]
		lea ecx, [ecx + 0]		;alignment before the loop?

	lp:
		movq mm0, [esi]
		movq mm4, [esi + 8]
		movq mm1, mm0
		movq mm2, mm0
		movq mm5, mm4
		movq mm6, mm4
		pand mm0, mm3
		pand mm1, mm7
		pand mm2, [bm]
		pand mm4, mm3
		pand mm5, mm7
		pand mm6, [bm]
		pslld mm0, 8
		pslld mm4, 8
		psrad mm0, 10h
		psrld mm1, 5
		psrld mm2, 3
		psrad mm4, 10h
		psrld mm5, 5
		psrld mm6, 3
		por mm0, mm1
		por mm4, mm5
		por mm0, mm2
		por mm4, mm6
		packssdw mm0, mm4
		movq[edi], mm0
		add edi, 8
		add esi, 10h
		dec eax
		jne lp
		emms
	}
}

void MMXBlit32to24(uchar* dest, ulong* src, long w)
{
	__int64 c0 = 0x0000000000FFFFFF;	//rgb1
	__int64 c1 = 0x00FFFFFF00000000;	//rgb2
	__int64 c2 = 0x000000000000FFFF;	//gb1
	__int64 c3 = 0x0000000000FF0000;	//r1

	__asm
	{
		mov esi, [src]
		mov edi, [dest]
		mov eax, w

	lp:
		movq mm0, [esi]
		movq mm1, [esi + 8]
		movq mm2, mm0
		movq mm3, mm1
		sub eax, 4
		movq mm4, mm1
		pand mm0, [c0]
		pand mm1, [c2]
		pand mm2, [c1]
		psrlq mm2, 8
		psllq mm1, 30h
		por mm1, mm2
		por mm0, mm1
		pand mm3, [c3]
		pand mm4, [c1]
		psrlq mm3, 10h
		psrlq mm4, 18h
		por mm3, mm4
		lea esi, [esi + 10h]
		movq[edi], mm0
		movd[edi + 8], mm3
		lea edi, [edi + 0Ch]
		jne lp
		emms
	}
}

void inject_mmx(bool replace)
{
	INJECT(0x00496930, MMXBlit32to15, replace);
	INJECT(0x00496A70, MMXBlit32to16, replace);
	INJECT(0x00496B20, MMXBlit32to24, replace);
}
