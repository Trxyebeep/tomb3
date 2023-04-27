#include "../tomb3/pch.h"
#include "drawbars.h"
#include "hwrender.h"
#include "../3dsystem/3d_gen.h"
#include "output.h"
#include "../3dsystem/3d_gen.h"
#include "winmain.h"
#include "display.h"
#include "../game/control.h"
#include "../tomb3/tomb3.h"

static ulong InterpolateColor(ulong col0, ulong col1, ulong value, ulong range)
{
	uchar* c0;
	uchar* c1;
	uchar* res;
	ulong result;

	if (!value)
		return col0;

	if (value == range)
		return col1;

	result = 0;
	c0 = (BYTE*)&col0;
	c1 = (BYTE*)&col1;
	res = (BYTE*)&result;

	for (int i = 0; i < 4; i++)
		res[i] = uchar((ulong)c0[i] * (range - value) / range + (ulong)c1[i] * value / range);

	return result;
}

static void DrawColoredRect(long x0, long y0, long x1, long y1, long z, ulong c0, ulong c1, ulong c2, ulong c3)
{
	D3DTLVERTEX v[4];
	float zv, fz;

	zv = one / (float)z;
	fz = f_a - zv * f_boo;

	v[0].sx = (float)x0;
	v[0].sy = (float)y0;
	v[0].color = RGBA_SETALPHA(c0, 0xFF);

	v[1].sx = (float)x1;
	v[1].sy = (float)y0;
	v[1].color = RGBA_SETALPHA(c1, 0xFF);

	v[2].sx = (float)x0;
	v[2].sy = (float)y1;
	v[2].color = RGBA_SETALPHA(c2, 0xFF);

	v[3].sx = (float)x1;
	v[3].sy = (float)y1;
	v[3].color = RGBA_SETALPHA(c3, 0xFF);

	for (int i = 0; i < 4; i++)
	{
		v[i].sz = fz;
		v[i].rhw = zv;
		v[i].specular = 0;
	}

	HWR_SetCurrentTexture(0);
	HWR_EnableColorAddition(0);
#if (DIRECT3D_VERSION >= 0x900)
	HWR_EnableAlphaBlend(1);
	DrawPrimitive(D3DPT_TRIANGLESTRIP, v, 4);
#else
	HWR_EnableColorKey(1);
	DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DVT_TLVERTEX, v, 4, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
#endif
}

static void DrawPSXBar(long x0, long y0, long x1, long y1, long bar, long p, ulong* l, ulong* r, ulong* f)
{
	ulong dl[4];
	ulong dr[4];
	long dy[4];
	long dh;

	DrawColoredRect(x0 - p * 3, y0 - p * 1, x1 + p * 3, y1 + p * 1, phd_znear + 40, f[4], f[5], f[5], f[4]);
	DrawColoredRect(x0 - p * 2, y0 - p * 2, x1 + p * 2, y1 + p * 2, phd_znear + 30, f[2], f[3], f[3], f[2]);
	DrawColoredRect(x0 - p * 1, y0 - p * 1, x1 + p * 1, y1 + p * 1, phd_znear + 20, f[0], f[1], f[1], f[0]);

	if (bar > 0)
	{
		dh = (y1 - y0) - p * 2;

		for (int i = 0; i < 4; i++)
		{
			dy[i] = dh * i / 3;

			if (i > 0 && i < 3)
			{
				dl[i] = InterpolateColor(l[i - 1], l[i], dy[i], dh);
				dr[i] = InterpolateColor(r[i - 1], r[i], dy[i], dh);
			}
			else
			{
				dl[i] = l[i];
				dr[i] = r[i];
			}
		}

		for (int i = 0; i < 3; i++)
			DrawColoredRect(x0, y1 - dy[i + 1], x0 + bar, y1 - dy[i], phd_znear + 10, dl[i + 1], dr[i + 1], dl[i], dr[i]);

		DrawColoredRect(x0, y0 + p * 0, x0 + bar, y0 + p * 1, phd_znear + 10, l[2], r[2], l[3], r[3]);
		DrawColoredRect(x0, y0 + p * 1, x0 + bar, y0 + p * 2, phd_znear + 10, l[5], r[5], l[4], r[4]);
	}
}

