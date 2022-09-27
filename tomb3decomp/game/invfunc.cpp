#include "../tomb3/pch.h"
#include "invfunc.h"
#include "../specific/frontend.h"
#include "../specific/option.h"
#include "text.h"
#include "sound.h"
#include "gameflow.h"
#include "../specific/smain.h"

static ushort req_bgnd_gour1[16] =
{
	0x1E00, 0x1E00, 0x1A00, 0x1E00, 0x1E00, 0x1E00, 0x1E00, 0x1A00,
	0x1A00, 0x1E00, 0x1E00, 0x1E00, 0x1E00, 0x1A00, 0x1E00,0x1E00
};

static ushort req_bgnd_gour2[9] = { 0x1A00, 0x1800, 0x1E00, 0x2000, 0x2000, 0x1E00, 0x1C00, 0x1C00, 0x1A00 };

static ushort req_main_gour1[16] =
{
	0x2000, 0x2000, 0x1800, 0x2000, 0x2000, 0x2000, 0x2000, 0x1800,
	0x1800, 0x2000, 0x2000, 0x2000, 0x2000, 0x1800, 0x2000, 0x2000
};

static ushort req_main_gour2[9] = { 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000 };

static ushort req_unsel_gour1[16] =
{
	0x2000, 0x2000, 0x1A00, 0x2000, 0x2000, 0x2000, 0x2000, 0x1A00,
	0x1A00, 0x2000, 0x2000, 0x2000, 0x2000, 0x1A00, 0x2000, 0x2000
};

static ushort req_sel_gour2[9] = { 0x2000, 0x1010, 0x2000, 0x1400, 0x2000, 0x1010, 0x2000, 0x1400, 0x2000 };

void InitColours()
{
	inv_colours[0] = S_COLOUR(0, 0, 0);
	inv_colours[1] = S_COLOUR(64, 64, 64);
	inv_colours[2] = S_COLOUR(255, 255, 255);
	inv_colours[3] = S_COLOUR(255, 0, 0);
	inv_colours[4] = S_COLOUR(255, 128, 0);
	inv_colours[5] = S_COLOUR(255, 255, 0);
	inv_colours[12] = S_COLOUR(0, 128, 0);
	inv_colours[13] = S_COLOUR(0, 255, 0);
	inv_colours[14] = S_COLOUR(0, 255, 255);
	inv_colours[15] = S_COLOUR(0, 0, 255);
	inv_colours[16] = S_COLOUR(255, 0, 255);
}

/************Requester stuff************/

void Init_Requester(REQUEST_INFO* req)
{
	req->background_flags = 1;
	req->moreup_flags = 1;
	req->moredown_flags = 1;
	req->item = 0;
	req->heading1text = 0;
	req->heading2text = 0;
	req->heading1_flags = 0;
	req->heading2_flags = 0;
	req->backgroundtext = 0;
	req->moreuptext = 0;
	req->moredowntext = 0;

	for (int i = 0; i < 24; i++)
	{
		req->texts1[i] = 0;
		req->texts2[i] = 0;
		req->texts1_flags[i] = 0;
		req->texts2_flags[i] = 0;
	}

	req->itemtexts1_flags = RequesterFlags1;
	req->itemtexts2_flags = RequesterFlags2;
	req->original_render_width = GetRenderWidth();
	req->original_render_height = GetRenderHeight();
}

void Remove_Requester(REQUEST_INFO* req)
{
	T_RemovePrint(req->heading1text);
	req->heading1text = 0;

	T_RemovePrint(req->heading2text);
	req->heading2text = 0;

	T_RemovePrint(req->backgroundtext);
	req->backgroundtext = 0;

	T_RemovePrint(req->moreuptext);
	req->moreuptext = 0;

	T_RemovePrint(req->moredowntext);
	req->moredowntext = 0;

	for (int i = 0; i < 24; i++)
	{
		T_RemovePrint(req->texts1[i]);
		req->texts1[i] = 0;

		T_RemovePrint(req->texts2[i]);
		req->texts2[i] = 0;
	}
}

