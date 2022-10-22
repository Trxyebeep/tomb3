#include "../tomb3/pch.h"
#include "di.h"

#define Keyboard	VAR_(0x006302A8, LPDIRECTINPUTDEVICE)
#define lpDirectInput	VAR_(0x006302A4, LPDIRECTINPUT)

void DI_ReadKeyboard(uchar* KeyMap)
{
	HRESULT state;

	state = Keyboard->GetDeviceState(256, KeyMap);

	if (FAILED(state) && state == DIERR_INPUTLOST)
	{
		if (FAILED(Keyboard->Acquire()) || FAILED(Keyboard->GetDeviceState(256, KeyMap)))
			memset(KeyMap, 0, 256);
	}
}

long DI_ReadJoystick(long& x, long& y)
{
	JOYINFOEX joystick;
	static JOYCAPS caps;
	static long unavailable = 1;

	if (!App.DXConfig.Joystick)
	{
		x = 0;
		y = 0;
		return 0;
	}

	joystick.dwSize = sizeof(JOYINFOEX);
	joystick.dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS;

	if (joyGetPosEx(0, &joystick) != JOYERR_NOERROR)
	{
		unavailable = 1;
		x = 0;
		y = 0;
		return 0;
	}

	if (unavailable)
	{
		if (joyGetDevCaps(JOYSTICKID1, &caps, sizeof(caps)) != JOYERR_NOERROR)
		{
			x = 0;
			y = 0;
			return 0;
		}
		else
			unavailable = 0;
	}

	x = (joystick.dwXpos << 5) / (caps.wXmax - caps.wXmin) - 16;
	y = (joystick.dwYpos << 5) / (caps.wYmax - caps.wYmin) - 16;
	return joystick.dwButtons;
}

void DI_StartKeyboard()
{
	if (FAILED(lpDirectInput->CreateDevice(GUID_SysKeyboard, &Keyboard, 0)))
		throw 6;

	if (FAILED(Keyboard->SetCooperativeLevel(App.WindowHandle, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
		throw 7;

	if (FAILED(Keyboard->SetDataFormat(&c_dfDIKeyboard)))
		throw 8;

	if (FAILED(Keyboard->Acquire()))
		throw 9;
}

void DI_FinishKeyboard()
{
	if (Keyboard)
	{
		Keyboard->Unacquire();

		if (Keyboard)
		{
			Keyboard->Release();
			Keyboard = 0;
		}
	}
}

bool DI_StartJoystick()
{
	return 1;
}

void DI_Start()
{
	if (!DI_Create())
		throw 5;

	DI_StartKeyboard();
	DI_StartJoystick();
}

void inject_di(bool replace)
{
	INJECT(0x00475450, DI_ReadKeyboard, replace);
	INJECT(0x004754B0, DI_ReadJoystick, replace);
	INJECT(0x004755B0, DI_StartKeyboard, replace);
	INJECT(0x00475680, DI_FinishKeyboard, replace);
	INJECT(0x004756B0, DI_StartJoystick, replace);
	INJECT(0x004756C0, DI_Start, replace);
}
