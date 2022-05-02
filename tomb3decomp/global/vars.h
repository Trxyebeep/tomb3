#pragma once
#include "types.h"

#define VAR_(address, type)			(*(type*)(address))
#define ARRAY_(address, type, length)	(*(type(*)length)(address))

/*vars*/
#define lara_item	VAR_(0x006D62A4, ITEM_INFO*)
#define LaraOnPad	VAR_(0x006E2A14, uchar)
#define trigger_index	VAR_(0x006E29A8, short*)
#define lara	VAR_(0x006D61A0, LARA_INFO)
#define anims	VAR_(0x006E2074, ANIM_STRUCT*)
#define input	VAR_(0x0069FDF0, long)
#define room	VAR_(0x006E2890, ROOM_INFO*)
#define camera	VAR_(0x006E3040, CAMERA_INFO)
#define height_type	VAR_(0x006E2A24, long)
#define DashTimer	VAR_(0x006D62A0, short)

/*arrays*/


/*constants*/
#define NO_HEIGHT -32512
#define NO_ITEM	-1