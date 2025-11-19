#pragma once
#include "MGRControls.h"
#include <BehaviorEmBase.h>
#include <cCameraGame.h>
#include <Pl0000.h>
#include <unordered_map>

extern bool EveryHeal;
extern bool isControllerIDSundownerPhase2[5];

bool SetFlagsForAction(Pl0000* player, int controllerNumber, std::string GamepadBind,
	InputBitflags bit, int* altField1 = nullptr, int* altField2 = nullptr);

bool SetFlagsForAnalog(Pl0000* player, int controllerNumber, std::string GamepadBind,
	InputBitflags bit, float* altField, bool invert);

void UpdateBossActions(BehaviorEmBase* Enemy, unsigned int BossActions[], int controllerNumber = -1);

void FullHandleAIBoss(BehaviorEmBase* Enemy, int controllerNumber, bool CanDamagePlayer);

void FullHandleAIPlayer(Pl0000* player, int controllerNumber, bool EnableDamageToPlayers);

void FullHandleDGPlayer(Behavior* dg, int controllerNumber, bool EnableDamageToPlayers);