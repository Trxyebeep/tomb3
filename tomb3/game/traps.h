#pragma once
#include "../global/types.h"

void LaraBurn();
void LavaBurn(ITEM_INFO* item);
void SpikeControl(short item_number);
void PropellerControl(short item_number);
void SideFlameDetection(FX_INFO* fx, long length);
void FlameControl(short fx_number);
void ControlSpikeWall(short item_number);
void ControlCeilingSpikes(short item_number);
void TriggerPendulumFlame(short item_number);
void SideFlameEmitterControl(short item_number);
void FlameEmitterControl(short item_number);
void FlameEmitter2Control(short item_number);
void FlameEmitter3Control(short item_number);
void DartsControl(short item_number);
void DartEmitterControl(short item_number);
void FallingCeiling(short item_number);
void TeethTrap(short item_number);
void FallingBlockCeiling(ITEM_INFO* item, long x, long y, long z, long* h);
void FallingBlockFloor(ITEM_INFO* item, long x, long y, long z, long* h);
void FallingBlock(short item_number);
void Pendulum(short item_number);
long OnTrapDoor(ITEM_INFO* item, long x, long z);
void TrapDoorCeiling(ITEM_INFO* item, long x, long y, long z, long* h);
void TrapDoorFloor(ITEM_INFO* item, long x, long y, long z, long* h);
void TrapDoorControl(short item_number);
void SpikeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SpringBoardControl(short item_number);
void InitialiseKillerStatue(short item_number);
void InitialiseBlade(short item_number);
void BladeControl(short item_number);
void IcicleControl(short item_number);
void SpinningBlade(short item_number);
void HookControl(short item_number);
void InitialiseRollingBall(short item_number);
void RollingBallControl(short item_number);
void RollingBallCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