static void InsertPSXBar(long polytype, long x0, long y0, long x1, long y1, long bar, long p)
{
	long* sort;
	short* info;

	sort = sort3dptrbf;
	info = info3dptrbf;
	
	surfacenumbf++;
	sort3dptrbf += 3;
	info3dptrbf += 6;

	sort[0] = (long)info;
	sort[1] = phd_znear;
	sort[2] = polytype;

	info[0] = (short)x0;
	info[1] = (short)y0;
	info[2] = (short)x1;
	info[3] = (short)y1;
	info[4] = (short)bar;
	info[5] = (short)p;
}

void DoPSXHealthBar(long x0, long y0, long x1, long y1, long bar, long p)
{
	ulong l[6] = { 0xFF400000, 0xFF4C0000, 0xFF580000, 0xFF640000, 0xFF700000, 0xFF7C0000 };
	ulong r[6] = { 0xFF008000, 0xFF009900, 0xFF00B200, 0xFF00CB00, 0xFF00E400, 0xFF00FD00 };
	ulong f[6] = { 0xFF000000, 0xFF000000, 0xFF508484, 0xFFA0A0A0, 0xFF284242, 0xFF505050 };

	if (PoisonFlag)
	{
		if (tomb3.improved_poison_bar)
		{
			r[0] = 0xFF400080;
			r[1] = 0xFF4C0099;
			r[2] = 0xFF5800B2;
			r[3] = 0xFF6400CB;
			r[4] = 0xFF7000E4;
			r[5] = 0xFF7C00FD;
		}
		else
		{
			r[0] = 0xFF606000;
			r[1] = 0xFF7C7C00;
			r[2] = 0xFF989800;
			r[3] = 0xFFB4B400;
			r[4] = 0xFFD0D000;
			r[5] = 0xFFECEC00;

			l[0] = 0xFF606000;
			l[1] = 0xFF7C7C00;
			l[2] = 0xFF989800;
			l[3] = 0xFFB4B400;
			l[4] = 0xFFD0D000;
			l[5] = 0xFFECEC00;
		}
	}

	for (int i = 0; i < 6; i++)
		r[i] = InterpolateColor(l[i], r[i], bar, x1 - x0);

	DrawPSXBar(x0, y0, x1, y1, bar, p, l, r, f);
}

void DoPSXDashBar(long x0, long y0, long x1, long y1, long bar, long p)
{
	ulong l[6] = { 0xFF900000, 0xFFA30000, 0xFFB60000, 0xFFC90000, 0xFFDC0000, 0xFFEF0000 };
	ulong r[6] = { 0xFF909000, 0xFFA3A300, 0xFFB6B600, 0xFFC9C900, 0xFFDCDC00, 0xFFEFEF00 };
	ulong f[6] = { 0xFF000000, 0xFF000000, 0xFF508484, 0xFFA0A0A0, 0xFF284242, 0xFF505050 };

	for (int i = 0; i < 6; i++)
		r[i] = InterpolateColor(l[i], r[i], bar, x1 - x0);

	DrawPSXBar(x0, y0, x1, y1, bar, p, l, r, f);
}

void DoPSXAirBar(long x0, long y0, long x1, long y1, long bar, long p)
{
	ulong l[6] = { 0xFF004054, 0xFF005064, 0xFF006874, 0xFF007884, 0xFF00848E, 0xFF009098 };
	ulong r[6] = { 0xFF004000, 0xFF005000, 0xFF006800, 0xFF007800, 0xFF008400, 0xFF009000 };
	ulong f[6] = { 0xFF000000, 0xFF000000, 0xFF508484, 0xFFA0A0A0, 0xFF284242, 0xFF505050 };

	for (int i = 0; i < 6; i++)
		r[i] = InterpolateColor(l[i], r[i], bar, x1 - x0);

	DrawPSXBar(x0, y0, x1, y1, bar, p, l, r, f);
}