void ReqItemCentreAlign(REQUEST_INFO* req, TEXTSTRING* txt)
{
	if (txt)
	{
		txt->bgndOffX = 0;
		txt->xpos = req->xpos;
	}
}

void ReqItemLeftalign(REQUEST_INFO* req, TEXTSTRING* txt)
{
	ulong h;

	h = GetTextScaleH(txt->scaleH);

	if (txt)
	{
		txt->bgndOffX = short(((h * req->pixwidth) >> 17) - ((8 * h) >> 16) - T_GetTextWidth(txt) / 2);
		txt->xpos = req->xpos - txt->bgndOffX;
	}
}

void ReqItemRightalign(REQUEST_INFO* req, TEXTSTRING* txt)
{
	ulong h;

	h = GetTextScaleH(txt->scaleH);

	if (txt)
	{
		txt->bgndOffX = -short(((h * req->pixwidth) >> 17) - ((8 * h) >> 16) - T_GetTextWidth(txt) / 2);
		txt->xpos = req->xpos - txt->bgndOffX;
	}
}

long Display_Requester(REQUEST_INFO* req, long des, long backgrounds)
{
	ulong flags;
	long rw, rh, nLines, c;
	long lHeight, lOff;

	lHeight = req->vis_lines * req->line_height + 10;
	lOff = req->ypos - lHeight;
	rw = GetRenderWidth();
	rh = GetRenderHeight();

	if (rw != req->original_render_width || rh != req->original_render_height)
	{
		Remove_Requester(req);
		req->original_render_width = rw;
		req->original_render_height = rh;
	}

	req->itemtexts1_flags = RequesterFlags1;
	req->itemtexts2_flags = RequesterFlags2;

	if (req->item < req->vis_lines)
		nLines = req->item;
	else
		nLines = req->vis_lines;

	for (int i = 0; i < nLines; i++)
	{
		if (req->texts1[i])
		{
			T_RemovePrint(req->texts1[i]);
			req->texts1[i] = 0;
		}

		if (req->texts2[i])
		{
			T_RemovePrint(req->texts2[i]);
			req->texts2[i] = 0;
		}
	}

	flags = req->heading1_flags;

	if (flags & R_USE)
	{
		if (!req->heading1text)
		{
			if (flags & R_HEADING)
				c = 5;
			else if (flags & R_BEST_TIME)
				c = 10;
			else if (flags & R_NORMAL_TIME)
				c = 2;
			else if (flags & R_NO_TIME)
				c = 11;
			else
				c = 0;

			req->heading1text = T_Print(req->xpos, lOff - req->line_height - 10, c, req->heading1_str);
			T_CentreH(req->heading1text, 1);
			T_BottomAlign(req->heading1text, 1);

			if (backgrounds)
			{
				T_AddBackground(req->heading1text, req->pixwidth - 4, 0, 0, 0, 8, 0, req_main_gour1, 2);
				T_AddOutline(req->heading1text, 1, 4, req_main_gour2, 0);
			}
		}

		if (flags & R_LEFTALIGN)
			ReqItemLeftalign(req, req->heading1text);

		if (flags & R_RIGHTALIGN)
			ReqItemRightalign(req, req->heading1text);
	}

	flags = req->heading2_flags;

	if (flags & R_USE)
	{
		if (!req->heading2text)
		{
			req->heading2text = T_Print(req->xpos, lOff - req->line_height - 10, req->zpos, req->heading2_str);
			T_CentreH(req->heading2text, 1);
			T_BottomAlign(req->heading2text, 1);

			if (backgrounds)
			{
				T_AddBackground(req->heading2text, req->pixwidth - 4, 0, 0, 0, 8, 0, req_main_gour1, 2);
				T_AddOutline(req->heading2text, 1, 4, req_main_gour2, 0);
			}
		}

		if (flags & R_LEFTALIGN)
			ReqItemLeftalign(req, req->heading2text);

		if (flags & R_RIGHTALIGN)
			ReqItemRightalign(req, req->heading2text);
	}

	if (backgrounds && !req->backgroundtext && req->background_flags & R_USE)
	{
		req->backgroundtext = T_Print(req->xpos, lOff - req->line_height - 12, 0, " ");
		T_CentreH(req->backgroundtext, 1);
		T_BottomAlign(req->backgroundtext, 1);
		T_AddBackground(req->backgroundtext, req->pixwidth, short(req->line_height + lHeight + 12), 0, 0, 48, 0, req_bgnd_gour1, 1);
		T_AddOutline(req->backgroundtext, 1, 15, req_bgnd_gour2, 0);
	}

	if (!req->line_offset)
	{
		T_RemovePrint(req->moreuptext);
		req->moreuptext = 0;
	}
	else if (!req->moreuptext && req->moreup_flags & R_USE)
	{
		T_CentreH(0, 1);
		T_BottomAlign(req->moreuptext, 1);
	}

	if (req->item <= req->vis_lines + req->line_offset)
	{
		T_RemovePrint(req->moredowntext);
		req->moredowntext = 0;
	}
	else if (!req->moredowntext && (req->moredown_flags & R_USE) != 0)
	{
		T_CentreH(0, 1);
		T_BottomAlign(req->moredowntext, 1);
	}

	for (int i = 0; i < nLines; i++)
	{
		flags = req->itemtexts1_flags[req->line_offset + i];

		if (flags & R_USE)
		{
			if (!req->texts1[i])
			{
				if (flags & R_HEADING)
					c = 5;
				else if (flags & R_BEST_TIME)
					c = 10;
				else if (flags & R_NORMAL_TIME)
					c = 2;
				else if (flags & R_NO_TIME)
					c = 11;
				else
					c = 0;

				req->texts1[i] = T_Print(0, lOff + req->line_height * i, c, &req->itemtexts1[(req->line_offset + i) * req->itemtextlen]);
				T_CentreH(req->texts1[i], 1);
				T_BottomAlign(req->texts1[i], 1);
			}

			if (req->noselector || req->line_offset + i != req->selected)
			{
				T_RemoveBackground(req->texts1[i]);
				T_RemoveOutline(req->texts1[i]);
			}
			else
			{
				T_AddBackground(req->texts1[i], req->pixwidth - 12, 19, 0, 0, 16, 0, req_unsel_gour1, 1);
				T_AddOutline(req->texts1[i], 1, 4, req_sel_gour2, 0);
			}

			if (flags & R_LEFTALIGN)
				ReqItemLeftalign(req, req->texts1[i]);
			else if (flags & R_RIGHTALIGN)
				ReqItemRightalign(req, req->texts1[i]);
			else
				ReqItemCentreAlign(req, req->texts1[i]);
		}
		else
		{
			T_RemovePrint(req->texts1[i]);
			T_RemoveBackground(req->texts1[i]);
			T_RemoveOutline(req->texts1[i]);
			req->texts1[i] = 0;
		}

		flags = req->itemtexts2_flags[req->line_offset + i];

		if (flags & R_USE)
		{
			if (!req->texts2[i])
			{
				req->texts2[i] = T_Print(0, lOff + req->line_height * i, 0, &req->itemtexts2[(req->line_offset + i) * req->itemtextlen]);
				T_CentreH(req->texts2[i], 1);
				T_BottomAlign(req->texts2[i], 1);
			}

			if (flags & R_LEFTALIGN)
				ReqItemLeftalign(req, req->texts2[i]);
			else if (flags & R_RIGHTALIGN)
				ReqItemRightalign(req, req->texts2[i]);
			else
				ReqItemCentreAlign(req, req->texts2[i]);
		}
		else
		{
			T_RemovePrint(req->texts2[i]);
			T_RemoveBackground(req->texts2[i]);
			T_RemoveOutline(req->texts2[i]);
			req->texts2[i] = 0;
		}
	}

	if (inputDB & IN_BACK)
	{
		if (req->noselector)
		{
			req->line_oldoffset = req->line_offset;

			if (req->item > req->vis_lines + req->line_offset)
			{
				req->line_offset++;
				SoundEffect(SFX_MENU_PASSPORT, 0, 0x4000000 | SFX_SETPITCH);
			}
		}
		else
		{
			if (req->selected < req->item - 1)
			{
				req->selected++;
				SoundEffect(SFX_MENU_PASSPORT, 0, 0x4000000 | SFX_SETPITCH);
			}

			req->line_oldoffset = req->line_offset;

			if (req->selected > req->line_offset + req->vis_lines - 1)
				req->line_offset++;
		}
	}
	else if (inputDB & IN_FORWARD)
	{
		if (req->noselector)
		{
			req->line_oldoffset = req->line_offset;

			if (req->line_offset)
			{
				req->line_offset--;
				SoundEffect(SFX_MENU_PASSPORT, 0, 0x4000000 | SFX_SETPITCH);
			}
		}
		else
		{
			if (req->selected)
			{
				req->selected--;
				SoundEffect(SFX_MENU_PASSPORT, 0, 0x4000000 | SFX_SETPITCH);
			}

			req->line_oldoffset = req->line_offset;

			if (req->selected < req->line_offset)
				req->line_offset--;
		}
	}
	else if (inputDB & IN_SELECT)
	{
		if (!strncmp(req->texts1[req->selected - req->line_offset]->string, GF_PCStrings[PCSTR_SAVESLOT], 12) &&
			!strcmp(passport_text1->string, GF_GameStrings[GT_LOADGAME]))
		{
			input = 0;
			return 0;
		}

		Remove_Requester(req);
		return req->selected + 1;
	}
	else if (inputDB & IN_DESELECT && des == 1)
	{
		Remove_Requester(req);
		return -1;
	}

	return 0;
}

