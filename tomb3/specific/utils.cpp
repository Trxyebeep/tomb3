#include "../tomb3/pch.h"
#include "utils.h"
#include "winmain.h"

static double start_us;
static double start;
static double period;

double UT_GetAccurateTimer()
{
	__int64 counter;

	if (QueryPerformanceCounter((LARGE_INTEGER*)&counter))
		return ((double)counter - start_us) * period;
	else
		return double(timeGetTime() - start) / (double)CLOCKS_PER_SEC;
}

void UT_InitAccurateTimer()
{
	__int64 fq;

	if (QueryPerformanceFrequency((LARGE_INTEGER*)&fq))
	{
		period = 1.0 / (double)fq;
		start_us = 0;
		start_us = UT_GetAccurateTimer();
	}
	else
	{
		period = 0;
		start = timeGetTime();
	}
}

void UT_CenterWindow(HWND hwnd)
{
	RECT window;
	RECT area;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &area, 0);
	GetWindowRect(hwnd, &window);
	SetWindowPos(hwnd, 0,
		((area.left + area.right) >> 1) - ((window.right - window.left) >> 1),
		((area.top + area.bottom) >> 1) - ((window.bottom - window.top) >> 1),
		-1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

char* UT_FindArg(const char* arg)
{
	char* str;

	str = strstr(G_lpCmdLine, arg);

	if (str)
		str += strlen(arg);

	return str;
}

INT_PTR CALLBACK UT_OKCB_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		UT_CenterWindow(hwnd);
		return 1;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hwnd, 1);
			return 1;

		case IDCANCEL:
			EndDialog(hwnd, 0);
			return 1;
		}

		break;
	}

	return 0;
}

bool UT_OKCancelBox(char* lpTemplateName, HWND hWndParent)
{
	bool ret;

	ShowCursor(1);
	ret = DialogBoxParam(App.hInstance, lpTemplateName, hWndParent, UT_OKCB_DlgProc, 0) != 0;
	ShowCursor(0);
	return ret;
}