void DoPSXColdBar(long x0, long y0, long x1, long y1, long bar, long p)
{
	ulong l[6] = { 0xFF000060, 0xFF00006C, 0xFF000078, 0xFF000084, 0xFF000090, 0xFF00009C };
	ulong r[6] = { 0xFF600000, 0xFF6C0000, 0xFF780000, 0xFF840000, 0xFF900000, 0xFF9C0000 };
	ulong f[6] = { 0xFF000000, 0xFF000000, 0xFF508484, 0xFFA0A0A0, 0xFF284242, 0xFF505050 };

	for (int i = 0; i < 6; i++)
		r[i] = InterpolateColor(l[i], r[i], bar, x1 - x0);

	DrawPSXBar(x0, y0, x1, y1, bar, p, l, r, f);
}

void S_DrawHealthBar(long percent)
{
	long w, h, xs, ys, p, x0, y0, x1, y1, bar;

	w = GetRenderScale(100);
	h = GetRenderScale(5);

	if (tomb3.bar_pos == BPOS_PSX)
	{
		xs = GetRenderScale(20);
		ys = GetRenderScale(18);
	}
	else
	{
		xs = GetRenderScale(8);
		ys = GetRenderScale(8);
	}

	p = GetRenderScale(1);

	if (tomb3.bar_pos == BPOS_PSX)
	{
		x1 = phd_winxmin + DumpWidth - xs;
		x0 = x1 - w;
	}
	else
	{
		x0 = phd_winxmin + xs;
		x1 = x0 + w;
	}

	y0 = phd_winymin + ys;
	y1 = y0 + h;

	bar = w * percent / 100;

	if (tomb3.bar_mode == BAR_PSX)
	{
		if (App.lpDXConfig->bZBuffer)
			DoPSXHealthBar(x0, y0, x1, y1, bar, p);
		else
			InsertPSXBar(POLYTYPE_HEALTHBAR, x0, y0, x1, y1, bar, p);

		return;
	}

	InsertFlatRect(x0 - p * 2, y0 - p * 2, x1 + p * 2, y1 + p * 2, phd_znear + 50, 0xFFFFFFFF);
	InsertFlatRect(x0 - p, y0 - p, x1 + p * 2, y1 + p * 2, phd_znear + 40, 0xFF404040);
	InsertFlatRect(x0 - p, y0 - p, x1 + p, y1 + p, phd_znear + 30, 0);
	
	if (bar > 0)
	{
		if (PoisonFlag)
		{
			InsertFlatRect(x0, y0, x0 + bar, y0 + h, phd_znear + 20, 0xFFC0C000);
			InsertFlatRect(x0, y0 + p, x0 + bar, y0 + p * 2, phd_znear + 10, 0xFFA0A000);
		}
		else
		{
			InsertFlatRect(x0, y0, x0 + bar, y0 + h, phd_znear + 20, 0xFFFF0000);
			InsertFlatRect(x0, y0 + p, x0 + bar, y0 + p * 2, phd_znear + 10, 0xFFFF8000);
		}
	}
}

void S_DrawDashBar(long percent)
{
	long w, h, xs, ys, p, x0, y0, x1, y1, bar;

	w = GetRenderScale(100);
	h = GetRenderScale(5);

	if (tomb3.bar_pos == BPOS_PSX)
	{
		xs = GetRenderScale(20);
		ys = GetRenderScale(46);
	}
	else
	{
		xs = GetRenderScale(8);

		if (tomb3.bar_pos == BPOS_IMPROVED)
			ys = GetRenderScale(8);
		else
			ys = GetRenderScale(25);
	}

	p = GetRenderScale(1);

	x1 = phd_winxmin + DumpWidth - xs;
	x0 = x1 - w;
	y0 = phd_winymin + ys;
	y1 = y0 + h;

	bar = w * percent / 100;

	if (tomb3.bar_mode == BAR_PSX)
	{
		if (App.lpDXConfig->bZBuffer)
			DoPSXDashBar(x0, y0, x1, y1, bar, p);
		else
			InsertPSXBar(POLYTYPE_DASHBAR, x0, y0, x1, y1, bar, p);

		return;
	}

	InsertFlatRect(x0 - p * 2, y0 - p * 2, x1 + p * 2, y1 + p * 2, phd_znear + 50, 0xFFFFFFFF);
	InsertFlatRect(x0 - p, y0 - p, x1 + p * 2, y1 + p * 2, phd_znear + 40, 0xFF404040);
	InsertFlatRect(x0 - p, y0 - p, x1 + p, y1 + p, phd_znear + 30, 0);

	if (bar > 0)
	{
		InsertFlatRect(x0, y0, x0 + bar, y0 + h, phd_znear + 20, 0xFF00C000);
		InsertFlatRect(x0, y0 + p, x0 + bar, y0 + p * 2, phd_znear + 10, 0xFF00A000);
	}
}

