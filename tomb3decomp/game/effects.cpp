#include "../tomb3/pch.h"
#include "effects.h"
#include "laraanim.h"
#include "lara.h"
#include "../specific/game.h"
#include "effect2.h"

void LaraBreath(ITEM_INFO* item)
{
	PHD_VECTOR p;
	PHD_VECTOR v;

	if (lara.water_status == LARA_UNDERWATER || lara_item->hit_points < 0)
		return;

	if (lara_item->current_anim_state == AS_STOP)
	{
		if (lara_item->frame_number < anims[ANIM_BREATH].frame_base + 30)
			return;
	}
	else if (lara_item->current_anim_state == AS_DUCK)
	{
		if (lara_item->frame_number < anims[ANIM_DUCKBREATHE].frame_base + 30)
			return;
	}
	else if (wibble < 128 || wibble > 192)
		return;

	p.x = 0;
	p.y = -4;
	p.z = 64;
	GetLaraHandAbsPosition(&p, LARA_HEAD);

	v.x = (GetRandomControl() & 7) - 4;
	v.y = (GetRandomControl() & 7) - 8;
	v.z = (GetRandomControl() & 0x7F) + 64;
	GetLaraHandAbsPosition(&v, LARA_HEAD);

	TriggerBreath(p.x, p.y, p.z, v.x - p.x, v.y - p.y, v.z - p.z);
}

void inject_effects(bool replace)
{
	INJECT(0x0042E630, LaraBreath, replace);
}
