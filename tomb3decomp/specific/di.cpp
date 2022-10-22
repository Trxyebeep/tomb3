#include "../tomb3/pch.h"
#include "di.h"

#define Keyboard	VAR_(0x006302A8, LPDIRECTINPUTDEVICE)

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

void inject_di(bool replace)
{
	INJECT(0x00475450, DI_ReadKeyboard, replace);
}
