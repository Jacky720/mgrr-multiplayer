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

__declspec(selectany) std::string GamepadForward = "XINPUT_GAMEPAD_LEFT_THUMB_UP";
__declspec(selectany) std::string GamepadBack = "XINPUT_GAMEPAD_LEFT_THUMB_DOWN";
__declspec(selectany) std::string GamepadLeft = "XINPUT_GAMEPAD_LEFT_THUMB_LEFT";
__declspec(selectany) std::string GamepadRight = "XINPUT_GAMEPAD_LEFT_THUMB_RIGHT";
__declspec(selectany) std::string GamepadNormalAttack = "XINPUT_GAMEPAD_X";
__declspec(selectany) std::string GamepadStrongAttack = "XINPUT_GAMEPAD_Y";
__declspec(selectany) std::string GamepadJump = "XINPUT_GAMEPAD_A";
__declspec(selectany) std::string GamepadInteract = "XINPUT_GAMEPAD_B";
__declspec(selectany) std::string GamepadCamUp = "XINPUT_GAMEPAD_RIGHT_THUMB_UP";
__declspec(selectany) std::string GamepadCamDown = "XINPUT_GAMEPAD_RIGHT_THUMB_DOWN";
__declspec(selectany) std::string GamepadCamLeft = "XINPUT_GAMEPAD_RIGHT_THUMB_LEFT";
__declspec(selectany) std::string GamepadCamRight = "XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT";
__declspec(selectany) std::string GamepadHeal = "XINPUT_GAMEPAD_DPAD_DOWN";
__declspec(selectany) std::string GamepadTaunt = "XINPUT_GAMEPAD_DPAD_UP";
__declspec(selectany) std::string GamepadWeaponMenu = "XINPUT_GAMEPAD_DPAD_LEFT";
__declspec(selectany) std::string GamepadWeaponMenu2 = "XINPUT_GAMEPAD_DPAD_RIGHT";
__declspec(selectany) std::string GamepadRun = "XINPUT_GAMEPAD_RIGHT_TRIGGER";
__declspec(selectany) std::string GamepadBladeMode = "XINPUT_GAMEPAD_LEFT_TRIGGER";
__declspec(selectany) std::string GamepadSubweapon = "XINPUT_GAMEPAD_LEFT_SHOULDER";
__declspec(selectany) std::string GamepadLockon = "XINPUT_GAMEPAD_RIGHT_SHOULDER";
__declspec(selectany) std::string GamepadPause = "XINPUT_GAMEPAD_START";
__declspec(selectany) std::string GamepadPause2 = "XINPUT_GAMEPAD_BACK";
__declspec(selectany) std::string GamepadAbility = "XINPUT_GAMEPAD_LEFT_THUMB";
__declspec(selectany) std::string GamepadCamReset = "XINPUT_GAMEPAD_RIGHT_THUMB";

__declspec(selectany) std::string GamepadSpawn = "XINPUT_GAMEPAD_START";

float GetGamepadAnalog(int controllerIndex, const std::string& button);

float GetMouseAnalog(const std::string& button);

bool IsGamepadButtonPressed(int controllerIndex, const std::string& button);

std::string TryParseVKToHex(std::string in);

void LoadInputConfig() noexcept;

bool CheckControlPressed(int controllerNumber, std::string GamepadBind);