void SetRequesterHeading(REQUEST_INFO* req, const char* text1, ulong flags1, const char* text2, ulong flags2)
{
	T_RemovePrint(req->heading1text);
	req->heading1text = 0;

	T_RemovePrint(req->heading2text);
	req->heading2text = 0;

	if (text1)
	{
		sprintf(req->heading1_str, text1);
		req->heading1_flags = flags1 | R_USE;
	}
	else
	{
		sprintf(req->heading1_str, "u");
		req->heading1_flags = 0;
	}

	if (text2)
	{
		sprintf(req->heading2_str, text2);
		req->heading2_flags = flags2 | R_USE;
	}
	else
	{
		sprintf(req->heading2_str, "u");
		req->heading2_flags = 0;
	}
}

void RemoveAllReqItems(REQUEST_INFO* req)
{
	req->item = 0;
	req->line_offset = 0;
	req->selected = 0;
}

void ChangeRequesterItem(REQUEST_INFO* req, long item, const char* text1, ulong flags1, const char* text2, ulong flags2)
{
	T_RemovePrint(req->texts1[item]);
	req->texts1[item] = 0;

	T_RemovePrint(req->texts2[item]);
	req->texts2[item] = 0;

	if (text1)
	{
		sprintf(&req->itemtexts1[item * req->itemtextlen], text1);
		req->itemtexts1_flags[item] = flags1 | R_USE;
	}
	else
		req->itemtexts1_flags[item] = 0;

	if (text2)
	{
		sprintf(&req->itemtexts2[item * req->itemtextlen], text2);
		req->itemtexts2_flags[item] = flags2 | R_USE;
	}
	else
		req->itemtexts2_flags[item] = 0;
}