void S_DrawAirBar(long percent)
{
	long w, h, xs, ys, p, x0, y0, x1, y1, bar;

	w = GetRenderScale(100);
	h = GetRenderScale(5);

	if (tomb3.bar_pos == BPOS_PSX)
	{
		xs = GetRenderScale(20);
		ys = GetRenderScale(32);
	}
	else
	{
		xs = GetRenderScale(8);

		if (tomb3.bar_pos == BPOS_IMPROVED)
			ys = GetRenderScale(25);
		else
			ys = GetRenderScale(8);
	}

	p = GetRenderScale(1);

	x1 = phd_winxmin + DumpWidth - xs;
	x0 = x1 - w;
	y0 = phd_winymin + ys;
	y1 = y0 + h;

	bar = w * percent / 100;

	if (tomb3.bar_mode == BAR_PSX)
	{
		if (App.lpDXConfig->bZBuffer)
			DoPSXAirBar(x0, y0, x1, y1, bar, p);
		else
			InsertPSXBar(POLYTYPE_AIRBAR, x0, y0, x1, y1, bar, p);

		return;
	}

	InsertFlatRect(x0 - p * 2, y0 - p * 2, x1 + p * 2, y1 + p * 2, phd_znear + 50, 0xFFFFFFFF);
	InsertFlatRect(x0 - p, y0 - p, x1 + p * 2, y1 + p * 2, phd_znear + 40, 0xFF404040);
	InsertFlatRect(x0 - p, y0 - p, x1 + p, y1 + p, phd_znear + 30, 0);

	if (bar > 0)
	{
		InsertFlatRect(x0, y0, x0 + bar, y0 + h, phd_znear + 20, 0xFF0000FF);
		InsertFlatRect(x0, y0 + p, x0 + bar, y0 + p * 2, phd_znear + 10, 0xFFFFFFFF);
	}
}

void S_DrawColdBar(long percent)
{
	long w, h, xs, ys, p, x0, y0, x1, y1, bar;

	w = GetRenderScale(100);
	h = GetRenderScale(5);

	if (tomb3.bar_pos == BPOS_PSX)
	{
		xs = GetRenderScale(20);
		ys = GetRenderScale(60);
	}
	else
	{
		xs = GetRenderScale(8);
		ys = GetRenderScale(25);
	}

	p = GetRenderScale(1);

	if (tomb3.bar_pos == BPOS_PSX)
	{
		x1 = phd_winxmin + DumpWidth - xs;
		x0 = x1 - w;
	}
	else
	{
		x0 = phd_winxmin + xs;
		x1 = x0 + w;
	}

	y0 = phd_winymin + ys;
	y1 = y0 + h;

	bar = w * percent / 100;

	if (tomb3.bar_mode == BAR_PSX)
	{
		if (App.lpDXConfig->bZBuffer)
			DoPSXColdBar(x0, y0, x1, y1, bar, p);
		else
			InsertPSXBar(POLYTYPE_COLDBAR, x0, y0, x1, y1, bar, p);

		return;
	}

	InsertFlatRect(x0 - p * 2, y0 - p * 2, x1 + p * 2, y1 + p * 2, phd_znear + 50, 0xFFFFFFFF);
	InsertFlatRect(x0 - p, y0 - p, x1 + p * 2, y1 + p * 2, phd_znear + 40, 0xFF404040);
	InsertFlatRect(x0 - p, y0 - p, x1 + p, y1 + p, phd_znear + 30, 0);

	if (bar > 0)
	{
		InsertFlatRect(x0, y0, x0 + bar, y0 + h, phd_znear + 20, 0xFF00C0C0);
		InsertFlatRect(x0, y0 + p, x0 + bar, y0 + p * 2, phd_znear + 10, 0xFF00A0A0);
	}
}
