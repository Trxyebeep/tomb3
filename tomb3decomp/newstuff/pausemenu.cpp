#include "../tomb3/pch.h"
#include "pausemenu.h"
#include "../game/invfunc.h"
#include "../game/inventry.h"
#include "../game/text.h"
#include "../specific/display.h"
#include "../game/sound.h"
#include "../specific/specific.h"
#include "../specific/output.h"
#include "../specific/input.h"
#include "../specific/picture.h"
#include "../game/health.h"
#include "../game/objects.h"

static TEXTSTRING* pause_text;
static REQUEST_INFO Pause_Requester = {0};

static long ShowPauseRequester(const char* txt, const char* opt1, const char* opt2, ushort selected)
{
	static long mode;
	long s;

	if (mode == 1)
	{
		s = Display_Requester(&Pause_Requester, 0, 0);

		if (s)
			mode = 0;
		else
		{
			inputDB = 0;
			input = 0;
		}

		return s;
	}

	Pause_Requester.noselector = 0;
	SetPCRequesterSize(&Pause_Requester, 2, -48);
	Pause_Requester.line_height = 18;
	Pause_Requester.item = 0;
	Pause_Requester.selected = selected;
	Pause_Requester.line_offset = 0;
	Pause_Requester.line_oldoffset = 0;
	Pause_Requester.pixwidth = 100;
	Pause_Requester.xpos = 0;
	Pause_Requester.zpos = 0;
	Pause_Requester.itemtexts1 = &Valid_Level_Strings[0][0];
	Pause_Requester.itemtexts2 = &Valid_Level_Strings2[0][0];
	Pause_Requester.itemtextlen = 50;
	Init_Requester(&Pause_Requester);
	SetRequesterHeading(&Pause_Requester, txt, R_CENTRE | R_NO_TIME, 0, 0);
	AddRequesterItem(&Pause_Requester, opt1, R_CENTRE, 0, 0);
	AddRequesterItem(&Pause_Requester, opt2, R_CENTRE, 0, 0);
	inputDB = 0;
	input = 0;
	mode = 1;
	return -1;
}

static long DoPauseRequester()
{
	static long page;
	long selected, p;

	selected = 0;
	inputDB = GetDebouncedInput(input);

	switch (page)
	{
	case 0:

		if (inputDB & IN_PAUSE)
		{
			selected = 1;
			break;
		}

		if (input & IN_OPTION)
			page = 1;

		if (page != 1)
			break;

	case 1:

		p = ShowPauseRequester("Exit to title?", "Continue", "Quit", 0);

		if (p == 1)
		{
			selected = 1;
			break;
		}
		else if (p == 2)
		{
			page = 2;
			break;
		}

		if (page != 2)
			break;

	case 2:

		p = ShowPauseRequester("Are you sure?", "Yes", "No", 1);

		if (p == 1)
		{
			selected = -1;
			break;
		}
		else if (p == 2)
		{
			selected = 1;
			break;
		}

		break;
	}

	if (selected)
		page = 0;

	return selected;
}

long S_Pause()
{
	long flag, selected;

	flag = overlay_flag;
	overlay_flag = -3;

	if (T_RemovePrint(ammotext))
		ammotext = 0;

	S_FadeInInventory(1);
	SOUND_Stop();
	S_CDVolume(0);
	TempVideoAdjust(HiResFlag, 1);
	VidSizeLocked = 1;
	S_SetupAboveWater(0);
	CreateMonoScreen();
	selected = 0;

	do
	{
		S_InitialisePolyList(0);
		DrawMonoScreen(0, 0, 0);

		if (!pause_text)
		{
			pause_text = T_Print(0, -24, 5, "Paused");
			T_CentreH(pause_text, 1);
			T_BottomAlign(pause_text, 1);
		}

		T_DrawText();
		S_OutputPolyList();
		SOUND_EndScene();
		S_DumpScreen();

	} while (!S_UpdateInput() && !(selected = DoPauseRequester()));

	Remove_Requester(&Pause_Requester);

	if (T_RemovePrint(pause_text))
		pause_text = 0;

	VidSizeLocked = 0;
	TempVideoRemove();
	S_CDVolume(25 * Option_Music_Volume + 5);
	S_FadeOutInventory(1);
	Inventory_Chosen = PASSPORT_OPTION;
	Inventory_ExtraData[0] = 2;
	Inventory_Mode = INV_GAME_MODE;
	overlay_flag = flag;
	RemoveMonoScreen(0);
	return selected < 0;
}
