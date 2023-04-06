#include "../tomb3/pch.h"
#include "discord.h"
#include "libs/discord/discord_rpc.h"
#include "../game/gameflow.h"
#include "../tomb3/tomb3.h"
#include "../game/savegame.h"
#include "../game/control.h"
#include "../game/lara.h"

bool RPC_title;

static const char* RPC_GetLevelName()
{
	if (RPC_title)
		return "Title Screen";

	return GF_Level_Names[CurrentLevel];
}

static const char* RPC_GetTimer()
{
	long t;
	static char buf[64];

	if (RPC_title)
		return 0;

	if (CurrentLevel == LV_GYM)
	{
		t = savegame.best_assault_times[0];

		if (!t)
			sprintf(buf, "Assault Course: No Time Set!");
		else
			sprintf(buf, "Assault Course: %d:%2.2d.%02d", (t / 30) / 60, (t / 30) % 60, 334 * (t % 30) / 100);
	}
	else
	{
		t = savegame.timer / 30;
		sprintf(buf, "Time Taken: %02d:%02d:%02d", t / 3600, t / 60 % 60, t % 60);
	}

	return buf;
}

static const char* RPC_GetGoldLevelPic()
{
	switch (CurrentLevel)
	{
	case LV_GYM:
		return "home";

	case 1:
		return "high";

	case 2:
		return "lair";

	case 3:
		return "cliff";

	case 4:
		return "fish";

	case 5:
		return "mad";

	case 6:
		return "reunion";
	}
	
	return "title";
}

static const char* RPC_GetLevelPic()
{
	if (RPC_title)
		return "title";

	if (tomb3.gold)
		return RPC_GetGoldLevelPic();

	switch (CurrentLevel)
	{
	case LV_GYM:
		return "home";

	case LV_JUNGLE:
		return "jungle";

	case LV_TEMPLE:
		return "ruins";

	case LV_QUADBIKE:
		return "ganges";

	case LV_INDIABOSS:
		return "kaliya";

	case LV_SHORE:
		return "village";

	case LV_CRASH:
		return "crash";

	case LV_RAPIDS:
		return "madubu";

	case LV_PACBOSS:
		return "puna";

	case LV_ROOFTOPS:
		return "wharf";

	case LV_SEWER:
		return "train";

	case LV_TOWER:
		return "luds";

	case LV_OFFICE:
		return "city";

	case LV_DESERT:
		return "nevada";

	case LV_COMPOUND:
		return "hsc";

	case LV_AREA51:
		return "a51";

	case LV_ANTARC:
		return "antarc";

	case LV_MINES:
		return "rx";

	case LV_CITY:
		return "tinnos";

	case LV_CHAMBER:
		return "cavern";

	case LV_STPAULS:
		return "hallows";
	}

	return "title";
}

static const char* RPC_GetHealthPic()
{
	if (!lara_item)
		return 0;

	if (lara_item->hit_points > 666)
		return "green";

	if (lara_item->hit_points > 333)
		return "yellow";

	return "red";
}

static const char* RPC_GetHealthPercentage()
{
	static char buf[32];

	if (!lara_item)
		return 0;

	sprintf(buf, "Health: %i%%", lara_item->hit_points / 10);
	return buf;
}

void RPC_Init()
{
	DiscordEventHandlers handlers;

	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize(tomb3.gold ? "1057239187859972186" : "1057182754279784508", &handlers, 1, 0);
}

void RPC_Update()
{
	DiscordRichPresence RPC;

	memset(&RPC, 0, sizeof(RPC));

	RPC.details = RPC_GetLevelName();
	RPC.largeImageKey = RPC_GetLevelPic();
	RPC.largeImageText = RPC.details;

	RPC.smallImageKey = RPC_GetHealthPic();
	RPC.smallImageText = RPC_GetHealthPercentage();

	RPC.state = RPC_GetTimer();

	RPC.instance = 1;
	Discord_UpdatePresence(&RPC);
}

void RPC_close()
{
	Discord_Shutdown();
}
