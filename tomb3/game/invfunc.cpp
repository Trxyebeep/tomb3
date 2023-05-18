#include "../tomb3/pch.h"
#include "invfunc.h"
#include "../specific/frontend.h"
#include "../specific/option.h"
#include "text.h"
#include "sound.h"
#include "gameflow.h"
#include "../specific/smain.h"
#include "../3dsystem/3d_gen.h"
#include "objects.h"
#include "health.h"
#include "items.h"
#include "../specific/litesrc.h"
#include "inventry.h"
#include "../specific/input.h"
#include "control.h"
#include "savegame.h"
#include "lara.h"
#include "../tomb3/tomb3.h"

GOURAUD_FILL req_main_gour1 =
{
	{
	{0x80000000, 0x80000000, 0x80108038, 0x80000000},
	{0x80000000, 0x80000000, 0x80000000, 0x80108038},
	{0x80108038, 0x80000000, 0x80000000, 0x80000000},
	{0x80000000, 0x80108038, 0x80000000, 0x80000000},
	}
};

GOURAUD_OUTLINE req_main_gour2 =
{
	0xFF000000, 0xFF000000, 0xFF000000,
	0xFF000000, 0xFF000000, 0xFF000000,
	0xFF000000, 0xFF000000, 0xFF000000,
};

GOURAUD_FILL req_bgnd_gour1 =
{ 
	{
	{0x80002000, 0x80002000, 0x80006000, 0x80002000},
	{0x80002000, 0x80002000, 0x80002000, 0x80006000},
	{0x80006000, 0x80002000, 0x80002000, 0x80002000},
	{0x80002000, 0x80006000, 0x80002000, 0x80002000},
	} 
};

GOURAUD_OUTLINE req_bgnd_gour2 =
{
	0xFF606060, 0xFF808080, 0xFF202020,
	0xFF000000, 0xFF000000, 0xFF202020,
	0xFF404040, 0xFF404040, 0xFF606060,
};

GOURAUD_FILL req_sel_gour1 =
{
	{
	{0x80000000, 0x80000000, 0x801C7840, 0x80000000},
	{0x80000000, 0x80000000, 0x80000000, 0x801C7840},
	{0x801C7840, 0x80000000, 0x80000000, 0x80000000},
	{0x80000000, 0x801C7840, 0x80000000, 0x80000000},
	}
};

GOURAUD_OUTLINE req_sel_gour2 =
{
	0xFF000000, 0xFFFFFFFF, 0xFF000000,
	0xFF38F080, 0xFF000000, 0xFFFFFFFF,
	0xFF000000, 0xFF38F080, 0xFF000000,
};

ulong inv_colours[17] =
{
	0xFF000000,
	0xFF404040,
	0xFFFFFFFF,
	0xFFFF0000,
	0xFFFF8000,
	0xFFFFFF00,

	0, 0, 0, 0, 0, 0,

	0xFF008000,
	0xFF00FF00,
	0xFF00FFFF,
	0xFF0000FF,
	0xFFFF00FF
};

short gLevelSecrets[21] = { 0, 3, 3, 3, 3, 3, 0, 3, 1, 5, 5, 6, 1, 3, 2, 3, 3, 3, 3, 0, 0 };
short LevelSecrets[21] = { 0, 6, 4, 5, 0, 3, 3, 3, 1, 5, 5, 6, 1, 3, 2, 3, 3, 3, 3, 0, 0 };

ulong RequesterFlags1[24];
ulong RequesterFlags2[24];
ulong SaveGameReqFlags1[24];
ulong SaveGameReqFlags2[24];

char Save_Game_Strings[24][50];
char Save_Game_Strings2[24][50];
char Valid_Level_Strings[24][50];
char Valid_Level_Strings2[24][50];

TEXTSTRING* Inv_itemText[3];
short inv_main_qtys[23] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
short inv_keys_qtys[23] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/*Main ring items*/
INVENTORY_ITEM icompass_option = { 0, MAP_OPTION, 1, 0, 0, 0, 1, 1, 0, 4352, 0, -1536, 0, 0, 0, 0, -170, 0, 320, 0, 0xFFFFFFFF, 0xFFFFFFFF, MAP_POS, 0, 0, 0, 0 };
INVENTORY_ITEM igun_option = { 0, GUN_OPTION, 12, 0, 0, 11, 1, 1, 0, 3200, 0, 2848, 0, 0, -32768, 0, 38, 0, 352, 0, 0xFFFFFFFF, 0xFFFFFFFF, GUN_POS, 0, 0, 0, 0 };
INVENTORY_ITEM ishotgun_option = { 0, SHOTGUN_OPTION, 13, 0, 0, 12, 1, 1, 0, 3200, 0, 5120, 0, 0, 30720, 0, 0, 0, 228, 0, 0xFFFFFFFF, 0xFFFFFFFF, SGN_POS, 0, 0, 0, 0 };
INVENTORY_ITEM imagnum_option = { 0, MAGNUM_OPTION, 12, 0, 0, 11, 1, 1, 0, 3200, 0, 3360, 0, 0, -32768, 0, 0, 0, 362, 0, 0xFFFFFFFF, 0xFFFFFFFF, MAG_POS, 0, 0, 0, 0 };
INVENTORY_ITEM iuzi_option = { 0, UZI_OPTION, 13, 0, 0, 12, 1, 1, 0, 3200, 0, 2336, 0, 0, -32768, 0, 56, 0, 322, 0, 0xFFFFFFFF, 0xFFFFFFFF, UZI_POS, 0, 0, 0, 0 };
INVENTORY_ITEM im16_option = { 0, M16_OPTION, 12, 0, 0, 11, 1, 1, 0, 3200, 0, -224, 0, 0, -18432, 0, 84, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, M16_POS, 0, 0, 0, 0 };
INVENTORY_ITEM irocket_option = { 0, ROCKET_OPTION, 12, 0, 0, 11, 1, 1, 0, 3200, 0, -224, 0, 0, 14336, 0, 56, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, ROCKET_POS, 0, 0, 0, 0 };
INVENTORY_ITEM igrenade_option = { 0, GRENADE_OPTION, 12, 0, 0, 11, 1, 1, 0, 3200, 0, -224, 0, 0, 14336, 0, 56, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, GRENADE_POS, 0, 0, 0, 0 };
INVENTORY_ITEM iharpoon_option = { 0, HARPOON_OPTION, 12, 0, 0, 11, 1, 1, 0, 3200, 0, -736, 0, 0, -19456, 0, 58, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, HARPOON_POS, 0, 0, 0, 0 };
INVENTORY_ITEM iflare_option = { 0, FLAREBOX_OPTION, 31, 0, 0, 30, 1, 1, 0, 3200, 0, 0, 0, 0, -8192, 0, 0, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, FLR_POS, 0, 0, 0, 0 };

