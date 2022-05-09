#include "../tomb3/pch.h"
#include "larafire.h"
#include "objects.h"

long WeaponObject(long weapon_type)
{
	switch (weapon_type)
	{
	case LG_MAGNUMS:
		return MAGNUM;

	case LG_UZIS:
		return UZI;

	case LG_SHOTGUN:
		return SHOTGUN;

	case LG_M16:
		return M16;

	case LG_ROCKET:
		return ROCKET_GUN;

	case LG_GRENADE:
		return GRENADE_GUN;

	case LG_HARPOON:
		return HARPOON_GUN;

	default:
		return PISTOLS;
	}
}

void inject_larafire(bool replace)
{
	INJECT(0x0044AF50, WeaponObject, replace);
}
