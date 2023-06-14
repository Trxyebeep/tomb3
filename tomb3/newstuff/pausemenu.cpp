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
#include "../game/health.h"
#include "../game/objects.h"
#include "../tomb3/tomb3.h"
#include "../specific/smain.h"
#include "../game/camera.h"
#include "../game/control.h"
#include "../game/gameflow.h"
#if (DIRECT3D_VERSION >= 0x900)
#include "Picture2.h"
#else
#include "../specific/picture.h"
#endif

static TEXTSTRING* pause_text;
static REQUEST_INFO Pause_Requester = {0};

static const char* GetStupidText(long type)	//0-> Exit to Title?, 1-> Are you sure?, 2-> Yes, 3-> No, 4-> PAUSED
{
	static char buf[64];

	switch (gameflow.language)
	{
	case 1:	//French

		if (!type)
			strcpy(buf, "Retour au menu principal ?");
		else if (type == 1)
			strcpy(buf, "(etes-vous s(ur ?");
		else if (type == 2)
			strcpy(buf, "Oui");
		else if (type == 3)
			strcpy(buf, "Non");
		else
			strcpy(buf, "PAUSE");

		break;

	case 2:	//German

		if (!type)
			strcpy(buf, "Zur~uck zum Titelbild?");
		else if (type == 1)
			strcpy(buf, "Sind Sie sicher?");
		else if (type == 2)
			strcpy(buf, "Ja");
		else if (type == 3)
			strcpy(buf, "Nein");
		else
			strcpy(buf, "PAUSE");

		break;

	case 5:	//Italian

		if (!type)
			strcpy(buf, "Tornare alla schermata principale?");
		else if (type == 1)
			strcpy(buf, "Sei sicuro?");
		else if (type == 2)
			strcpy(buf, "S$i");
		else if (type == 3)
			strcpy(buf, "No");
		else
			strcpy(buf, "PAUSA");

		break;

	case 6:	//Spanish

		if (!type)
			strcpy(buf, "Volver al Men)u?");
		else if (type == 1)
			strcpy(buf, "Est)as seguro?");
		else if (type == 2)
			strcpy(buf, "S)i");
		else if (type == 3)
			strcpy(buf, "No");
		else
			strcpy(buf, "PAUSADO");

		break;

	case 0:	//English
	case 3:	//"American"
	case 4:	//Japanese
	default:

		if (!type)
			strcpy(buf, "Exit to Title?");
		else if (type == 1)
			strcpy(buf, "Are you sure?");
		else if (type == 2)
			strcpy(buf, "Yes");
		else if (type == 3)
			strcpy(buf, "No");
		else
			strcpy(buf, "PAUSED");

		break;
	}

	return buf;
}

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
	Pause_Requester.line_height = 19;
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
	char t[64], y[32], n[32];

	selected = 0;
	inputDB = GetDebouncedInput(input);

	strcpy(y, GetStupidText(2));
	strcpy(n, GetStupidText(3));

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
		strcpy(t, GetStupidText(0));
		p = ShowPauseRequester(t, y, n, 1);

		if (p == 1)
		{
			page = 2;
			break;
			
		}
		else if (p == 2)
		{
			selected = 1;
			break;
		}

		if (page != 2)
			break;

	case 2:
		strcpy(t, GetStupidText(1));
		p = ShowPauseRequester(t, y, n, 1);

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
	float vol;
	long flag, selected;

	flag = overlay_flag;
	overlay_flag = -3;

	T_RemovePrint(ammotext);
	ammotext = 0;

	SOUND_Stop();
	S_CDVolume(0);
	TempVideoAdjust(HiResFlag, 1.0);
	VidSizeLocked = 1;
	S_SetupAboveWater(0);
	CreateMonoScreen();
	selected = 0;

	do
	{
		S_InitialisePolyList(0);
		DrawMonoScreen(112, 80, 80);

		if (!pause_text)
		{
			pause_text = T_Print(0, -24, 5, GetStupidText(4));
			T_CentreH(pause_text, 1);
			T_BottomAlign(pause_text, 1);
		}

		T_DrawText();
		S_OutputPolyList();
		SOUND_EndScene();
		S_DumpScreen();

	} while (!S_UpdateInput() && !(selected = DoPauseRequester()));

	Remove_Requester(&Pause_Requester);

	T_RemovePrint(pause_text);
	pause_text = 0;

	VidSizeLocked = 0;
	TempVideoRemove();

	if (camera.underwater)
	{
		vol = (1.0F - tomb3.unwater_music_mute) * float(25 * Option_Music_Volume + 5);

		if (vol >= 1)
			S_CDVolume((long)vol);
		else
			S_CDVolume(0);
	}
	else
		S_CDVolume(25 * Option_Music_Volume + 5);

	Inventory_Chosen = PASSPORT_OPTION;
	Inventory_ExtraData[0] = 2;
	Inventory_Mode = INV_GAME_MODE;
	overlay_flag = flag;
	RemoveMonoScreen(0);
	return selected < 0;
}
