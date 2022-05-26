#include "../tomb3/pch.h"
#include "input.h"

long Key(long number)
{
	short key;

	key = layout[1][number];

	if (key < 256)
	{
		if (key_pressed(key))
			return 1;

		switch (key)
		{
		case DIK_RCONTROL:
			return key_pressed(DIK_LCONTROL);

		case DIK_LCONTROL:
			return key_pressed(DIK_RCONTROL);

		case DIK_RSHIFT:
			return key_pressed(DIK_LSHIFT);

		case DIK_LSHIFT:
			return key_pressed(DIK_RSHIFT);

		case DIK_RMENU:
			return key_pressed(DIK_LMENU);

		case DIK_LMENU:
			return key_pressed(DIK_RMENU);
		}
	}
	else if (joy_fire & (1 << key))
		return 1;

	if (conflict[number])
		return 0;

	key = layout[0][number];

	if (key_pressed(key))
		return 1;

	switch (key)
	{
	case DIK_RCONTROL:
		return key_pressed(DIK_LCONTROL);

	case DIK_LCONTROL:
		return key_pressed(DIK_RCONTROL);

	case DIK_RSHIFT:
		return key_pressed(DIK_LSHIFT);

	case DIK_LSHIFT:
		return key_pressed(DIK_RSHIFT);

	case DIK_RMENU:
		return key_pressed(DIK_LMENU);

	case DIK_LMENU:
		return key_pressed(DIK_RMENU);
	}

	return 0;
}

void inject_input(bool replace)
{
	INJECT(0x00486140, Key, replace);
}
