#include "../tomb3/pch.h"
#include "objlight.h"
#include "control.h"
#include "gameflow.h"
#include "effect2.h"
#include "sound.h"
#include "../specific/smain.h"

void ControlStrobeLight(short item_number)
{
	ITEM_INFO* item;
	long s, c;
	short ang;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

#ifdef RANDO_STUFF
	if (rando.levels[RANDOLEVEL].original_id == LV_AREA51 && !item->really_active)
#else
	if (CurrentLevel == LV_AREA51 && !item->really_active)
#endif
		return;

	item->pos.y_rot += 2912;
	ang = ((item->pos.y_rot + 0x5800) >> 4) & 0xFFF;
	TriggerAlertLight(item->pos.x_pos, item->pos.y_pos - 512, item->pos.z_pos, 31, 8, 0, ang, item->room_number);
	s = rcossin_tbl[ang << 1] >> 4;
	c = rcossin_tbl[(ang << 1) + 1] >> 4;
	TriggerDynamic(item->pos.x_pos + s, item->pos.y_pos - 768, item->pos.z_pos + c, 6, 31, 12, 0);

	if (!(wibble & 0x7F))
		SoundEffect(SFX_ALARM_1, &item->pos, SFX_DEFAULT);

	item->item_flags[0]++;

	if (item->item_flags[0] > 1800)
	{
		item->really_active = 0;
		item->item_flags[0] = 0;
	}
}

void ControlPulseLight(short item_number)
{
	ITEM_INFO* item;
	long f;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	item->item_flags[0] += 728;
	f = abs(rcossin_tbl[((item->item_flags[0] >> 4) & 0xFFF) << 1] >> 7);

	if (f > 31)
		f = 31;
	else if (f < 8)
	{
		f = 8;
		item->item_flags[0] += 2048;
	}

	TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, f, 31, 12, 0);
}

void inject_objlight(bool replace)
{
	INJECT(0x00459B00, ControlStrobeLight, inject_rando ? 1 : replace);
	INJECT(0x00459C00, ControlPulseLight, replace);
}
