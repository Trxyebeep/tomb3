#include "../tomb3/pch.h"
#include "di.h"
#include "winmain.h"

static LPDIRECTINPUTDEVICEX Keyboard;
static LPDIRECTINPUTX lpDirectInput;

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

void DI_StartKeyboard()
{
	if (FAILED(lpDirectInput->CreateDevice(GUID_SysKeyboard, &Keyboard, 0)))
		S_ExitSystem("Keyboard device creation failed.");

	if (FAILED(Keyboard->SetCooperativeLevel(App.WindowHandle, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
		S_ExitSystem("Keyboard SetCooperativeLevel failed");

	if (FAILED(Keyboard->SetDataFormat(&c_dfDIKeyboard)))
		S_ExitSystem("Keyboard SetDataFormat failed");

	if (FAILED(Keyboard->Acquire()))
		S_ExitSystem("Keyboard Acquire failed");
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

void DI_Start()
{
	if (!DI_Create())
		S_ExitSystem("DI_Create failed");

	DI_StartKeyboard();
}

void DI_Finish()
{
	DI_FinishKeyboard();

	if (lpDirectInput)
	{
		lpDirectInput->Release();
		lpDirectInput = 0;
	}
}

bool DI_Create()
{
	return SUCCEEDED(DirectInput8Create(App.hInstance, DIRECTINPUT_VERSION, DIGUID, (LPVOID*)&lpDirectInput, 0));
}
