#pragma once
#include "IniReader.h"
#include <string>
#include <XInput.h>

enum InputBitflags {
	WeaponMenuBit,
	WeaponMenu2Bit,
	HealBit,
	TauntBit,
	JumpBit,
	InteractBit,
	LightAttackBit,
	HeavyAttackBit,
	PauseBit,
	CodecBit,
	SubWeaponBit,
	BladeModeBit,
	AbilityBit,
	LockOnBit,
	RunBit,
	CamResetBit,
	LeftBit,
	RightBit,
	ForwardBit,
	BackwardBit,
	CamLeftBit,
	CamRightBit,
	CamUpBit,
	CamDownBit,
};

float GetGamepadAnalog(int controllerIndex, const std::string& button);

bool IsGamepadButtonPressed(int controllerIndex, const std::string& button);

std::string TryParseVKToHex(std::string in);

/*
std::string Forward, Back, Left, Right, NormalAttack, StrongAttack, Jump, Interact,
	CamUp, CamDown, CamLeft, CamRight, Heal, Taunt, WeaponMenu, WeaponMenu2, Run,
	BladeMode, Subweapon, Lockon, Pause, Pause2, Ability, CamReset, GamepadForward,
	GamepadBack, GamepadLeft, GamepadRight, GamepadNormalAttack, GamepadStrongAttack,
	GamepadJump, GamepadInteract, GamepadCamUp, GamepadCamDown, GamepadCamLeft,
	GamepadCamRight, GamepadHeal, GamepadTaunt, GamepadWeaponMenu, GamepadWeaponMenu2,
	GamepadRun, GamepadBladeMode, GamepadSubweapon, GamepadLockon, GamepadPause, GamepadPause2,
	GamepadAbility, GamepadCamReset, GamepadSpawnSam, GamepadSpawnWolf, GamepadSpawnArmstrong,
	GamepadSpawnBossSam;*/

void LoadConfig();

bool CheckControlPressed(int controllerNumber, std::string Keybind, std::string GamepadBind);

