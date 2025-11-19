#pragma once
#include "MGRControls.h"
#include <BehaviorEmBase.h>
#include <cCameraGame.h>
#include <Pl0000.h>
#include <unordered_map>
#include "MPPlayer.h"

__declspec(selectany) bool EveryHeal = false;

struct setAction {
	unsigned int ActionId;
	unsigned int SubactionId;
	unsigned int MaxSubaction; // For checking states that contain a range of subactions 0-x
	unsigned int AnimationMapId;
	float AnimationLength;
};


struct ActionList {
	setAction Idle;
	setAction Walking;
	setAction LightAttack;
	setAction HeavyAttack;
	setAction Interaction;
	setAction Jumping;
	setAction Taunting;
	setAction Special;
	setAction EndSpecial;
	setAction StartRun;
	setAction MidRun;
	setAction EndRun;
};

bool SetFlagsForAction(MPPlayer* player, std::string GamepadBind, InputBitflags bit,
	unsigned int* altField1 = nullptr, unsigned int* altField2 = nullptr, cInput::InputUnit* curInput = nullptr);

bool SetFlagsForAnalog(MPPlayer* player, std::string GamepadBind, InputBitflags bit,
	float* altField, bool invert, cInput::InputUnit* curInput = nullptr);

void UpdateBossActions(MPPlayer* Enemy, unsigned int* BossActions); // Rejects ActionList* for some reason

void FullHandleAIBoss(MPPlayer* Enemy);

void FullHandleAIPlayer(MPPlayer* player);

void FullHandleDGPlayer(MPPlayer* dg);