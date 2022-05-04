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
#define wibble	VAR_(0x006261A8, long)
#define items	VAR_(0x006E2A1C, ITEM_INFO*)
#define gameflow	VAR_(0x006D77C0, GAMEFLOW_INFO)
#define cheat_hit_points	VAR_(0x006D6182, short)
#define savegame	VAR_(0x006D2280, SAVEGAME_INFO)
#define meshes	VAR_(0x006E1434, short**)
#define GF_NoFloor	VAR_(0x006261F0, short)
#define ExposureMeter	VAR_(0x006D6180, short)
#define mycoll	VAR_(0x004C7448, COLL_INFO*)
#define CurrentLevel	VAR_(0x00633F50, long)
#define phd_winxmax	VAR_(0x005316E0, short)
#define assault_timer_display	VAR_(0x006261BC, long)
#define QuadbikeLapTimeDisplayTimer	VAR_(0x006261D4, long)
#define assault_penalty_display_timer	VAR_(0x006261C8, long)
#define assault_target_penalties	VAR_(0x006D8760, long)
#define assault_penalties	VAR_(0x006261C4, long)
#define QuadbikeLapTime	VAR_(0x006261D0, long)
#define GlobalAlpha	VAR_(0x00625DF0, long)
#define phd_znear	VAR_(0x005316E8, long)
#define bBlueEffect	VAR_(0x005BAB00, bool)
#define BarsWidth	VAR_(0x004C805C, long)
#define DumpWidth	VAR_(0x004CD900, short)

/*arrays*/
#define objects	ARRAY_(0x006DCAC0, OBJECT_INFO, [376])

/*constants*/
#define NO_HEIGHT -32512
#define NO_ITEM	-1