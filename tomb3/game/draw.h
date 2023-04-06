#pragma once
#include "../global/types.h"

void phd_PopMatrix_I();
void phd_PushMatrix_I();
void phd_RotY_I(short angle);
void phd_RotX_I(short angle);
void phd_RotZ_I(short angle);
void phd_TranslateRel_I(long x, long y, long z);
void phd_TranslateRel_ID(long x, long y, long z, long x2, long y2, long z2);
void phd_RotYXZ_I(short y, short x, short z);
void phd_PutPolygons_I(short* objptr, long clip);
void gar_RotYXZsuperpack(short** pprot, long skip);
void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, long skip);
void InitInterpolate(long frac, long rate);
void InterpolateMatrix();
void InterpolateArmMatrix();
long GetFrames(ITEM_INFO* item, short* frm[], long* rate);
short* GetBestFrame(ITEM_INFO* item);
short* GetBoundsAccurate(ITEM_INFO* item);
void SetRoomBounds(short* door, long rn, ROOM_INFO* actualRoom);
void GetRoomBounds();
void ClipRoom(ROOM_INFO* r);
void PrintRooms(short current_room);
void DrawEffect(short fx_number);
void PrintObjects(short room_number);
void DrawGunFlash(long gun_type, long clip);
void CalculateObjectLighting(ITEM_INFO* item, short* frame);
void CalculateObjectLightingLara();
void DrawRooms(short current_room);
long DrawPhaseGame();
void DrawAnimatingItem(ITEM_INFO* item);
void DrawDummyItem(ITEM_INFO* item);

extern long box_lines[12][2];
extern short null_rotations[12];

extern long* IMptr;
extern long IM_rate;
extern long IM_frac;
extern long IMstack[768];

extern long number_draw_rooms;
extern short draw_rooms[100];
extern char IsJointUnderwater[15];
extern char GotJointPos[15];

extern long nPolyType;
extern long camera_underwater;
extern long mid_sort;

extern long outside;

extern MESH_INFO* CurrentMesh;
extern long CurrentRoom;