INVENTORY_ITEM igunammo_option = { 0, GUN_AMMO_OPTION, 1, 0, 0, 0, 1, 1, 0, 3200, 0, -3808, 0, 0, 0, 0, 0, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, GUN_POS, 0, 0, 0, 0 };
INVENTORY_ITEM isgunammo_option = { 0, SG_AMMO_OPTION, 1, 0, 0, 0, 1, 1, 0, 3200, 0, -3808, 0, 0, 0, 0, 0, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, SGN_POS, 0, 0, 0, 0 };
INVENTORY_ITEM imagammo_option = { 0, MAG_AMMO_OPTION, 1, 0, 0, 0, 1, 1, 0, 3200, 0, -3808, 0, 0, 0, 0, 0, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, MAG_POS, 0, 0, 0, 0 };
INVENTORY_ITEM iuziammo_option = { 0, UZI_AMMO_OPTION, 1, 0, 0, 0, 1, 1, 0, 3200, 0, -3808, 0, 0, 0, 0, 0, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, UZI_POS, 0, 0, 0, 0 };
INVENTORY_ITEM im16ammo_option = { 0, M16_AMMO_OPTION, 1, 0, 0, 0, 1, 1, 0, 3200, 0, -3808, 0, 0, 0, 0, 0, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, M16_POS, 0, 0, 0, 0 };
INVENTORY_ITEM irocketammo_option = { 0, ROCKET_AMMO_OPTION, 1, 0, 0, 0, 1, 1, 0, 3200, 0, -3808, 0, 0, 0, 0, 0, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, ROCKET_POS, 0, 0, 0, 0 };
INVENTORY_ITEM igrenadeammo_option = { 0, GRENADE_AMMO_OPTION, 1, 0, 0, 0, 1, 1, 0, 3200, 0, -3808, 0, 0, 0, 0, 0, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, GRENADE_POS, 0, 0, 0, 0 };
INVENTORY_ITEM iharpoonammo_option = { 0, HARPOON_AMMO_OPTION, 1, 0, 0, 0, 1, 1, 0, 3200, 0, -3808, 0, 0, 0, 0, 0, 0, 296, 0, 0xFFFFFFFF, 0xFFFFFFFF, HARPOON_POS, 0, 0, 0, 0 };

INVENTORY_ITEM ibigmedi_option = { 0, BIGMEDI_OPTION, 20, 0, 0, 19, 1, 1, 0, 3616, 0, -8160, 0, 0, -4096, 0, 0, 0, 352, 0, 0xFFFFFFFF, 0xFFFFFFFF, BGM_POS, 0, 0, 0, 0 };
INVENTORY_ITEM imedi_option = { 0, MEDI_OPTION, 26, 0, 0, 25, 1, 1, 0, 4032, 0, -7296, 0, 0, -4096, 0, 0, 0, 216, 0, 0xFFFFFFFF, 0xFFFFFFFF, MED_POS, 0, 0, 0, 0 };
INVENTORY_ITEM sgcrystal_option = { 0, SAVEGAME_CRYSTAL_OPTION, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, SGC_POS, 0, 0, 0, 0 };


/*Options ring items*/
INVENTORY_ITEM ipassport_option = { 0, PASSPORT_CLOSED, 30, 0, 0, 14, 1, 1, 0, 4640, 0, -4320, 0, 0, 0, 0, 0, 0, 384, 0, 0x11, 0x11, PAS_POS, 0, 0, 0, 0 };
INVENTORY_ITEM idetail_option = { 0, DETAIL_OPTION, 1, 0, 0, 0, 1, 1, 0, 4224, 0, -7232, 0, 0, 0, 0, -16, 0, 444, 0, 0xFFFFFFFF, 0xFFFFFFFF, DTL_POS, 0, 0, 0, 0 };
INVENTORY_ITEM isound_option = { 0, SOUND_OPTION, 1, 0, 0, 0, 1, 1, 0, 4832, 0, -5408, 0, 0, -3072, 0, -18, 0, 350, 0, 0xFFFFFFFF, 0xFFFFFFFF, SND_POS, 0, 0, 0, 0 };
INVENTORY_ITEM icontrol_option = { 0, CONTROL_OPTION, 1, 0, 0, 0, 1, 1, 0, 5504, 0, -2560, 5632, 5632, 13312, 0, 46, 0, 508, 0, 0xFFFFFFFF, 0xFFFFFFFF, CNT_POS, 0, 0, 0, 0 };
INVENTORY_ITEM igamma_option = { 0, GAMMA_OPTION, 160, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, -176, 0, 256, 0, 0xFFFFFFF7, 0xFFFFFFF7, GAM_POS, 0, 0, 0, 0 };
INVENTORY_ITEM iphoto_option = { 0, PHOTO_OPTION, 1, 0, 0, 0, 1, 1, 0, 4640, 0, -4320, 0, 0, 0, 0, 16, 0, 448, 0, 0xFFFFFFFF, 0xFFFFFFFF, PIC_POS, 0, 0, 0, 0 };


/*Keys ring items*/
INVENTORY_ITEM ikey1_option = { 0, KEY_OPTION1, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, KY1_POS, 0, 0, 0, 0 };
INVENTORY_ITEM ikey2_option = { 0, KEY_OPTION2, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, KY2_POS, 0, 0, 0, 0 };
INVENTORY_ITEM ikey3_option = { 0, KEY_OPTION3, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, KY3_POS, 0, 0, 0, 0 };
INVENTORY_ITEM ikey4_option = { 0, KEY_OPTION4, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, KY4_POS, 0, 0, 0, 0 };
INVENTORY_ITEM ipuzzle4_option = { 0, PUZZLE_OPTION4, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, PZ4_POS, 0, 0, 0, 0 };
INVENTORY_ITEM ipuzzle3_option = { 0, PUZZLE_OPTION3, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, PZ3_POS, 0, 0, 0, 0 };
INVENTORY_ITEM ipuzzle2_option = { 0, PUZZLE_OPTION2, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, PZ2_POS, 0, 0, 0, 0 };
INVENTORY_ITEM ipuzzle1_option = { 0, PUZZLE_OPTION1, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, PZ1_POS, 0, 0, 0, 0 };

INVENTORY_ITEM ipickup2_option = { 0, PICKUP_OPTION2, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, PK2_POS, 0, 0, 0, 0 };
INVENTORY_ITEM ipickup1_option = { 0, PICKUP_OPTION1, 1, 0, 0, 0, 1, 1, 0, 7200, 0, 7200, 0, 0, -32768, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, PK1_POS, 0, 0, 0, 0 };

INVENTORY_ITEM icon1_option = { 0, ICON_PICKUP1_OPTION, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, IC1_POS, 0, 0, 0, 0 };
INVENTORY_ITEM icon2_option = { 0, ICON_PICKUP2_OPTION, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, IC2_POS, 0, 0, 0, 0 };
INVENTORY_ITEM icon3_option = { 0, ICON_PICKUP3_OPTION, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, IC3_POS, 0, 0, 0, 0 };
INVENTORY_ITEM icon4_option = { 0, ICON_PICKUP4_OPTION, 1, 0, 0, 0, 1, 1, 0, 7200, 0, -4352, 0, 0, 0, 0, 0, 0, 256, 0, 0xFFFFFFFF, 0xFFFFFFFF, IC4_POS, 0, 0, 0, 0 };


/*Requesters*/
REQUEST_INFO Load_Game_Requester = { 0, 1, 0, 5, 0, 0, 296, 18, 0, -32, 0, 50, Save_Game_Strings[0], Save_Game_Strings2[0] };
REQUEST_INFO Level_Select_Requester = { 0, 1, 0, 5, 0, 0, 272, 18, 0, -32, 0, 50, Valid_Level_Strings[0], Valid_Level_Strings2[0] };
static REQUEST_INFO Stats_Requester;

