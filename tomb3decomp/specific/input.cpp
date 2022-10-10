#include "../tomb3/pch.h"
#include "input.h"
#include "workstubs.h"
#include "di.h"
#include "display.h"
#include "../game/invfunc.h"
#include "../game/objects.h"
#include "../game/laramisc.h"

const char* KeyboardButtons[272] =
{
	0,
	"Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "Bksp",
	"Tab", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "<", ">", "Return",
	"Ctrl", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "`",
	"Shift", "#", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "Shift",
	"Padx", "Alt", "Space", "Caps", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Nmlk", 0,
	"Pad7", "Pad8", "Pad9", "Pad-",
	"Pad4", "Pad5", "Pad6", "Pad+",
	"Pad1", "Pad2", "Pad3",
	"Pad0", "Pad.", 0, 0, "\\", 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Enter", "Ctrl", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Shift", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Pad/", 0, 0, "Alt", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Home", "Up", "PgUp", 0, "Left", 0, "Right", 0, "End", "Down", "PgDn", "Ins", "Del",
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Joy 1", "Joy 2", "Joy 3", "Joy 4", "Joy 5", "Joy 6", "Joy 7", "Joy 8",
	"Joy 9", "Joy 10", "Joy 11", "Joy 12", "Joy 13", "Joy 14", "Joy 15", "Joy 16"
};

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

long S_UpdateInput()
{
	long linput;
	static long med_debounce = 0;

	DD_SpinMessageLoop(0);
	DI_ReadKeyboard(keymap);
	joy_fire = DI_ReadJoystick(joy_x, joy_y);
	linput = 0;

	if (joy_x < -8)
		linput |= IN_LEFT;
	else if (joy_x > 8)
		linput |= IN_RIGHT;

	if (joy_y > 8)
		linput |= IN_BACK;
	else if (joy_y < -8)
		linput |= IN_FORWARD;

	if (Key(0))
		linput |= IN_FORWARD;

	if (Key(1))
		linput |= IN_BACK;

	if (Key(2))
		linput |= IN_LEFT;

	if (Key(3))
		linput |= IN_RIGHT;

	if (Key(4))
		linput |= IN_DUCK;

	if (Key(5))
		linput |= IN_SPRINT;

	if (Key(6))
		linput |= IN_WALK;

	if (Key(7))
		linput |= IN_JUMP;

	if (Key(8))
		linput |= IN_ACTION;

	if (Key(9))
		linput |= IN_DRAW;

	if (Key(10))
		linput |= IN_FLARE;

	if (Key(11))
		linput |= IN_LOOK;

	if (Key(12))
		linput |= IN_ROLL;

	if (linput & IN_WALK && !(linput & (IN_FORWARD | IN_BACK)))
	{
		if (linput & IN_LEFT)
			linput = (linput & ~IN_LEFT) | IN_LSTEP;
		else if (linput & IN_RIGHT)
			linput = (linput & ~IN_RIGHT) | IN_RSTEP;
	}

	if (key_pressed(DIK_MULTIPLY))
	{
		farz -= 50;

		if (farz < 0x2000)
			farz = 0x2000;

		distanceFogValue = farz - 0x2000;
	}

	if (key_pressed(DIK_DIVIDE))
	{
		farz += 50;

		if (farz > 0x5000)
			farz = 0x5000;

		distanceFogValue = farz - 0x2000;
	}

	if (Key(13) && camera.type != CINEMATIC_CAMERA && !pictureFading)
		linput |= IN_OPTION;

	if (linput & IN_FORWARD && linput & IN_BACK)
		linput |= IN_ROLL;

	if (key_pressed(DIK_RETURN) || linput & IN_ACTION)
		linput |= IN_SELECT;

	if (key_pressed(DIK_ESCAPE))
		linput |= IN_DESELECT;

	if ((linput & (IN_RIGHT | IN_LEFT)) == (IN_RIGHT | IN_LEFT))
		linput -= IN_RIGHT | IN_LEFT;

	if (GnGameMode == GAMEMODE_IN_GAME && !nLoadedPictures)
	{
		if (key_pressed(DIK_ADD))
			IncreaseScreenSize();

		if (key_pressed(DIK_SUBTRACT))
			DecreaseScreenSize();
	}

	if (key_pressed(DIK_1) && Inv_RequestItem(GUN_OPTION))
		lara.request_gun_type = LG_PISTOLS;
	else if (key_pressed(DIK_2) && Inv_RequestItem(SHOTGUN_OPTION))
		lara.request_gun_type = LG_SHOTGUN;
	else if (key_pressed(DIK_3) && Inv_RequestItem(MAGNUM_OPTION))
		lara.request_gun_type = LG_MAGNUMS;
	else if (key_pressed(DIK_4) && Inv_RequestItem(UZI_OPTION))
		lara.request_gun_type = LG_UZIS;
	else if (key_pressed(DIK_5) && Inv_RequestItem(HARPOON_OPTION))
		lara.request_gun_type = LG_HARPOON;
	else if (key_pressed(DIK_6) && Inv_RequestItem(M16_OPTION))
		lara.request_gun_type = LG_M16;
	else if (key_pressed(DIK_7) && Inv_RequestItem(ROCKET_OPTION))
		lara.request_gun_type = LG_ROCKET;
	else if (key_pressed(DIK_8) && Inv_RequestItem(GRENADE_OPTION))
		lara.request_gun_type = LG_GRENADE;

	if (key_pressed(DIK_0) && Inv_RequestItem(MEDI_OPTION))
	{
		if (!med_debounce)
		{
			UseItem(MEDI_OPTION);
			med_debounce = 15;
		}
	}
	else if (key_pressed(DIK_9) && Inv_RequestItem(BIGMEDI_OPTION))
	{
		if (!med_debounce)
		{
			UseItem(BIGMEDI_OPTION);
			med_debounce = 15;
		}
	}
	else if (med_debounce)
		med_debounce--;

	if (key_pressed(DIK_APOSTROPHE))
		SaveDDBuffer(App.lpFrontBuffer);

	if (FinishLevelCheat)
	{
		level_complete = 1;
		FinishLevelCheat = 0;
	}

	if (!gameflow.loadsave_disabled)
	{
		if (key_pressed(DIK_F5))
			linput |= IN_SAVE;
		else if (key_pressed(DIK_F6))
			linput |= IN_LOAD;
	}

	input = linput;
	return GtWindowClosed;
}

void inject_input(bool replace)
{
	INJECT(0x00486140, Key, replace);
	INJECT(0x00486260, S_UpdateInput, replace);
}
