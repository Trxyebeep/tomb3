#include "../tomb3/pch.h"
#include "collide.h"
#include "sphere.h"

void ShiftItem(ITEM_INFO* item, COLL_INFO* coll)
{
	item->pos.x_pos += coll->shift.x;
	item->pos.y_pos += coll->shift.y;
	item->pos.z_pos += coll->shift.z;
	coll->shift.z = 0;
	coll->shift.y = 0;
	coll->shift.x = 0;
}

void ObjectCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l) && coll->enable_baddie_push)
		ItemPushLara(item, l, coll, 0, 1);
}

void ObjectCollisionSub(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l))
		ItemPushLara(item, l, coll, 0, 0);
}

void inject_collide(bool replace)
{
	INJECT(0x0041E690, ShiftItem, replace);
	INJECT(0x0041EBD0, ObjectCollision, replace);
	INJECT(0x0041EC30, ObjectCollisionSub, replace);
}