void AddRequesterItem(REQUEST_INFO* req, const char* text1, ulong flags1, const char* text2, ulong flags2)
{
	req->itemtexts1_flags = RequesterFlags1;
	req->itemtexts2_flags = RequesterFlags2;

	if (text1)
	{
		sprintf(&req->itemtexts1[req->item * req->itemtextlen], text1);
		req->itemtexts1_flags[req->item] = flags1 | R_USE;
	}
	else
		RequesterFlags1[req->item] = 0;

	if (text2)
	{
		sprintf(&req->itemtexts2[req->item * req->itemtextlen], text2);
		req->itemtexts2_flags[req->item] = flags2 | R_USE;
	}
	else
		req->itemtexts2_flags[req->item] = 0;

	req->item++;
}

void SetPCRequesterSize(REQUEST_INFO* req, long nLines, long y)
{
	long h;

	h = GetRenderHeight() / 2 / 18;	//HMMMM

	if (h > nLines)
		h = nLines;

	req->ypos = (short)y;
	req->vis_lines = (ushort)h;
}

long AddAssaultTime(ulong time)
{
	long n, add;

	for (n = 0, add = 0; n < 10; n++)
	{
		if (!savegame.best_assault_times[n] || time < savegame.best_assault_times[n])	//we have an open slot or we got a better time
		{
			add = 1;
			break;
		}
	}

	if (add)
	{
		for (int i = 9; i > n; i--)
			savegame.best_assault_times[i] = savegame.best_assault_times[i - 1];	//move all slower times back 1

		savegame.best_assault_times[n] = time;
		return 1;
	}

	return 0;
}

