#include "../tomb3/pch.h"
#include "psxsaves.h"
#include "../game/invfunc.h"
#include "../game/objects.h"
#include "../game/savegame.h"
#include "../specific/game.h"
#include "../specific/option.h"
#include "../game/gameflow.h"
#include "../game/text.h"
#include "../game/inventry.h"

void do_crystal_option(INVENTORY_ITEM* item)
{
	long select;

	if (!passport_text1)
		passport_text1 = T_Print(0, 0, 0, " ");

	GetSavedGamesList(&Load_Game_Requester);
	SetRequesterHeading(&Load_Game_Requester, GF_GameStrings[GT_SAVEGAME], 0, 0, 0);
	SetPassportRequesterSize(&Load_Game_Requester);
	select = Display_Requester(&Load_Game_Requester, 1, 1);

	if (select)
	{
		if (select > 0)
		{
			Inv_RemoveItem(SAVEGAME_CRYSTAL_OPTION);
			savegame.cp_secrets = savegame.secrets;
			savegame.cp_timer = savegame.timer;
			savegame.cp_kills = savegame.kills;
			savegame.cp_distance_travelled = savegame.distance_travelled;
			savegame.cp_ammo_used = savegame.ammo_used;
			savegame.cp_ammo_hit = savegame.ammo_hit;
			savegame.cp_health_used = savegame.health_used;
			CreateSaveGameInfo();
			S_SaveGame(&savegame, sizeof(SAVEGAME_INFO), select - 1);
		}
		else
			inputDB |= IN_SELECT;
	}

	T_RemovePrint(passport_text1);
	passport_text1 = 0;
}
