#include "../tomb3/pch.h"
#include "effects.h"
#include "lara.h"
#include "../specific/game.h"
#include "effect2.h"
#include "draw.h"

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

long ItemNearLara(PHD_3DPOS* pos, long rad)
{
	short* bounds;
	long dx, dy, dz;

	dx = pos->x_pos - lara_item->pos.x_pos;
	dy = pos->y_pos - lara_item->pos.y_pos;
	dz = pos->z_pos - lara_item->pos.z_pos;

	if (dx >= -rad && dx <= rad && dz >= -rad && dz <= rad && dy >= -3072 && dy <= 3072 && SQUARE(dx) + SQUARE(dz) <= SQUARE(rad))
	{
		bounds = GetBoundsAccurate(lara_item);

		if (dy >= bounds[2] && dy <= bounds[3] + 100)
			return 1;
	}

	return 0;
}

void inject_effects(bool replace)
{
	INJECT(0x0042E630, LaraBreath, replace);
	INJECT(0x0042E170, ItemNearLara, replace);
}