long AddQuadbikeTime(ulong time)	//same as above for quad times
{
	long n, add;

	for (n = 0, add = 0; n < 10; n++)
	{
		if (!savegame.best_quadbike_times[n] || time < savegame.best_quadbike_times[n])
		{
			add = 1;
			break;
		}
	}

	if (add)
	{
		for (int i = 9; i > n; i--)
			savegame.best_quadbike_times[i] = savegame.best_quadbike_times[i - 1];	//move all slower times back 1

		savegame.best_quadbike_times[n] = time;
		return 1;
	}

	return 0;
}

#define Stats_Requester	VAR_(0x00626490, REQUEST_INFO)
#define Valid_Level_Strings	ARRAY_(0x006D62E0, char, [24][50])
#define Valid_Level_Strings2	ARRAY_(0x006D7220, char, [24][50])

void ShowGymStatsText(const char* time, long type)
{
	ulong t;
	static long mode;
	long n;
	char txt[32];

	if (mode == 1)
	{
		if (Display_Requester(&Stats_Requester, 1, 1))
			mode = 0;
		else
		{
			inputDB = 0;
			input = 0;
		}

		return;
	}

	Stats_Requester.noselector = 1;
	SetPCRequesterSize(&Stats_Requester, 7, -32);
	Stats_Requester.line_height = 18;
	Stats_Requester.item = 0;
	Stats_Requester.selected = 0;
	Stats_Requester.line_offset = 0;
	Stats_Requester.line_oldoffset = 0;
	Stats_Requester.pixwidth = 296;
	Stats_Requester.xpos = 0;
	Stats_Requester.zpos = 0;
	Stats_Requester.itemtexts1 = &Valid_Level_Strings[0][0];
	Stats_Requester.itemtexts2 = &Valid_Level_Strings2[0][0];
	Stats_Requester.itemtextlen = 50;
	Init_Requester(&Stats_Requester);
	SetRequesterHeading(&Stats_Requester, GF_GameStrings[GT_BESTTIMES], 0, 0, 0);

	for (n = -1; n < 10; n++)	//-1 to add the heading
	{
		if (n == -1)
		{
			switch (gameflow.language)
			{
			case 1:
				AddRequesterItem(&Stats_Requester, "Entra(inement", R_HEADING, 0, 0);
				break;

			case 2:
				AddRequesterItem(&Stats_Requester, "Hinderniskurs", R_HEADING, 0, 0);
				break;

			case 5:
				AddRequesterItem(&Stats_Requester, "Circuito d'addestramento", R_HEADING, 0, 0);
				break;

			case 6:
				AddRequesterItem(&Stats_Requester, "Asaltar Carrera", R_HEADING, 0, 0);
				break;

			default:
				AddRequesterItem(&Stats_Requester, "Assault Course", R_HEADING, 0, 0);
				break;
			}

			continue;
		}

		t = savegame.best_assault_times[n];

		if (!t)
			break;

		sprintf(txt, "%d:%2.2d.%02d", (t / 30) / 60, (t / 30) % 60, 334 * (t % 30) / 100);
		AddRequesterItem(&Stats_Requester, txt, n == 0 ? R_BEST_TIME : R_NORMAL_TIME, 0, 0);
	}

	if (!n)
		AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_NOTIMES], R_NO_TIME, 0, 0);

	if (savegame.QuadbikeKeyFlag)
	{
		AddRequesterItem(&Stats_Requester, " ", 0, 0, 0);

		for (n = -1; n < 10; n++)	//-1 to add the heading
		{
			if (n == -1)
			{
				switch (gameflow.language)
				{
				case 1:
					AddRequesterItem(&Stats_Requester, "Course de moto", R_HEADING, 0, 0);
					break;

				case 2:
					AddRequesterItem(&Stats_Requester, "Quadbike Teststrecke", R_HEADING, 0, 0);
					break;

				case 5:
					AddRequesterItem(&Stats_Requester, "Circuito per moto", R_HEADING, 0, 0);
					break;

				case 6:
					AddRequesterItem(&Stats_Requester, "Circuito Moto Quad", R_HEADING, 0, 0);
					break;

				default:
					AddRequesterItem(&Stats_Requester, "Quadbike Track", R_HEADING, 0, 0);
					break;
				}

				continue;
			}

			t = savegame.best_quadbike_times[n];

			if (!t)
				break;

			sprintf(txt, "%d:%2.2d.%02d", (t / 30) / 60, (t / 30) % 60, 334 * (t % 30) / 100);
			AddRequesterItem(&Stats_Requester, txt, n == 0 ? R_BEST_TIME : R_NORMAL_TIME, 0, 0);
		}
	}

	mode = 1;
}