INVENTORY_ITEM* inv_main_list[23] =
{
	&icompass_option,
	&iflare_option,
	&igun_option,
	&ishotgun_option,
	&imagnum_option,
	&iuzi_option,
	&im16_option,
	&irocket_option,
	&igrenade_option,
	&iharpoon_option,
	&ibigmedi_option,
	&imedi_option,
	&sgcrystal_option,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

INVENTORY_ITEM* inv_keys_list[23] =
{
	&ipuzzle1_option,
	&ipuzzle2_option,
	&ipuzzle3_option,
	&ipuzzle4_option,
	&ikey1_option,
	&ikey2_option,
	&ikey3_option,
	&ikey4_option,
	&ipickup1_option,
	&ipickup2_option,
	&icon1_option,
	&icon2_option,
	&icon3_option,
	&icon4_option,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};

INVENTORY_ITEM* inv_option_list[5] =
{
	&ipassport_option,
	&icontrol_option,
	&isound_option,
	&idetail_option,
	&iphoto_option
};

INVENTORY_ITEM* inv_levelselect_list[1] =
{
	&igamma_option
};

/************Requester stuff************/

#define STATS_LN_COUNT	7
#define STATS_Y_POS		-44
#define REQ_LN_HEIGHT	18

bool noAdditiveBG;

#define bgFlag	noAdditiveBG ? 0 : 1

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
	req->original_render_width = GetRenderWidthDownscaled();
	req->original_render_height = GetRenderHeightDownscaled();
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
	short bgndOffX;

	h = GetTextScaleH(txt->scaleH);
	bgndOffX = short((req->pixwidth - T_GetTextWidth(txt)) / 2 - 8);

	if (txt)
	{
		txt->bgndOffX = bgndOffX;
		txt->xpos = req->xpos - txt->bgndOffX;
	}
}

