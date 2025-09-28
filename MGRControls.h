#pragma once
#include "IniReader.h"
#include <string>
#include <XInput.h>

enum InputBitflags {
	WeaponMenuBit = 0x1,
	WeaponMenu2Bit = 0x2,
	HealBit = 0x4,
	TauntBit = 0x8,
	JumpBit = 0x10,
	InteractBit = 0x20,
	LightAttackBit = 0x40,
	HeavyAttackBit = 0x80,
	PauseBit = 0x100,
	CodecBit = 0x200,
	SubWeaponBit = 0x400,
	BladeModeBit = 0x800,
	AbilityBit = 0x1000,
	LockOnBit = 0x2000,
	RunBit = 0x4000,
	CamResetBit = 0x8000,
	LeftBit = 0x100000,
	RightBit = 0x200000,
	ForwardBit = 0x400000,
	BackwardBit = 0x800000,
	CamLeftBit = 0x1000000,
	CamRightBit = 0x2000000,
	CamUpBit = 0x4000000,
	CamDownBit = 0x8000000,
};

extern std::string Forward, Back, Left, Right, NormalAttack, StrongAttack, Jump, Interact,
	CamUp, CamDown, CamLeft, CamRight, Heal, Taunt, WeaponMenu, WeaponMenu2, Run,
	BladeMode, Subweapon, Lockon, Pause, Pause2, Ability, CamReset, GamepadForward,
	GamepadBack, GamepadLeft, GamepadRight, GamepadNormalAttack, GamepadStrongAttack,
	GamepadJump, GamepadInteract, GamepadCamUp, GamepadCamDown, GamepadCamLeft,
	GamepadCamRight, GamepadHeal, GamepadTaunt, GamepadWeaponMenu, GamepadWeaponMenu2,
	GamepadRun, GamepadBladeMode, GamepadSubweapon, GamepadLockon, GamepadPause, GamepadPause2,
	GamepadAbility, GamepadCamReset, GamepadSpawn;

float GetGamepadAnalog(int controllerIndex, const std::string& button);

float GetMouseAnalog(const std::string& button);

bool IsGamepadButtonPressed(int controllerIndex, const std::string& button);

std::string TryParseVKToHex(std::string in);

void LoadConfig();

bool CheckControlPressed(int controllerNumber, std::string Keybind, std::string GamepadBind);