void ShowStatsText(const char* time, long type)
{
	static long mode;
	long s;
	char txt[32];

	if (mode == 1)
	{
		ChangeRequesterItem(&Stats_Requester, 0, GF_GameStrings[GT_STAT_TIME], R_LEFTALIGN, time, R_RIGHTALIGN);

		if (Display_Requester(&Stats_Requester, type, 1))
			mode = 0;
		else
		{
			inputDB = 0;
			input = 0;
		}

		return;
	}

	Stats_Requester.noselector = 1;
	SetPCRequesterSize(&Stats_Requester, 7, -32);
	Stats_Requester.line_height = 18;
	Stats_Requester.item = 0;
	Stats_Requester.selected = 0;
	Stats_Requester.line_offset = 0;
	Stats_Requester.line_oldoffset = 0;
	Stats_Requester.pixwidth = 304;
	Stats_Requester.xpos = 0;
	Stats_Requester.zpos = 0;
	Stats_Requester.itemtexts1 = &Valid_Level_Strings[0][0];
	Stats_Requester.itemtexts2 = &Valid_Level_Strings2[0][0];
	Stats_Requester.itemtextlen = 50;
	Init_Requester(&Stats_Requester);
	SetRequesterHeading(&Stats_Requester, GF_Level_Names[CurrentLevel], 0, 0, 0);
	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_TIME], R_LEFTALIGN, time, R_RIGHTALIGN);

	if (LevelSecrets[CurrentLevel])
	{
		s = savegame.secrets & 1;
		s += (savegame.secrets >> 1) & 1;
		s += (savegame.secrets >> 2) & 1;
		s += (savegame.secrets >> 3) & 1;
		s += (savegame.secrets >> 4) & 1;
		s += (savegame.secrets >> 5) & 1;
		s += (savegame.secrets >> 6) & 1;
		s += (savegame.secrets >> 7) & 1;
		sprintf(txt, "%d %s %d", s, GF_GameStrings[GT_OF], LevelSecrets[CurrentLevel]);
	}
	else
		sprintf(txt, "%s", GF_GameStrings[GT_NONE]);

	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_SECRETS], R_LEFTALIGN, txt, R_RIGHTALIGN);

	sprintf(txt, "%d", savegame.kills);
	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_KILLS], R_LEFTALIGN, txt, R_RIGHTALIGN);

	sprintf(txt, "%d", savegame.ammo_used);
	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_AMMO], R_LEFTALIGN, txt, R_RIGHTALIGN);

	sprintf(txt, "%d", savegame.ammo_hit);
	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_RATIO], R_LEFTALIGN, txt, R_RIGHTALIGN);

	if (savegame.health_used & 1)
		sprintf(txt, "%d.5", savegame.health_used >> 1);
	else
		sprintf(txt, "%d.0", savegame.health_used >> 1);

	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_HEALTH], R_LEFTALIGN, txt, R_RIGHTALIGN);

	s = savegame.distance_travelled / 445;

	if (s < 1000)
		sprintf(txt, "%dm", s);
	else
		sprintf(txt, "%d.%02dkm", s / 1000, s % 100);

	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_DISTANCE], R_LEFTALIGN, txt, R_RIGHTALIGN);
	mode = 1;
}

