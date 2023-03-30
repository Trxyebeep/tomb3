#pragma once
#include "../global/types.h"

void CalcLaraMatrices(long flag);
void GetLaraMeshPos(PHD_VECTOR* pos, long node);
void SetLaraUnderwaterNodes();
void ResetLaraUnderwaterNodes();
void NewDrawLara(ITEM_INFO* item);

enum lara_matrices_order
{
	LMX_HIPS,
	LMX_THIGH_L,
	LMX_CALF_L,
	LMX_FOOT_L,
	LMX_THIGH_R,
	LMX_CALF_R,
	LMX_FOOT_R,
	LMX_TORSO,
	LMX_HEAD,
	LMX_UARM_R,
	LMX_LARM_R,
	LMX_HAND_R,
	LMX_UARM_L,
	LMX_LARM_L,
	LMX_HAND_L
};