void ReqItemRightalign(REQUEST_INFO* req, TEXTSTRING* txt)
{
	ulong h;
	short bgndOffX;

	h = GetTextScaleH(txt->scaleH);
	bgndOffX = short((req->pixwidth - T_GetTextWidth(txt)) / 2 - 8);

	if (txt)
	{
		txt->bgndOffX = -bgndOffX;
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
	rw = GetRenderWidthDownscaled();
	rh = GetRenderHeightDownscaled();

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
				T_AddBackground(req->heading1text, req->pixwidth - 4, 0, 0, 0, 8, 0, &req_main_gour1, bgFlag);
				T_AddOutline(req->heading1text, 1, 4, &req_main_gour2, 0);
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
				T_AddBackground(req->heading2text, req->pixwidth - 4, 0, 0, 0, 8, 0, &req_main_gour1, bgFlag);
				T_AddOutline(req->heading2text, 1, 4, &req_main_gour2, 0);
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
		T_AddBackground(req->backgroundtext, req->pixwidth, short(req->line_height + lHeight + 12), 0, 0, 48, 0, &req_bgnd_gour1, bgFlag);
		T_AddOutline(req->backgroundtext, 1, 15, &req_bgnd_gour2, 0);
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
				T_AddBackground(req->texts1[i], req->pixwidth - 12, 19, 0, 0, 16, 0, &req_sel_gour1, bgFlag);
				T_AddOutline(req->texts1[i], 1, 4, &req_sel_gour2, 0);
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
				SoundEffect(SFX_MENU_PASSPORT, 0, 0x4000000 | SFX_SETPITCH | SFX_ALWAYS);
			}
		}
		else
		{
			if (req->selected < req->item - 1)
			{
				req->selected++;
				SoundEffect(SFX_MENU_PASSPORT, 0, 0x4000000 | SFX_SETPITCH | SFX_ALWAYS);
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
				SoundEffect(SFX_MENU_PASSPORT, 0, 0x4000000 | SFX_SETPITCH | SFX_ALWAYS);
			}
		}
		else
		{
			if (req->selected)
			{
				req->selected--;
				SoundEffect(SFX_MENU_PASSPORT, 0, 0x4000000 | SFX_SETPITCH | SFX_ALWAYS);
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

	h = GetRenderHeightDownscaled() / 2 / REQ_LN_HEIGHT;

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

void ShowGymStatsText(const char* time, long type)
{
	ulong t;
	static long mode;
	long n;
	char txt[32];

	if (mode == 1)
	{
		noAdditiveBG = 1;

		if (Display_Requester(&Stats_Requester, 1, 1))
			mode = 0;
		else
		{
			inputDB = 0;
			input = 0;
		}

		noAdditiveBG = 0;
		return;
	}

	Stats_Requester.noselector = 1;
	SetPCRequesterSize(&Stats_Requester, STATS_LN_COUNT, STATS_Y_POS);
	Stats_Requester.line_height = REQ_LN_HEIGHT;
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
	SetPCRequesterSize(&Stats_Requester, STATS_LN_COUNT, STATS_Y_POS);
	Stats_Requester.line_height = REQ_LN_HEIGHT;
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
		noAdditiveBG = 1;

		if (Display_Requester(&Stats_Requester, 0, 1))
			mode = 0;
		else
		{
			inputDB = 0;
			input = 0;
		}

		noAdditiveBG = 0;
		return;
	}

	Stats_Requester.noselector = 1;
	SetPCRequesterSize(&Stats_Requester, STATS_LN_COUNT, STATS_Y_POS);
	Stats_Requester.line_height = REQ_LN_HEIGHT;
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

	for (num = 0, i = 0; i < totallevels; i++, pS += sizeof(START_INFO))
	{
		numsecrets += (*pS & 1) + ((*pS >> 1) & 1) + ((*pS >> 2) & 1) + ((*pS >> 3) & 1) +
			((*pS >> 4) & 1) + ((*pS >> 5) & 1) + ((*pS >> 6) & 1) + ((*pS >> 7) & 1);
		num += LevelSecrets[i];
	}

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

void Inv_RingMotionCameraPitch(RING_INFO* ring, short target)
{
	IMOTION_INFO* imo;

	imo = ring->imo;
	imo->camera_pitch_target = target;
	imo->camera_pitch_rate = target / imo->count;
}

void Inv_RingMotionItemSelect(RING_INFO* ring, INVENTORY_ITEM* inv_item)
{
	IMOTION_INFO* imo;

	imo = ring->imo;

	imo->item_ptxrot_target = inv_item->pt_xrot_sel;
	imo->item_ptxrot_rate = inv_item->pt_xrot_sel / imo->count;

	imo->item_xrot_target = inv_item->x_rot_sel;
	imo->item_xrot_rate = (inv_item->x_rot_sel - inv_item->x_rot_nosel) / imo->count;

	imo->item_ytrans_target = inv_item->ytrans_sel;
	imo->item_ytrans_rate = inv_item->ytrans_sel / imo->count;

	imo->item_ztrans_target = inv_item->ztrans_sel;
	imo->item_ztrans_rate = inv_item->ztrans_sel / imo->count;
}

void Inv_RingMotionItemDeselect(RING_INFO* ring, INVENTORY_ITEM* inv_item)
{
	IMOTION_INFO* imo;

	imo = ring->imo;

	imo->item_ptxrot_target = 0;
	imo->item_ptxrot_rate = -(inv_item->pt_xrot_sel / imo->count);

	imo->item_xrot_target = inv_item->x_rot_nosel;
	imo->item_xrot_rate = (inv_item->x_rot_nosel - inv_item->x_rot_sel) / imo->count;

	imo->item_ytrans_target = 0;
	imo->item_ytrans_rate = -(inv_item->ytrans_sel / imo->count);

	imo->item_ztrans_target = 0;
	imo->item_ztrans_rate = -(inv_item->ztrans_sel / imo->count);
}

void Inv_RingDoMotions(RING_INFO* ring)
{
	IMOTION_INFO* imo;
	INVENTORY_ITEM* item;

	imo = ring->imo;

	if (imo->count)
	{
		ring->radius += imo->radius_rate;
		ring->camerapos.y_pos += imo->camera_yrate;
		ring->ringpos.y_rot += imo->rotate_rate;
		ring->camera_pitch += imo->camera_pitch_rate;

		item = ring->list[ring->current_object];
		item->pt_xrot += imo->item_ptxrot_rate;
		item->x_rot += imo->item_xrot_rate;
		item->ytrans += imo->item_ytrans_rate;
		item->ztrans += imo->item_ztrans_rate;

		imo->count--;

		if (!imo->count)
		{
			imo->status = imo->status_target;

			if (imo->radius_rate)
			{
				imo->radius_rate = 0;
				ring->radius = imo->radius_target;
			}

			if (imo->camera_yrate)
			{
				imo->camera_yrate = 0;
				ring->camerapos.y_pos = imo->camera_ytarget;
			}

			if (imo->rotate_rate)
			{
				imo->rotate_rate = 0;
				ring->ringpos.y_rot = imo->rotate_target;
			}

			if (imo->item_ptxrot_rate)
			{
				imo->item_ptxrot_rate = 0;
				item->pt_xrot = imo->item_ptxrot_target;
			}

			if (imo->item_xrot_rate)
			{
				imo->item_xrot_rate = 0;
				item->x_rot = imo->item_xrot_target;
			}

			if (imo->item_ytrans_rate)
			{
				imo->item_ytrans_rate = 0;
				item->ytrans = imo->item_ytrans_target;
			}

			if (imo->item_ztrans_rate)
			{
				imo->item_ztrans_rate = 0;
				item->ztrans = imo->item_ztrans_target;
			}

			if (imo->camera_pitch_rate)
			{
				imo->camera_pitch_rate = 0;
				ring->camera_pitch = imo->camera_pitch_target;
			}
		}
	}

	if (ring->rotating)
	{
		ring->ringpos.y_rot += ring->rot_adder;
		ring->rot_count--;

		if (!ring->rot_count)
		{
			ring->current_object = ring->target_object;
			ring->ringpos.y_rot = -0x4000 - ring->current_object * ring->angle_adder;
			ring->rotating = 0;
		}
	}
}

/************Ring stuff************/

void Inv_RingInit(RING_INFO* ring, short type, INVENTORY_ITEM** list, short qty, short current, IMOTION_INFO* imo)
{
	ring->type = type;
	ring->list = list;
	ring->number_of_objects = qty;
	ring->current_object = current;
	ring->radius = 0;
	ring->angle_adder = 0x10000 / qty;

	if (Inventory_Mode == INV_TITLE_MODE)
		ring->camera_pitch = 1024;
	else
		ring->camera_pitch = 0;

	ring->rotating = 0;
	ring->rot_count = 0;
	ring->target_object = 0;
	ring->rot_adder = 0;
	ring->rot_adderL = 0;
	ring->rot_adderR = 0;
	ring->imo = imo;
	ring->camerapos.x_pos = 0;
	ring->camerapos.y_pos = -1536;
	ring->camerapos.z_pos = 896;
	ring->camerapos.x_rot = 0;
	ring->camerapos.y_rot = 0;
	ring->camerapos.z_rot = 0;
	Inv_RingMotionInit(ring, 32, 0, 1);
	Inv_RingMotionRadius(ring, 688);
	Inv_RingMotionCameraPos(ring, -256);
	Inv_RingMotionRotation(ring, -0x8000, -0x4000 - ring->current_object * ring->angle_adder);
	ring->ringpos.x_pos = 0;
	ring->ringpos.y_pos = 0;
	ring->ringpos.z_pos = 0;
	ring->ringpos.x_rot = 0;
	ring->ringpos.y_rot = imo->rotate_target + 0x8000;
	ring->ringpos.z_rot = 0;
	ring->light.x = -1536;
	ring->light.y = 256;
	ring->light.z = 1024;
}

void Inv_RingGetView(RING_INFO* ring, PHD_3DPOS* viewer)
{
	short angles[2];

	phd_GetVectorAngles(-ring->camerapos.x_pos, -96 - ring->camerapos.y_pos, ring->radius - ring->camerapos.z_pos, angles);
	viewer->x_pos = ring->camerapos.x_pos;
	viewer->y_pos = ring->camerapos.y_pos;
	viewer->z_pos = ring->camerapos.z_pos;
	viewer->x_rot = angles[1] + ring->camera_pitch;
	viewer->y_rot = angles[0];
	viewer->z_rot = 0;
}

void Inv_RingLight(RING_INFO* ring, short object_number)
{
	long x, y, z;

	LightCol[M00] = 3312;
	LightCol[M10] = 1664;	//sun
	LightCol[M20] = 0;

	LightCol[M01] = 3312;
	LightCol[M11] = 3312;	//spot
	LightCol[M21] = 3312;

	LightCol[M02] = 0;
	LightCol[M12] = 0;		//dynamic
	LightCol[M22] = 3072;

	smcr = 32;
	smcg = 32;
	smcb = 32;

#if (DIRECT3D_VERSION >= 0x900)
	if (tomb3.psx_contrast)
	{
		for (int i = 0; i < indices_count; i++)
			LightCol[i] >>= 1;

		smcr >>= 1;
		smcg >>= 1;
		smcb >>= 1;
	}
#endif

	x = 0x4000;
	y = -0x4000;
	z = 0x3000;
	LPos[0].x = (x * w2v_matrix[M00] + y * w2v_matrix[M01] + z * w2v_matrix[M02]) >> W2V_SHIFT;
	LPos[0].y = (x * w2v_matrix[M10] + y * w2v_matrix[M11] + z * w2v_matrix[M12]) >> W2V_SHIFT;
	LPos[0].z = (x * w2v_matrix[M20] + y * w2v_matrix[M21] + z * w2v_matrix[M22]) >> W2V_SHIFT;

	x = -0x4000;
	y = -0x4000;
	z = 0x3000;
	LPos[1].x = (x * w2v_matrix[M00] + y * w2v_matrix[M01] + z * w2v_matrix[M02]) >> W2V_SHIFT;
	LPos[1].y = (x * w2v_matrix[M10] + y * w2v_matrix[M11] + z * w2v_matrix[M12]) >> W2V_SHIFT;
	LPos[1].z = (x * w2v_matrix[M20] + y * w2v_matrix[M21] + z * w2v_matrix[M22]) >> W2V_SHIFT;

	x = 0;
	y = 0x2000;
	z = 0x3000;
	LPos[2].x = (x * w2v_matrix[M00] + y * w2v_matrix[M01] + z * w2v_matrix[M02]) >> W2V_SHIFT;
	LPos[2].y = (x * w2v_matrix[M10] + y * w2v_matrix[M11] + z * w2v_matrix[M12]) >> W2V_SHIFT;
	LPos[2].z = (x * w2v_matrix[M20] + y * w2v_matrix[M21] + z * w2v_matrix[M22]) >> W2V_SHIFT;
}

void Inv_GlobeLight(short mesh_number)
{
	long x, y, z;

	if (mesh_number == 1)
	{
		LightCol[M00] = 0;
		LightCol[M10] = 256;	//sun
		LightCol[M20] = 3840;

		LightCol[M01] = 0;
		LightCol[M11] = 256;	//spot
		LightCol[M21] = 3840;

		LightCol[M02] = 0;
		LightCol[M12] = 256;	//dynamic
		LightCol[M22] = 3840;
	}
	else if (mesh_number & 0x7E)	//the location "dots"
	{
		if (mesh_number == 2 && savegame.LondonComplete ||		//London
			mesh_number == 4 && savegame.IndiaComplete ||		//India
			mesh_number == 8 && savegame.PeruComplete ||		//ok
			mesh_number == 16 && savegame.NevadaComplete ||		//Nevada
			mesh_number == 32 && savegame.SPacificComplete ||	//South Pacific
			mesh_number == 64 && savegame.AntarcticaComplete)	//Antarctica
		{
			LightCol[M00] = rcossin_tbl[GlobePointLight << 5];
			LightCol[M10] = 0;
			LightCol[M20] = 0;

			LightCol[M01] = rcossin_tbl[GlobePointLight << 5];
			LightCol[M11] = 0;
			LightCol[M21] = 0;

			LightCol[M02] = rcossin_tbl[GlobePointLight << 5];
			LightCol[M12] = 0;
			LightCol[M22] = 0;
		}
		else
		{
			LightCol[M00] = 0;
			LightCol[M10] = rcossin_tbl[GlobePointLight << 5];
			LightCol[M20] = 0;

			LightCol[M01] = 0;
			LightCol[M11] = rcossin_tbl[GlobePointLight << 5];
			LightCol[M21] = 0;

			LightCol[M02] = 0;
			LightCol[M12] = rcossin_tbl[GlobePointLight << 5];
			LightCol[M22] = 0;
		}
	}
	else
	{
		LightCol[M00] = 256;
		LightCol[M10] = 1024;
		LightCol[M20] = 256;

		LightCol[M01] = 256;
		LightCol[M11] = 1024;
		LightCol[M21] = 256;

		LightCol[M02] = 256;
		LightCol[M12] = 1024;
		LightCol[M22] = 256;
	}

	smcr = 32;
	smcg = 32;
	smcb = 32;

	x = 0x1000;
	y = -0x1000;	//sun pos
	z = 0xC00;
	LPos[0].x = (x * w2v_matrix[M00] + y * w2v_matrix[M01] + z * w2v_matrix[M02]) >> W2V_SHIFT;
	LPos[0].y = (x * w2v_matrix[M10] + y * w2v_matrix[M11] + z * w2v_matrix[M12]) >> W2V_SHIFT;
	LPos[0].z = (x * w2v_matrix[M20] + y * w2v_matrix[M21] + z * w2v_matrix[M22]) >> W2V_SHIFT;

	x = -0x1000;
	y = -0x1000;	//spot pos
	z = 0xC00;
	LPos[1].x = (x * w2v_matrix[M00] + y * w2v_matrix[M01] + z * w2v_matrix[M02]) >> W2V_SHIFT;
	LPos[1].y = (x * w2v_matrix[M10] + y * w2v_matrix[M11] + z * w2v_matrix[M12]) >> W2V_SHIFT;
	LPos[1].z = (x * w2v_matrix[M20] + y * w2v_matrix[M21] + z * w2v_matrix[M22]) >> W2V_SHIFT;

	x = 0;
	y = 0x800;		//dynamic pos
	z = 0xC00;
	LPos[2].x = (x * w2v_matrix[M00] + y * w2v_matrix[M01] + z * w2v_matrix[M02]) >> W2V_SHIFT;
	LPos[2].y = (x * w2v_matrix[M10] + y * w2v_matrix[M11] + z * w2v_matrix[M12]) >> W2V_SHIFT;
	LPos[2].z = (x * w2v_matrix[M20] + y * w2v_matrix[M21] + z * w2v_matrix[M22]) >> W2V_SHIFT;
}

void Inv_RingCalcAdders(RING_INFO* ring, short duration)
{
	ring->angle_adder = 0x10000 / ring->number_of_objects;
	ring->rot_adderL = ring->angle_adder / duration;
	ring->rot_adderR = -(ring->angle_adder / duration);
}

void Inv_RingRotateLeft(RING_INFO* ring)
{
	ring->rotating = 1;
	ring->target_object = ring->current_object - 1;

	if (ring->target_object < 0)
		ring->target_object = ring->number_of_objects - 1;

	ring->rot_count = 24;
	ring->rot_adder = ring->rot_adderL;
}

void Inv_RingRotateRight(RING_INFO* ring)
{
	ring->rotating = 1;
	ring->target_object = ring->current_object + 1;

	if (ring->target_object >= ring->number_of_objects)
		ring->target_object = 0;

	ring->rot_count = 24;
	ring->rot_adder = ring->rot_adderR;
}

void RingIsOpen(RING_INFO* ring)
{
	if (Inventory_Mode == INV_TITLE_MODE)
		return;

	if (!Inv_ringText)
	{
		switch (ring->type)
		{
		case 0:
			Inv_ringText = T_Print(0, 26, 5, GF_GameStrings[GT_MAIN_HEADING]);
			break;

		case 1:

			if (Inventory_Mode == INV_DEATH_MODE)
				Inv_ringText = T_Print(0, 26, 5, GF_GameStrings[GT_GAMEOVER_HEADING]);
			else
				Inv_ringText = T_Print(0, 26, 5, GF_GameStrings[GT_OPTION_HEADING]);

			break;

		case 2:
			Inv_ringText = T_Print(0, 26, 5, GF_GameStrings[GT_KEYS_HEADING]);
			break;

		case 3:
			Inv_ringText = T_Print(0, 26, 5, GF_GameStrings[GT_LEVELSELECT]);
			break;
		}

		T_CentreH(Inv_ringText, 1);
	}

	if (Inventory_Mode == INV_KEYS_MODE || Inventory_Mode == INV_DEATH_MODE)
		return;

	if (!Inv_upArrow1)
	{
		if (ring->type == 1 || !ring->type && inv_keys_objects)
		{
			Inv_upArrow1 = T_Print(20, 28, 2, "[");
			Inv_upArrow2 = T_Print(-20, 28, 2, "[");
			T_RightAlign(Inv_upArrow2, 1);
		}
	}

	if (!Inv_downArrow1)
	{
		if (!ring->type && !gameflow.lockout_optionring || ring->type == 2)
		{
			Inv_downArrow1 = T_Print(20, -15, 2, "]");
			T_BottomAlign(Inv_downArrow1, 1);

			Inv_downArrow2 = T_Print(-20, -15, 2, "]");
			T_BottomAlign(Inv_downArrow2, 1);
			T_RightAlign(Inv_downArrow2, 1);
		}
	}
}

void RingIsNotOpen(RING_INFO* ring)
{
	T_RemovePrint(Inv_tagText);
	Inv_tagText = 0;

	T_RemovePrint(Inv_ringText);
	Inv_ringText = 0;

	T_RemovePrint(Inv_upArrow1);
	Inv_upArrow1 = 0;

	T_RemovePrint(Inv_upArrow2);
	Inv_upArrow2 = 0;

	T_RemovePrint(Inv_downArrow1);
	Inv_downArrow1 = 0;

	T_RemovePrint(Inv_downArrow2);
	Inv_downArrow2 = 0;
}

void RingActive()
{
	T_RemovePrint(Inv_itemText[0]);
	Inv_itemText[0] = 0;

	T_RemovePrint(Inv_itemText[1]);
	Inv_itemText[1] = 0;
}

void RingNotActive(INVENTORY_ITEM* inv_item)
{
	long qty;
	char txt[64];

	if (!Inv_itemText[0])
	{
		switch (inv_item->object_number)
		{
		case PASSPORT_OPTION:
		case GAMMA_OPTION:
			break;

		case PUZZLE_OPTION1:
			Inv_itemText[0] = T_Print(0, -16, 0, GF_Puzzle1Strings[CurrentLevel]);
			break;

		case PUZZLE_OPTION2:
			Inv_itemText[0] = T_Print(0, -16, 0, GF_Puzzle2Strings[CurrentLevel]);
			break;

		case PUZZLE_OPTION3:
			Inv_itemText[0] = T_Print(0, -16, 0, GF_Puzzle3Strings[CurrentLevel]);
			break;

		case PUZZLE_OPTION4:
			Inv_itemText[0] = T_Print(0, -16, 0, GF_Puzzle4Strings[CurrentLevel]);
			break;

		case KEY_OPTION1:
			Inv_itemText[0] = T_Print(0, -16, 0, GF_Key1Strings[CurrentLevel]);
			break;

		case KEY_OPTION2:
			Inv_itemText[0] = T_Print(0, -16, 0, GF_Key2Strings[CurrentLevel]);
			break;

		case KEY_OPTION3:
			Inv_itemText[0] = T_Print(0, -16, 0, GF_Key3Strings[CurrentLevel]);
			break;

		case KEY_OPTION4:
			Inv_itemText[0] = T_Print(0, -16, 0, GF_Key4Strings[CurrentLevel]);
			break;

		case PICKUP_OPTION1:
			Inv_itemText[0] = T_Print(0, -16, 0, GF_Pickup1Strings[CurrentLevel]);
			break;

		case PICKUP_OPTION2:
			Inv_itemText[0] = T_Print(0, -16, 0, GF_Pickup2Strings[CurrentLevel]);
			break;

		default:
			Inv_itemText[0] = T_Print(0, -16, 5, inv_item->itemText);
			break;
		}

		if (Inv_itemText[0])
		{
			T_BottomAlign(Inv_itemText[0], 1);
			T_CentreH(Inv_itemText[0], 1);
		}
	}

	qty = Inv_RequestItem(inv_item->object_number);

	switch (inv_item->object_number)
	{
	case SHOTGUN_OPTION:

		if (Inv_itemText[1] || savegame.bonus_flag)
			return;

		wsprintf(txt, "%5d", lara.shotgun.ammo / 6);
		break;

	case MAGNUM_OPTION:

		if (Inv_itemText[1] || savegame.bonus_flag)
			return;

		wsprintf(txt, "%5d", lara.magnums.ammo);
		break;

	case UZI_OPTION:

		if (Inv_itemText[1] || savegame.bonus_flag)
			return;

		wsprintf(txt, "%5d", lara.uzis.ammo);
		break;

	case HARPOON_OPTION:

		if (Inv_itemText[1] || savegame.bonus_flag)
			return;

		wsprintf(txt, "%5d", lara.harpoon.ammo);
		break;

	case M16_OPTION:

		if (Inv_itemText[1] || savegame.bonus_flag)
			return;

		wsprintf(txt, "%5d", lara.m16.ammo);
		break;

	case ROCKET_OPTION:

		if (Inv_itemText[1] || savegame.bonus_flag)
			return;

		wsprintf(txt, "%5d", lara.rocket.ammo);
		break;

	case GRENADE_OPTION:

		if (Inv_itemText[1] || savegame.bonus_flag)
			return;

		wsprintf(txt, "%5d", lara.grenade.ammo);
		break;

	case SG_AMMO_OPTION:

		if (Inv_itemText[1])
			return;

		wsprintf(txt, "%5d", qty * 2);
		break;

	case MAG_AMMO_OPTION:
	case UZI_AMMO_OPTION:
	case M16_AMMO_OPTION:

		if (Inv_itemText[1] || savegame.bonus_flag)
			return;

		wsprintf(txt, "%d", qty * 2);
		break;

	case HARPOON_AMMO_OPTION:

		if (Inv_itemText[1] || savegame.bonus_flag)
			return;

		wsprintf(txt, "%d", lara.harpoon.ammo);
		break;

	case ROCKET_AMMO_OPTION:
	case GRENADE_AMMO_OPTION:
	case FLAREBOX_OPTION:

		if (Inv_itemText[1] || savegame.bonus_flag)
			return;

		wsprintf(txt, "%d", qty);
		break;

	case MEDI_OPTION:
	case BIGMEDI_OPTION:
		health_bar_timer = 40;
		DrawHealthBar(FlashIt());
		//fallthrough

	case SAVEGAME_CRYSTAL_OPTION:
	case PUZZLE_OPTION1:
	case PUZZLE_OPTION2:
	case PUZZLE_OPTION3:
	case PUZZLE_OPTION4:
	case KEY_OPTION1:
	case KEY_OPTION2:
	case KEY_OPTION3:
	case KEY_OPTION4:
	case PICKUP_OPTION1:
	case PICKUP_OPTION2:
	case ICON_PICKUP1_OPTION:
	case ICON_PICKUP2_OPTION:
	case ICON_PICKUP3_OPTION:
	case ICON_PICKUP4_OPTION:

		if (Inv_itemText[1] || qty <= 1)
			return;

		wsprintf(txt, "%d", qty);
		break;

	default:
		return;
	}

	if (!Inv_itemText[1])
	{
		Inv_itemText[1] = T_Print(64, -56, 3, txt);
		T_BottomAlign(Inv_itemText[1], 1);
		T_CentreH(Inv_itemText[1], 1);
	}
}

/************Item stuff************/

long Inv_GetItemOption(long item_number)
{
	switch (item_number)
	{
	case GUN_ITEM:
	case GUN_OPTION:
		return GUN_OPTION;
		
	case SHOTGUN_ITEM:
	case SHOTGUN_OPTION:
		return SHOTGUN_OPTION;
		
	case MAGNUM_ITEM:
	case MAGNUM_OPTION:
		return MAGNUM_OPTION;
		
	case UZI_ITEM:
	case UZI_OPTION:
		return UZI_OPTION;
		
	case HARPOON_ITEM:
	case HARPOON_OPTION:
		return HARPOON_OPTION;
		
	case M16_ITEM:
	case M16_OPTION:
		return M16_OPTION;
		
	case ROCKET_GUN_ITEM:
	case ROCKET_OPTION:
		return ROCKET_OPTION;
		
	case GRENADE_GUN_ITEM:
	case GRENADE_OPTION:
		return GRENADE_OPTION;
		
	case SG_AMMO_ITEM:
	case SG_AMMO_OPTION:
		return SG_AMMO_OPTION;
		
	case MAG_AMMO_ITEM:
	case MAG_AMMO_OPTION:
		return MAG_AMMO_OPTION;
		
	case UZI_AMMO_ITEM:
	case UZI_AMMO_OPTION:
		return UZI_AMMO_OPTION;
		
	case HARPOON_AMMO_ITEM:
	case HARPOON_AMMO_OPTION:
		return HARPOON_AMMO_OPTION;
		
	case M16_AMMO_ITEM:
	case M16_AMMO_OPTION:
		return M16_AMMO_OPTION;
		
	case ROCKET_AMMO_ITEM:
	case ROCKET_AMMO_OPTION:
		return ROCKET_AMMO_OPTION;
		
	case GRENADE_AMMO_ITEM:
	case GRENADE_AMMO_OPTION:
		return GRENADE_AMMO_OPTION;
		
	case MEDI_ITEM:
	case MEDI_OPTION:
		return MEDI_OPTION;
		
	case BIGMEDI_ITEM:
	case BIGMEDI_OPTION:
		return BIGMEDI_OPTION;
		
	case FLAREBOX_ITEM:
	case FLARE_ITEM:
	case FLAREBOX_OPTION:
		return FLAREBOX_OPTION;
		
	case PUZZLE_ITEM1:
	case PUZZLE_OPTION1:
		return PUZZLE_OPTION1;
		
	case PUZZLE_ITEM2:
	case PUZZLE_OPTION2:
		return PUZZLE_OPTION2;
		
	case PUZZLE_ITEM3:
	case PUZZLE_OPTION3:
		return PUZZLE_OPTION3;
		
	case PUZZLE_ITEM4:
	case PUZZLE_OPTION4:
		return PUZZLE_OPTION4;
		
	case KEY_ITEM1:
	case KEY_OPTION1:
		return KEY_OPTION1;
		
	case KEY_ITEM2:
	case KEY_OPTION2:
		return KEY_OPTION2;
		
	case KEY_ITEM3:
	case KEY_OPTION3:
		return KEY_OPTION3;
		
	case KEY_ITEM4:
	case KEY_OPTION4:
		return KEY_OPTION4;
		
	case PICKUP_ITEM1:
	case PICKUP_OPTION1:
		return PICKUP_OPTION1;
		
	case PICKUP_ITEM2:
	case PICKUP_OPTION2:
		return PICKUP_OPTION2;
		
	case ICON_PICKUP1_ITEM:
	case ICON_PICKUP1_OPTION:
		return ICON_PICKUP1_OPTION;
		
	case ICON_PICKUP2_ITEM:
	case ICON_PICKUP2_OPTION:
		return ICON_PICKUP2_OPTION;
		
	case ICON_PICKUP3_ITEM:
	case ICON_PICKUP3_OPTION:
		return ICON_PICKUP3_OPTION;
		
	case ICON_PICKUP4_ITEM:
	case ICON_PICKUP4_OPTION:
		return ICON_PICKUP4_OPTION;

	case SAVEGAME_CRYSTAL_ITEM:
	case SAVEGAME_CRYSTAL_OPTION:
		
		if (tomb3.psx_saving)
			return SAVEGAME_CRYSTAL_OPTION;
	}

	return NO_ITEM;
}

long Inv_RequestItem(long item_number)
{
	INVENTORY_ITEM* item;
	long obj_num;

	obj_num = Inv_GetItemOption(item_number);

	for (int i = 0; i < inv_main_objects; i++)
	{
		item = inv_main_list[i];

		if (item->object_number == obj_num)
			return inv_main_qtys[i];
	}

	for (int i = 0; i < inv_keys_objects; i++)
	{
		item = inv_keys_list[i];

		if (item->object_number == obj_num)
			return inv_keys_qtys[i];
	}

	return 0;
}

void Inv_InsertItem(INVENTORY_ITEM* item)
{
	INVENTORY_ITEM* next;
	long i, n;

	if (item->inv_pos < 100)
	{
		for (i = 0; i < inv_main_objects; i++)
		{
			next = inv_main_list[i];

			if (next->inv_pos > item->inv_pos)
				break;
		}

		if (i == inv_main_objects)
		{
			inv_main_list[inv_main_objects] = item;
			inv_main_qtys[inv_main_objects] = 1;
			inv_main_objects++;
		}
		else
		{
			for (n = inv_main_objects; n > i - 1; n--)
			{
				inv_main_list[n + 1] = inv_main_list[n];
				inv_main_qtys[n + 1] = inv_main_qtys[n];
			}

			inv_main_list[i] = item;
			inv_main_qtys[i] = 1;
			inv_main_objects++;
		}
	}
	else
	{
		for (i = 0; i < inv_keys_objects; i++)
		{
			next = inv_keys_list[i];

			if (next->inv_pos > item->inv_pos)
				break;
		}

		if (i == inv_keys_objects)
		{
			inv_keys_list[inv_keys_objects] = item;
			inv_keys_qtys[inv_keys_objects] = 1;
			inv_keys_objects++;
		}
		else
		{
			for (n = inv_keys_objects; n > i - 1; n--)
			{
				inv_keys_list[n + 1] = inv_keys_list[n];
				inv_keys_qtys[n + 1] = inv_keys_qtys[n];
			}

			inv_keys_list[i] = item;
			inv_keys_qtys[i] = 1;
			inv_keys_objects++;
		}
	}
}

long Inv_RemoveItem(long item_number)
{
	INVENTORY_ITEM* item;
	long obj_num;

	obj_num = Inv_GetItemOption(item_number);

	for (int i = 0; i < inv_main_objects; i++)
	{
		item = inv_main_list[i];

		if (item->object_number == obj_num)
		{
			inv_main_qtys[i]--;

			if (inv_main_qtys[i] <= 0)
			{
				inv_main_objects--;

				for (int j = i; j < inv_main_objects; j++)
				{
					inv_main_list[j] = inv_main_list[j + 1];
					inv_main_qtys[j] = inv_main_qtys[j + 1];
				}
			}

			return 1;
		}
	}

	for (int i = 0; i < inv_keys_objects; i++)
	{
		item = inv_keys_list[i];

		if (item->object_number == obj_num)
		{
			inv_keys_qtys[i]--;

			if (inv_keys_qtys[i] <= 0)
			{
				inv_keys_objects--;

				for (int j = i; j < inv_keys_objects; j++)
				{
					inv_keys_list[j] = inv_keys_list[j + 1];
					inv_keys_qtys[j] = inv_keys_qtys[j + 1];
				}
			}

			return 1;
		}
	}

	return 0;
}

void Inv_RemoveAllItems()
{
	inv_main_objects = 0;
	inv_main_current = 0;
	inv_keys_objects = 0;
	inv_keys_current = 0;
}

long Inv_AddItem(long item_number)
{
	INVENTORY_ITEM* item;
	long obj_num, add, nMain, nKeys, n;

	obj_num = Inv_GetItemOption(item_number);
	add = 0;

	for (nMain = 0; nMain < inv_main_objects; nMain++)
	{
		item = inv_main_list[nMain];

		if (item->object_number == obj_num)
		{
			add = 1;
			break;
		}
	}

	for (nKeys = 0; nKeys < inv_keys_objects; nKeys++)
	{
		item = inv_keys_list[nKeys];

		if (item->object_number == obj_num)
		{
			add = 2;
			break;
		}
	}

	if (add == 1)
	{
		if (item_number == FLAREBOX_ITEM)
			inv_main_qtys[nMain] += 8;
		else if (item_number == HARPOON_AMMO_ITEM)
			lara.harpoon.ammo += 3;
		else
			inv_main_qtys[nMain]++;

		return 1;
	}

	if (add == 2)
	{
		inv_keys_qtys[nKeys]++;
		return 1;
	}

	switch (item_number)
	{
	case MAP_OPTION:
	case MAP_CLOSED:
		Inv_InsertItem(&icompass_option);
		return 1;

	case GUN_ITEM:
	case GUN_OPTION:
		Inv_InsertItem(&igun_option);

		if (lara.last_gun_type == LG_UNARMED)
		{
			lara.last_gun_type = LG_PISTOLS;
			lara.mesh_ptrs[THIGH_L] = meshes[objects[PISTOLS].mesh_index + THIGH_L];
			lara.mesh_ptrs[THIGH_R] = meshes[objects[PISTOLS].mesh_index + THIGH_R];
		}

		return 1;

	case SHOTGUN_ITEM:
	case SHOTGUN_OPTION:
		n = Inv_RequestItem(SG_AMMO_ITEM);

		if (n)
		{
			for (int i = 0; i < n; i++)
			{
				Inv_RemoveItem(SG_AMMO_ITEM);
				lara.shotgun.ammo += 12;
			}
		}

		lara.shotgun.ammo += 12;
		Inv_InsertItem(&ishotgun_option);

		if (lara.last_gun_type == LG_UNARMED)
			lara.last_gun_type = LG_SHOTGUN;

		if (!lara.back_gun)
			lara.back_gun = 3;

		GlobalItemReplace(SHOTGUN_ITEM, SG_AMMO_ITEM);
		return 0;

	case MAGNUM_ITEM:
	case MAGNUM_OPTION:
		n = Inv_RequestItem(MAG_AMMO_ITEM);

		if (n)
		{
			for (int i = 0; i < n; i++)
			{
				Inv_RemoveItem(MAG_AMMO_ITEM);
				lara.magnums.ammo += 10;
			}
		}

		lara.magnums.ammo += 10;
		Inv_InsertItem(&imagnum_option);
		GlobalItemReplace(MAGNUM_ITEM, MAG_AMMO_ITEM);
		return 0;

	case UZI_ITEM:
	case UZI_OPTION:
		n = Inv_RequestItem(UZI_AMMO_ITEM);

		if (n)
		{
			for (int i = 0; i < n; i++)
			{
				Inv_RemoveItem(UZI_AMMO_ITEM);
				lara.uzis.ammo += 40;
			}
		}

		lara.uzis.ammo += 40;
		Inv_InsertItem(&iuzi_option);
		GlobalItemReplace(UZI_ITEM, UZI_AMMO_ITEM);
		return 0;

	case HARPOON_ITEM:
	case HARPOON_OPTION:
		n = Inv_RequestItem(HARPOON_AMMO_ITEM);

		if (n)
		{
			for (int i = 0; i < n; i++)
				Inv_RemoveItem(HARPOON_AMMO_ITEM);
		}

		lara.harpoon.ammo += 3;
		Inv_InsertItem(&iharpoon_option);
		GlobalItemReplace(HARPOON_ITEM, HARPOON_AMMO_ITEM);
		return 0;

	case M16_ITEM:
	case M16_OPTION:
		n = Inv_RequestItem(M16_AMMO_ITEM);

		if (n)
		{
			for (int i = 0; i < n; i++)
			{
				Inv_RemoveItem(M16_AMMO_ITEM);
				lara.m16.ammo += 60;
			}
		}

		lara.m16.ammo += 60;
		Inv_InsertItem(&im16_option);
		GlobalItemReplace(M16_ITEM, M16_AMMO_ITEM);
		return 0;

	case ROCKET_GUN_ITEM:
	case ROCKET_OPTION:
		n = Inv_RequestItem(ROCKET_AMMO_ITEM);

		if (n)
		{
			for (int i = 0; i < n; i++)
			{
				Inv_RemoveItem(ROCKET_AMMO_ITEM);
				lara.rocket.ammo++;
			}
		}

		lara.rocket.ammo++;
		Inv_InsertItem(&irocket_option);
		GlobalItemReplace(ROCKET_GUN_ITEM, ROCKET_AMMO_ITEM);
		return 0;

	case GRENADE_GUN_ITEM:
	case GRENADE_OPTION:
		n = Inv_RequestItem(GRENADE_AMMO_ITEM);

		if (n)
		{
			for (int i = 0; i < n; i++)
			{
				Inv_RemoveItem(GRENADE_AMMO_ITEM);
				lara.grenade.ammo += 2;
			}
		}

		lara.grenade.ammo += 2;
		Inv_InsertItem(&igrenade_option);
		GlobalItemReplace(GRENADE_GUN_ITEM, GRENADE_AMMO_ITEM);
		return 0;

	case SG_AMMO_ITEM:
	case SG_AMMO_OPTION:

		if (Inv_RequestItem(SHOTGUN_ITEM))
			lara.shotgun.ammo += 12;
		else
			Inv_InsertItem(&isgunammo_option);

		return 0;

	case MAG_AMMO_ITEM:
	case MAG_AMMO_OPTION:

		if (Inv_RequestItem(MAGNUM_ITEM))
			lara.magnums.ammo += 10;
		else
			Inv_InsertItem(&imagammo_option);

		return 0;

	case UZI_AMMO_ITEM:
	case UZI_AMMO_OPTION:

		if (Inv_RequestItem(UZI_ITEM))
			lara.uzis.ammo += 40;
		else
			Inv_InsertItem(&iuziammo_option);

		return 0;

	case HARPOON_AMMO_ITEM:
	case HARPOON_AMMO_OPTION:
		lara.harpoon.ammo += 3;

		if (!Inv_RequestItem(HARPOON_ITEM) && !Inv_RequestItem(HARPOON_AMMO_ITEM))
			Inv_InsertItem(&iharpoonammo_option);

		return 0;

	case M16_AMMO_ITEM:
	case M16_AMMO_OPTION:

		if (Inv_RequestItem(M16_ITEM))
			lara.m16.ammo += 60;
		else
			Inv_InsertItem(&im16ammo_option);

		return 0;

	case ROCKET_AMMO_ITEM:
	case ROCKET_AMMO_OPTION:

		if (Inv_RequestItem(ROCKET_GUN_ITEM))
			lara.rocket.ammo++;
		else
			Inv_InsertItem(&irocketammo_option);

		return 0;

	case GRENADE_AMMO_ITEM:
	case GRENADE_AMMO_OPTION:

		if (Inv_RequestItem(GRENADE_GUN_ITEM))
			lara.grenade.ammo += 2;
		else
			Inv_InsertItem(&igrenadeammo_option);

		return 0;

	case MEDI_ITEM:
	case MEDI_OPTION:
		Inv_InsertItem(&imedi_option);
		return 1;

	case BIGMEDI_ITEM:
	case BIGMEDI_OPTION:
		Inv_InsertItem(&ibigmedi_option);
		return 1;

	case FLAREBOX_ITEM:
	case FLAREBOX_OPTION:
		Inv_InsertItem(&iflare_option);

		for (int i = 0; i < 7; i++)
			Inv_AddItem(FLARE_ITEM);

		return 1;

	case FLARE_ITEM:
		Inv_InsertItem(&iflare_option);
		return 1;

	case PUZZLE_ITEM1:
	case PUZZLE_OPTION1:
		Inv_InsertItem(&ipuzzle1_option);
		return 1;

	case PUZZLE_ITEM2:
	case PUZZLE_OPTION2:
		Inv_InsertItem(&ipuzzle2_option);
		return 1;

	case PUZZLE_ITEM3:
	case PUZZLE_OPTION3:
		Inv_InsertItem(&ipuzzle3_option);
		return 1;

	case PUZZLE_ITEM4:
	case PUZZLE_OPTION4:
		Inv_InsertItem(&ipuzzle4_option);
		return 1;

	case SECRET_ITEM1:
		savegame.secrets |= 1;
		return 1;

	case SECRET_ITEM2:
		savegame.secrets |= 2;
		return 1;

	case SECRET_ITEM3:
		savegame.secrets |= 4;
		return 1;

	case KEY_ITEM1:
	case KEY_OPTION1:
		Inv_InsertItem(&ikey1_option);

		if (CurrentLevel == LV_GYM)
			savegame.QuadbikeKeyFlag = 1;

		return 1;

	case KEY_ITEM2:
	case KEY_OPTION2:
		Inv_InsertItem(&ikey2_option);
		return 1;

	case KEY_ITEM3:
	case KEY_OPTION3:
		Inv_InsertItem(&ikey3_option);
		return 1;

	case KEY_ITEM4:
	case KEY_OPTION4:
		Inv_InsertItem(&ikey4_option);
		return 1;

	case PICKUP_ITEM1:
	case PICKUP_OPTION1:
		Inv_InsertItem(&ipickup1_option);
		return 1;

	case PICKUP_ITEM2:
	case PICKUP_OPTION2:
		Inv_InsertItem(&ipickup2_option);
		return 1;

	case ICON_PICKUP1_ITEM:
	case ICON_PICKUP1_OPTION:
		Inv_InsertItem(&icon1_option);
		return 1;

	case ICON_PICKUP2_ITEM:
	case ICON_PICKUP2_OPTION:
		Inv_InsertItem(&icon2_option);
		return 1;

	case ICON_PICKUP3_ITEM:
	case ICON_PICKUP3_OPTION:
		Inv_InsertItem(&icon3_option);
		return 1;

	case ICON_PICKUP4_ITEM:
	case ICON_PICKUP4_OPTION:
		Inv_InsertItem(&icon4_option);
		return 1;

	case SAVEGAME_CRYSTAL_ITEM:
	case SAVEGAME_CRYSTAL_OPTION:

		if (tomb3.psx_saving && CurrentLevel != LV_GYM)
		{
			Inv_InsertItem(&sgcrystal_option);
			return 1;
		}
	}

	return 0;
}

/************other stuff************/

void RemoveInventoryText()
{
	for (int i = 0; i < 2; i++)
	{
		T_RemovePrint(Inv_itemText[i]);
		Inv_itemText[i] = 0;
	}
}