void ShowEndStatsText()
{
	uchar* pS;
	static long mode;
	long totallevels, i, num, numsecrets;
	char txt[32];

	if (mode == 1)
	{
		if (Display_Requester(&Stats_Requester, 0, 1))
			mode = 0;
		else
		{
			inputDB = 0;
			input = 0;
		}

		return;
	}

	Stats_Requester.noselector = 1;
	SetPCRequesterSize(&Stats_Requester, 7, -32);
	Stats_Requester.line_height = 18;
	Stats_Requester.item = 0;
	Stats_Requester.selected = 0;
	Stats_Requester.line_offset = 0;
	Stats_Requester.line_oldoffset = 0;
	Stats_Requester.pixwidth = 304;
	Stats_Requester.xpos = 0;
	Stats_Requester.zpos = 0;
	Stats_Requester.itemtexts1 = &Valid_Level_Strings[0][0];
	Stats_Requester.itemtexts2 = &Valid_Level_Strings2[0][0];
	Stats_Requester.itemtextlen = 50;
	Init_Requester(&Stats_Requester);
	SetRequesterHeading(&Stats_Requester, GF_GameStrings[GT_GAMESTATS], 0, 0, 0);

	totallevels = gameflow.num_levels - gameflow.num_demos;

	for (num = 0, i = 1; i < totallevels; i++)	//start at 1 because we don't need Gym
		num += savegame.start[i].timer;

	sprintf(txt, "%02d:%02d:%02d", num / 30 / 3600, num / 30 / 60 % 60, num / 30 % 60);
	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_TIME], R_LEFTALIGN, txt, R_RIGHTALIGN);

	numsecrets = 0;
	pS = &savegame.start[0].secrets_found;

#ifdef RANDO_STUFF	//No bonus level secrets please
	totallevels--;
#endif

	for (num = 0, i = 0; i < totallevels; i++, pS += sizeof(START_INFO))
	{
		numsecrets += (*pS & 1) + ((*pS >> 1) & 1) + ((*pS >> 2) & 1) + ((*pS >> 3) & 1) +
			((*pS >> 4) & 1) + ((*pS >> 5) & 1) + ((*pS >> 6) & 1) + ((*pS >> 7) & 1);
		num += LevelSecrets[i];
	}

#ifdef RANDO_STUFF
	totallevels++;
#endif

	sprintf(txt, "%d %s %d", numsecrets, GF_GameStrings[GT_OF], num);
	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_SECRETS], R_LEFTALIGN, txt, R_RIGHTALIGN);

	for (num = 0, i = 1; i < totallevels; i++)
		num += savegame.start[i].kills;

	sprintf(txt, "%d", num);
	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_KILLS], R_LEFTALIGN, txt, R_RIGHTALIGN);

	for (num = 0, i = 1; i < totallevels; i++)
		num += savegame.start[i].ammo_used;

	sprintf(txt, "%d", num);
	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_AMMO], R_LEFTALIGN, txt, R_RIGHTALIGN);

	for (num = 0, i = 1; i < totallevels; i++)
		num += savegame.start[i].ammo_hit;

	sprintf(txt, "%d", num);
	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_RATIO], R_LEFTALIGN, txt, R_RIGHTALIGN);

	for (num = 0, i = 1; i < totallevels; i++)
		num += savegame.start[i].health_used;

	if (num & 1)
		sprintf(txt, "%d.5", num >> 1);
	else
		sprintf(txt, "%d.0", num >> 1);

	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_HEALTH], R_LEFTALIGN, txt, R_RIGHTALIGN);

	for (num = 0, i = 1; i < totallevels; i++)
		num += savegame.start[i].distance_travelled;

	num /= 445;

	if (num < 1000)
		sprintf(txt, "%dm", num);
	else
		sprintf(txt, "%d.%02dkm", num / 1000, num % 100);

	AddRequesterItem(&Stats_Requester, GF_GameStrings[GT_STAT_DISTANCE], R_LEFTALIGN, txt, R_RIGHTALIGN);
	mode = 1;
}

/************Ring Motion stuff************/

void Inv_RingMotionInit(RING_INFO* ring, short count, short status, short status_target)
{
	IMOTION_INFO* imo;

	imo = ring->imo;
	imo->count = count;
	imo->status_target = status_target;
	imo->status = status;
	imo->radius_target = 0;
	imo->radius_rate = 0;
	imo->camera_ytarget = 0;
	imo->camera_yrate = 0;
	imo->camera_pitch_target = 0;
	imo->camera_pitch_rate = 0;
	imo->rotate_target = 0;
	imo->rotate_rate = 0;
	imo->item_ptxrot_target = 0;
	imo->item_ptxrot_rate = 0;
	imo->item_xrot_target = 0;
	imo->item_xrot_rate = 0;
	imo->item_ytrans_target = 0;
	imo->item_ytrans_rate = 0;
	imo->item_ztrans_target = 0;
	imo->item_ztrans_rate = 0;
	imo->misc = 0;
}

void Inv_RingMotionSetup(RING_INFO* ring, short status, short status_target, short count)
{
	IMOTION_INFO* imo;

	imo = ring->imo;
	imo->count = count;
	imo->status_target = status_target;
	imo->status = status;
	imo->radius_rate = 0;
	imo->camera_yrate = 0;
}

void Inv_RingMotionRadius(RING_INFO* ring, short target)
{
	IMOTION_INFO* imo;

	imo = ring->imo;
	imo->radius_target = target;
	imo->radius_rate = (target - ring->radius) / imo->count;
}

void Inv_RingMotionRotation(RING_INFO* ring, short rotation, short target)
{
	IMOTION_INFO* imo;

	imo = ring->imo;
	imo->rotate_target = target;
	imo->rotate_rate = rotation / imo->count;
}

void Inv_RingMotionCameraPos(RING_INFO* ring, short target)
{
	IMOTION_INFO* imo;

	imo = ring->imo;
	imo->camera_ytarget = target;
	imo->camera_yrate = short((target - ring->camerapos.y_pos) / imo->count);
}

void inject_invfunc(bool replace)
{
	INJECT(0x00437050, InitColours, replace);

	INJECT(0x00439150, Init_Requester, replace);
	INJECT(0x004391E0, Remove_Requester, replace);
	INJECT(0x00439290, ReqItemCentreAlign, replace);
	INJECT(0x004392B0, ReqItemLeftalign, replace);
	INJECT(0x00439310, ReqItemRightalign, replace);
	INJECT(0x00439370, Display_Requester, replace);
	INJECT(0x00439B80, SetRequesterHeading, replace);
	INJECT(0x00439C30, RemoveAllReqItems, replace);
	INJECT(0x00439C50, ChangeRequesterItem, replace);
	INJECT(0x00439D10, AddRequesterItem, replace);
	INJECT(0x00439DC0, SetPCRequesterSize, replace);
	INJECT(0x00439E00, AddAssaultTime, replace);
	INJECT(0x00439E60, AddQuadbikeTime, replace);
	INJECT(0x00439EC0, ShowGymStatsText, replace);
	INJECT(0x0043A220, ShowStatsText, replace);
	INJECT(0x0043A5B0, ShowEndStatsText, inject_rando ? 1 : replace);

	INJECT(0x00438F30, Inv_RingMotionInit, replace);
	INJECT(0x00438FA0, Inv_RingMotionSetup, replace);
	INJECT(0x00438FD0, Inv_RingMotionRadius, replace);
	INJECT(0x00439000, Inv_RingMotionRotation, replace);
	INJECT(0x00439030, Inv_RingMotionCameraPos, replace);
}
