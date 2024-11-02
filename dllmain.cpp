// Main program by Ruslan and Frouk
// complete bindings, arbitrary player count, analog support by Jacky720
#include "pch.h"
#include <assert.h>
#include "gui.h"
#include <Events.h>
#include "imgui/imgui.h"
#include "IniReader.h"
#include "spawner.cpp"
#include <Trigger.h>
#include <GameMenuStatus.h>
#include <Pl0000.h>
#include <EntitySystem.h>
#include <Entity.h>
#include <cCameraGame.h>
#include <cGameUIManager.h>
#include <injector/injector.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <PlayerManagerImplement.h>
#include <XInput.h>
#include <BehaviorEmBase.h>
#include <d3dx9.h>
#include <math.h>

#ifdef _MSC_VER < 1700 //pre 2012
#pragma comment(lib,"Xinput.lib")
#else
#pragma comment(lib,"Xinput9_1_0.lib")
#endif
#include <Windows.h>
#include <Camera.h>

bool configLoaded = false;
//bool SamSpawned = false;
//bool WolfSpawned = false;

bool AutoNormalAttackEnable = false;
bool AutoStrongAttackEnable = false;

/*
bool ArmstrongAtOnce = false;
bool SundownerAtOnce = false;
bool MistralAtOnce = false;
bool MonsoonAtOnce = false;
bool SamAtOnce = false;
*/



bool PlayAsMistral = false;
bool PlayAsMonsoon = false;
bool PlayAsSundowner = false;
bool PlayAsSam = false;
bool BossSamCanDamagePlayer = true;
bool PlayAsArmstrong = false;
bool ArmstrongCanDamagePlayer = true;


bool EnableDamageToPlayers = false;

//unsigned int sword = 0x0;
//unsigned int originalSword = 0x0;


bool isMenuShow = false;


//eObjID mObjId = (eObjID)0x0;

unsigned int HotKey = VK_INSERT; //Hotkey for menu show


std::string Forward = "26";
std::string Back = "28";
std::string Left = "25";
std::string Right = "27";
std::string NormalAttack = "49"; // I
std::string StrongAttack = "4F"; // O
std::string Jump = "50"; // P
std::string Interact = "74";
std::string CamUp = "70";
std::string CamDown = "71";
std::string CamLeft = "72";
std::string CamRight = "73";
std::string Heal = "76";
std::string Taunt = "59"; // Y
std::string WeaponMenu = "77";
std::string WeaponMenu2 = "78";
std::string Run = "55"; // U
std::string BladeMode = "4C"; // L
std::string Subweapon = "79";
std::string Lockon = "80";
std::string Pause = "81";
std::string Pause2 = "75";
std::string Ability = "82";
std::string CamReset = "83";

std::string GamepadForward = "XINPUT_GAMEPAD_LEFT_THUMB_UP";
std::string GamepadBack = "XINPUT_GAMEPAD_LEFT_THUMB_DOWN";
std::string GamepadLeft = "XINPUT_GAMEPAD_LEFT_THUMB_LEFT";
std::string GamepadRight = "XINPUT_GAMEPAD_LEFT_THUMB_RIGHT";
std::string GamepadNormalAttack = "XINPUT_GAMEPAD_X";
std::string GamepadStrongAttack = "XINPUT_GAMEPAD_Y";
std::string GamepadJump = "XINPUT_GAMEPAD_A";
std::string GamepadInteract = "XINPUT_GAMEPAD_B";
std::string GamepadCamUp = "XINPUT_GAMEPAD_RIGHT_THUMB_UP";
std::string GamepadCamDown = "XINPUT_GAMEPAD_RIGHT_THUMB_DOWN";
std::string GamepadCamLeft = "XINPUT_GAMEPAD_RIGHT_THUMB_LEFT";
std::string GamepadCamRight = "XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT";
std::string GamepadHeal = "XINPUT_GAMEPAD_DPAD_DOWN";
std::string GamepadTaunt = "XINPUT_GAMEPAD_DPAD_UP";
std::string GamepadWeaponMenu = "XINPUT_GAMEPAD_DPAD_LEFT";
std::string GamepadWeaponMenu2 = "XINPUT_GAMEPAD_DPAD_RIGHT";
std::string GamepadRun = "XINPUT_GAMEPAD_RIGHT_TRIGGER";
std::string GamepadBladeMode = "XINPUT_GAMEPAD_LEFT_TRIGGER";
std::string GamepadSubweapon = "XINPUT_GAMEPAD_LEFT_SHOULDER";
std::string GamepadLockon = "XINPUT_GAMEPAD_RIGHT_SHOULDER";
std::string GamepadPause = "XINPUT_GAMEPAD_START";
std::string GamepadPause2 = "XINPUT_GAMEPAD_BACK";
std::string GamepadAbility = "XINPUT_GAMEPAD_LEFT_THUMB";
std::string GamepadCamReset = "XINPUT_GAMEPAD_RIGHT_THUMB";

std::string GamepadSpawnSam = "XINPUT_GAMEPAD_START";
std::string GamepadSpawnWolf = "XINPUT_GAMEPAD_BACK";
std::string GamepadSpawnArmstrong = "XINPUT_GAMEPAD_LEFT_TRIGGER";
std::string GamepadSpawnBossSam = "XINPUT_GAMEPAD_RIGHT_TRIGGER";

bool DisableNumberBinds = false;



DWORD dwResult;
XINPUT_STATE state;

//this struct begins from player CF8 field (player key isHolding), size - 0x30;
struct KeysStruct
{
	unsigned int keyCode; //CF8
	unsigned int  field_4;//CFC
	unsigned int  field_8;//D00
	unsigned int  field_C;//D04
	float moveFloatA;//D08
	float moveFloatB;//D0C
	float bladeMoveFloatA;//D10
	float bladeMoveFloatB;//D14
	unsigned int field_20;//D18
	unsigned int field_24;//D1C
	unsigned int field_28;//D20
	unsigned int field_2C;//D24
};

KeysStruct* m_pKeysStruct;
KeysStruct m_nKeysStruct;


struct ModelItems
{
	unsigned int m_nModel;
	unsigned int m_nHair;
	unsigned int m_nVisor;
	unsigned int m_nSheath;
	unsigned int m_nHead;
};




ModelItems* modelItems;
ModelItems originalModelItems;


//original address in game is "METAL GEAR RISING REVENGEANCE.exe"+1777E60
struct Player1_Keys
{
	unsigned int key_forward = 'W';
	unsigned int key_back = 'S';
	unsigned int key_left = 'A';
	unsigned int key_right = 'D';
	unsigned int key_slowWalking = VK_TAB;
	unsigned int key_jump = VK_SPACE;
	unsigned int key_normalAttack = 0x80000001;
	unsigned int key_strongAttack = 0x80000002;
	unsigned int key_bladeMode = 0x9F;
	unsigned int key_run = 0x9B;
	unsigned int key_action = 'F';
	unsigned int key_ripperMode = 'R';
	unsigned int key_target = 'E';
	unsigned int key_useSecondaryWeapon = 'V';
	unsigned int key_useItem = 'Q';
	unsigned int key_visor = '1';
	unsigned int key_weaponSelect = '2';
	unsigned int key_codec = '3';
	unsigned int key_pause = VK_SCROLL;
	unsigned int key_finish = VK_SCROLL;
	unsigned int key_dodge = 0x80000004;

};

struct Keys {
	unsigned int m_nKeys[28];
};

Keys* keys = injector::ReadMemory<Keys*>(shared::base + 0x177B7C0, true);

unsigned int keyArray[] = {
	'W', 'S', 'A', 'D', VK_TAB, VK_SPACE, 0x80000001, 0x80000002, 0x9F,
	0x9B, 'F', 'R', 'E', 'V'
};

Player1_Keys pl1Keys;

KeysStruct currentKey;

enum InputBitflags { // Initially written by Jacky720
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


bool isPlayerAtOnce = false;

Pl0000* players[5] = { nullptr };
eObjID playerTypes[5] = { (eObjID)0 };

void Spawner(eObjID id, int controllerIndex = -1) {
	if (controllerIndex > -1) {
		playerTypes[controllerIndex + 1] = id;
	}
	else {
		for (int i = 0; i < 5; i++) {
			if (!playerTypes[i]) {
				playerTypes[i] = id;
				break;
			}
		}
	}

	if (id == (eObjID)0x11400) {
		modelItems->m_nHair = 0x11401;
		modelItems->m_nVisor = 0x11402;
		modelItems->m_nSheath = 0x11404;
		modelItems->m_nHead = 0x11405;
		m_EntQueue.push_back({ .mObjId = id, .iSetType = 0,.bWorkFail = !isObjExists(id) });
	}
	else {
		m_EntQueue.push_back({ .mObjId = id, .iSetType = 0,.bWorkFail = !isObjExists(id) });
	}

	//injector::WriteMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, 0x11501, true);

}

enum GamepadAnalogValues {
	LeftX,
	LeftY,
	RightX,
	RightY
};

// Returns a scale factor (expected 0.0 to 1.0, technically can give -1.0 to 1.0) for analog movement
float GetGamepadAnalog(int controllerIndex, const std::string& button)
{
	XINPUT_STATE state;
	DWORD dwResult = XInputGetState(controllerIndex, &state);



	if (dwResult == ERROR_SUCCESS)
	{
		GamepadAnalogValues mode;
		float invertFactor = 1.0;
		if (button == "XINPUT_GAMEPAD_LEFT_THUMB_UP") mode = LeftY;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_DOWN") { mode = LeftY; invertFactor = -1.0; }
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_RIGHT") mode = LeftX;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_LEFT") { mode = LeftX; invertFactor = -1.0; }
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_UP") mode = RightY;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_DOWN") { mode = RightY; invertFactor = -1.0; }
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT") mode = RightX;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_LEFT") { mode = RightX; invertFactor = -1.0; }
		else { return 1.0; } // Default behavior shouldn't slow you down

		switch (mode) {
		case LeftX: return invertFactor * state.Gamepad.sThumbLX / SHRT_MAX;
		case RightX: return invertFactor * state.Gamepad.sThumbRX / SHRT_MAX;
		case LeftY: return invertFactor * state.Gamepad.sThumbLY / SHRT_MAX;
		case RightY: return invertFactor * state.Gamepad.sThumbRY / SHRT_MAX;
		}
	}
	return 0.0;
}

bool IsGamepadButtonPressed(int controllerIndex, const std::string& button)
{
	if (controllerIndex < 0)
		return false;

	XINPUT_STATE state;
	DWORD dwResult = XInputGetState(controllerIndex, &state);


	if (dwResult == ERROR_SUCCESS)
	{
		if (button == "XINPUT_GAMEPAD_A")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
		else if (button == "XINPUT_GAMEPAD_B")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
		else if (button == "XINPUT_GAMEPAD_X")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
		else if (button == "XINPUT_GAMEPAD_Y")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
		else if (button == "XINPUT_GAMEPAD_LEFT_SHOULDER")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
		else if (button == "XINPUT_GAMEPAD_RIGHT_SHOULDER")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
		else if (button == "XINPUT_GAMEPAD_BACK")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
		else if (button == "XINPUT_GAMEPAD_START")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
		else if (button == "XINPUT_GAMEPAD_DPAD_UP")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
		else if (button == "XINPUT_GAMEPAD_DPAD_DOWN")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
		else if (button == "XINPUT_GAMEPAD_DPAD_LEFT")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
		else if (button == "XINPUT_GAMEPAD_DPAD_RIGHT")
			return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_UP")
			return state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_DOWN")
			return state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_LEFT")
			return state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_RIGHT")
			return state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_UP")
			return state.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_DOWN")
			return state.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_LEFT")
			return state.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT")
			return state.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		else if (button == "XINPUT_GAMEPAD_LEFT_TRIGGER")
			return state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
		else if (button == "XINPUT_GAMEPAD_RIGHT_TRIGGER")
			return state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
	}

	return false;
}

std::string TryParseVKToHex(std::string in) {
	// Ruslan found the docs, I just copied them into a massive if-chain --Jacky720
	// http://www.kbdedit.com/manual/low_level_vk_list.html
	if (in == "LMB" || in == "Mouse1" || in == "VK_LBUTTON")
		return "01";
	if (in == "RMB" || in == "Mouse2" || in == "VK_RBUTTON")
		return "02";
	if (in == "VK_CANCEL" || in == "Break")
		return "03";
	if (in == "MMB" || in == "Mouse3" || in == "VK_MBUTTON")
		return "04";
	if (in == "Mouse4" || in == "VK_XBUTTON1")
		return "05";
	if (in == "Mouse5" || in == "VK_XBUTTON2")
		return "06";
	if (in == "VK_BACK" || in == "Backspace")
		return "08";
	if (in == "VK_TAB")
		return "09";
	if (in == "VK_CLEAR" || in == "Clear")
		return "0C";
	if (in == "VK_RETURN" || in == "Return" || in == "Enter")
		return "0D";
	if (in == "VK_PAUSE" || in == "Pause")
		return "13";
	if (in == "VK_CAPITAL" || in == "CapsLock" || in == "Caps" || in == "Caps Lock")
		return "14";
	if (in == "VK_KANA" || in == "Kana")
		return "15";
	if (in == "VK_JUNJA" || in == "Junja")
		return "17";
	if (in == "VK_FINAL" || in == "Final")
		return "18";
	if (in == "VK_KANJI" || in == "Kanji")
		return "19";
	if (in == "VK_ESCAPE" || in == "Esc" || in == "Escape")
		return "1B";
	if (in == "VK_CONVERT" || in == "Convert")
		return "1C";
	if (in == "VK_NONCONVERT" || in == "NonConvert" || in == "Non Convert")
		return "1D";
	if (in == "VK_ACCEPT" || in == "Accept")
		return "1E";
	if (in == "VK_MODECHANGE" || in == "Mode Change" || in == "ModeChange")
		return "1F";
	if (in == "VK_SPACE" || in == "Space")
		return "20";
	if (in == "VK_PRIOR" || in == "PageUp" || in == "Page Up" || in == "Prior")
		return "21";
	if (in == "VK_NEXT" || in == "PageDown" || in == "Page Down" || in == "Next")
		return "22";
	if (in == "VK_END" || in == "End")
		return "23";
	if (in == "VK_HOME" || in == "Home")
		return "24";
	if (in == "VK_LEFT" || in == "Left" || in == "Left Arrow" || in == "LeftArrow")
		return "25";
	if (in == "VK_UP" || in == "Up" || in == "Up Arrow" || in == "UpArrow")
		return "26";
	if (in == "VK_RIGHT" || in == "Right" || in == "Right Arrow" || in == "RightArrow")
		return "27";
	if (in == "VK_DOWN" || in == "Down" || in == "Down Arrow" || in == "DownArrow")
		return "28";
	if (in == "VK_SELECT" || in == "Select")
		return "29";
	if (in == "VK_PRINT" || in == "Print")
		return "2A";
	if (in == "VK_EXECUTE" || in == "Execute")
		return "2B";
	if (in == "VK_SNAPSHOT" || in == "Snapshot" || in == "PrintScreen" || in == "Print Screen")
		return "2C";
	if (in == "VK_INSERT" || in == "Insert")
		return "2D";
	if (in == "VK_DELETE" || in == "Delete")
		return "2E";
	if (in == "VK_HELP" || in == "Help")
		return "2F";
	if (in == "VK_LWIN" || in == "LeftWin" || in == "Left Win" || in == "Windows" || in == "Win" || in == "LWin")
		return "5B";
	if (in == "VK_RWIN" || in == "RightWin" || in == "Right Win" || in == "RWin")
		return "5C";
	if (in == "VK_APPS" || in == "Apps" || in == "Context")
		return "5D";
	if (in == "VK_SLEEP" || in == "Sleep")
		return "5F";
	if (in == "VK_MULTIPLY" || in == "Multiply" || in == "Numpad*" || in == "Numpad *" || in == "Mul")
		return "6A";
	if (in == "VK_ADD" || in == "Add" || in == "Numpad+" || in == "Numpad +")
		return "6B";
	if (in == "VK_SEPARATOR" || in == "Separator" || in == "Seperator")
		return "6C";
	if (in == "VK_SUBTRACT" || in == "Subtract" || in == "Numpad-" || in == "Numpad -" || in == "Sub")
		return "6D";
	if (in == "VK_DECIMAL" || in == "Decimal" || in == "Numpad." || in == "Numpad .")
		return "6E";
	if (in == "VK_DIVIDE" || in == "Divide" || in == "Numpad/" || in == "Numpad /" || in == "Div")
		return "6F";
	if (in == "VK_NUMLOCK" || in == "Num Lock" || in == "NumLock")
		return "90";
	if (in == "VK_SCROLL" || in == "Scroll Lock" || in == "ScrollLock" || in == "Scroll")
		return "91";
	if (in == "VK_OEM_FJ_JISHO" || in == "Jisho")
		return "92";
	if (in == "VK_OEM_FJ_MASSHOU" || in == "Masshou" || in == "Mashu")
		return "93";
	if (in == "VK_OEM_FJ_TOUROKU" || in == "Touroku")
		return "94";
	if (in == "VK_OEM_FJ_LOYA" || in == "Loya")
		return "95";
	if (in == "VK_OEM_FJ_ROYA" || in == "Roya")
		return "96";
	if (in == "VK_LSHIFT" || in == "Left Shift" || in == "LeftShift" || in == "Shift")
		return "A0";
	if (in == "VK_RSHIFT" || in == "Right Shift" || in == "RightShift")
		return "A1";
	if (in == "VK_LCONTROL" || in == "Left Control" || in == "LeftControl" || in == "Control")
		return "A2";
	if (in == "VK_RCONTROL" || in == "Right Control" || in == "RightControl")
		return "A3";
	if (in == "VK_LMENU" || in == "Left Alt" || in == "LeftAlt" || in == "Alt")
		return "A4";
	if (in == "VK_RMENU" || in == "Right Alt" || in == "RightAlt")
		return "A5";
	if (in == "VK_BROWSER_BACK" || in == "Browser Back" || in == "BrowserBack")
		return "A6";
	if (in == "VK_BROWSER_FORWARD" || in == "Browser Forward" || in == "BrowserForward")
		return "A7";
	if (in == "VK_BROWSER_REFRESH" || in == "Browser Refresh" || in == "BrowserRefresh")
		return "A8";
	if (in == "VK_BROWSER_STOP" || in == "Browser Stop" || in == "BrowserStop")
		return "A9";
	if (in == "VK_BROWSER_SEARCH" || in == "Browser Search" || in == "BrowserSearch")
		return "AA";
	if (in == "VK_BROWSER_FAVORITES" || in == "Browser Favorites" || in == "BrowserFavorites")
		return "AB";
	if (in == "VK_BROWSER_HOME" || in == "Browser Home" || in == "BrowserHome")
		return "AC";
	if (in == "VK_VOLUME_MUTE" || in == "Mute" || in == "Volume Mute" || in == "VolumeMute")
		return "AD";
	if (in == "VK_VOLUME_DOWN" || in == "Volume Down" || in == "VolumeDown")
		return "AE";
	if (in == "VK_VOLUME_UP" || in == "Volume Up" || in == "VolumeUp")
		return "AF";
	if (in == "VK_MEDIA_NEXT_TRACK" || in == "Next Track" || in == "NextTrack")
		return "B0";
	if (in == "VK_MEDIA_PREV_TRACK" || in == "Previous Track" || in == "PreviousTrack" || in == "PrevTrack")
		return "B1";
	if (in == "VK_MEDIA_STOP" || in == "Stop" || in == "Media Stop" || in == "MediaStop")
		return "B2";
	if (in == "VK_MEDIA_PLAY_PAUSE" || in == "Play/Pause" || in == "Media Play" || in == "MediaPlay")
		return "B3";
	if (in == "VK_LAUNCH_MAIL" || in == "Mail" || in == "Launch Mail" || in == "LaunchMail")
		return "B4";
	if (in == "VK_LAUNCH_MEDIA_SELECT" || in == "Media" || in == "Media Select" || in == "MediaSelect")
		return "B5";
	if (in == "VK_LAUNCH_APP1" || in == "App1" || in == "App 1" || in == "Application 1")
		return "B6";
	if (in == "VK_LAUNCH_APP2" || in == "App2" || in == "App 2" || in == "Application 2")
		return "B7";
	if (in == "VK_OEM_1" || in == ";" || in == "';'")
		return "BA";
	if (in == "VK_OEM_PLUS" || in == "=" || in == "'='" || in == "Plus")
		return "BB";
	if (in == "VK_OEM_COMMA" || in == "," || in == "','")
		return "BC";
	if (in == "VK_OEM_MINUS" || in == "-" || in == "'-'")
		return "BD";
	if (in == "VK_OEM_PERIOD" || in == "." || in == "'.'")
		return "BE";
	if (in == "VK_OEM_2" || in == "/" || in == "'/'")
		return "BF";
	if (in == "VK_OEM_3" || in == "`" || in == "'`'")
		return "C0";
	if (in == "VK_ABNT_C1" || in == "Abnt C1" || in == "AbntC1")
		return "C1";
	if (in == "VK_ABNT_C2" || in == "Abnt C2" || in == "AbntC2")
		return "C2";
	if (in == "VK_OEM_4" || in == "[" || in == "'['")
		return "DB";
	if (in == "VK_OEM_5" || in == "\\" || in == "'\\'")
		return "DC";
	if (in == "VK_OEM_6" || in == "]" || in == "']'")
		return "DD";
	if (in == "VK_OEM_7" || in == "'" || in == "'\\''")
		return "DE";
	if (in == "VK_OEM_8" || in == "!" || in == "'!'")
		return "DF";
	if (in == "VK_OEM_AX" || in == "Ax")
		return "E1";
	if (in == "VK_OEM_102" || in == "<" || in == "'<'")
		return "E2";
	if (in == "VK_ICO_HELP" || in == "Ico Help" || in == "IcoHelp")
		return "E3";
	if (in == "VK_ICO_00")
		return "E4";
	if (in == "VK_PROCESSKEY" || in == "Process")
		return "E5";
	if (in == "VK_ICO_CLEAR" || in == "Ico Clear" || in == "IcoClear")
		return "E6";
	if (in == "VK_PACKET" || in == "Packet")
		return "E7";
	if (in == "VK_OEM_RESET" || in == "Reset")
		return "E9";
	if (in == "VK_OEM_JUMP" || in == "Jump")
		return "EA";
	if (in == "VK_OEM_PA1" || in == "OemPa1" || in == "Oem Pa1")
		return "EB";
	if (in == "VK_OEM_PA2" || in == "OemPa2" || in == "Oem Pa2")
		return "EC";
	if (in == "VK_OEM_PA3" || in == "OemPa3" || in == "Oem Pa3")
		return "ED";
	if (in == "VK_OEM_WSCTRL" || in == "WsCtrl")
		return "EE";
	if (in == "VK_OEM_CUSEL" || in == "CuSel" || in == "Cu Sel")
		return "EF";
	if (in == "VK_OEM_ATTN" || in == "OemAttn" || in == "Oem Attn")
		return "F0";
	if (in == "VK_OEM_FINISH" || in == "Finish")
		return "F1";
	if (in == "VK_OEM_COPY" || in == "Copy")
		return "F2";
	if (in == "VK_OEM_AUTO" || in == "Auto")
		return "F3";
	if (in == "VK_OEM_ENLW" || in == "Enlw")
		return "F4";
	if (in == "VK_OEM_BACKTAB" || in == "Back Tab" || in == "BackTab")
		return "F5";
	if (in == "VK_ATTN" || in == "Attn")
		return "F6";
	if (in == "VK_CRSEL" || in == "CrSel" || in == "Cr Sel")
		return "F7";
	if (in == "VK_EXSEL" || in == "ExSel" || in == "Ex Sel")
		return "F8";
	if (in == "VK_EREOF" || in == "ErEof" || in == "Er Eof")
		return "F9";
	if (in == "VK_PLAY" || in == "Play")
		return "FA";
	if (in == "VK_ZOOM" || in == "Zoom")
		return "FB";
	if (in == "VK_NONAME" || in == "NoName" || in == "No Name")
		return "FC";
	if (in == "VK_PA1" || in == "Pa1" || in == "PA1")
		return "FD";
	if (in == "VK_OEM_CLEAR" || in == "OemClear" || in == "Oem Clear")
		return "FE";
	if (in == "VK__none_" || in == "None")
		return "FF";

	if (in.starts_with("VK_F") || (in[0] == 'F' && in.length() > 1)) {
		std::string num = in.substr(1);
		if (in[0] == 'V') num = in.substr(4);
		switch (std::stoi(num)) {
		case 1: return "70";
		case 2: return "71";
		case 3: return "72";
		case 4: return "73";
		case 5: return "74";
		case 6: return "75";
		case 7: return "76";
		case 8: return "77";
		case 9: return "78";
		case 10: return "79";
		case 11: return "7A";
		case 12: return "7B";
		case 13: return "7C";
		case 14: return "7D";
		case 15: return "7E";
		case 16: return "7F";
		case 17: return "80";
		case 18: return "81";
		case 19: return "82";
		case 20: return "83";
		case 21: return "84";
		case 22: return "85";
		case 23: return "86";
		case 24: return "87";
		}
	}
	if (in.starts_with("VK_NUMPAD") || in.starts_with("Numpad")) {
		char usednum = in[6];
		if (in[0] == 'V')
			usednum = in[9];
		else if (usednum == ' ')
			usednum = in[7];
		return "6" + usednum;
	}
	if (in.starts_with("VK_KEY_") || in.length() == 1 || (in.length() == 3 && in[0] == '\'' && in[2] == '\'')) {
		char usedchar = in[0];
		if (in.length() == 3)
			usedchar = in[1];
		if (in.length() > 3)
			usedchar = in[7];
		switch (usedchar) {
		case ' ': return "20";
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': return "3" + usedchar;
		case 'A': return "41";
		case 'B': return "42";
		case 'C': return "43";
		case 'D': return "44";
		case 'E': return "45";
		case 'F': return "46";
		case 'G': return "47";
		case 'H': return "48";
		case 'I': return "49";
		case 'J': return "4A";
		case 'K': return "4B";
		case 'L': return "4C";
		case 'M': return "4D";
		case 'N': return "4E";
		case 'O': return "4F";
		case 'P': return "50";
		case 'Q': return "51";
		case 'R': return "52";
		case 'S': return "53";
		case 'T': return "54";
		case 'U': return "55";
		case 'V': return "56";
		case 'W': return "57";
		case 'X': return "58";
		case 'Y': return "59";
		case 'Z': return "5A";
		}
	}

	return in;
}

std::string GetVanillaKeybind(InputBitflags bit) {
	if (bit == WeaponMenuBit || bit == WeaponMenu2Bit)
		return "32"; // "2"
	if (bit == HealBit)
		return "51"; // "Q"
	if (bit == TauntBit)
		return "31"; // "1"
	if (bit == JumpBit)
		return "20"; // " "
	if (bit == InteractBit)
		return "46"; // "F"
	if (bit == LightAttackBit)
		return "01"; // LMB
	if (bit == HeavyAttackBit)
		return "02"; // RMB
	if (bit == PauseBit)
		return "1B"; // Escape
	if (bit == CodecBit)
		return "33"; // "3"
	if (bit == SubWeaponBit)
		return "43"; // "C"
	if (bit == BladeModeBit)
		return "A0"; // LShift
	if (bit == AbilityBit)
		return "52"; // "R", technically not a 1:1 match because platinum
	if (bit == LockOnBit)
		return "45"; // "E"
	if (bit == RunBit)
		return "A2"; // LCtrl
	if (bit == CamResetBit)
		return "03"; // MMB
	if (bit == LeftBit)
		return "41"; // "A"
	if (bit == RightBit)
		return "44"; // "D"
	if (bit == ForwardBit)
		return "57"; // "W"
	if (bit == BackwardBit)
		return "53"; // "S"
	return "None"; // Camera is analog and won't really work here anyway
}

std::string GetVanillaKeybind(std::string Keybind) {
	if (Keybind == WeaponMenu || Keybind == WeaponMenu2)
		return "32"; // "2"
	if (Keybind == Heal)
		return "51"; // "Q"
	if (Keybind == Taunt)
		return "31"; // "1"
	if (Keybind == Jump)
		return "20"; // " "
	if (Keybind == Interact)
		return "46"; // "F"
	if (Keybind == NormalAttack)
		return "01"; // LMB
	if (Keybind == StrongAttack)
		return "02"; // RMB
	if (Keybind == Pause)
		return "1B"; // Escape
	if (Keybind == Pause2)
		return "33"; // "3"
	if (Keybind == Subweapon)
		return "43"; // "C"
	if (Keybind == BladeMode)
		return "A0"; // LShift
	if (Keybind == Ability)
		return "52"; // "R", technically not a 1:1 match because platinum
	if (Keybind == Lockon)
		return "45"; // "E"
	if (Keybind == Run)
		return "A2"; // LCtrl
	if (Keybind == CamReset)
		return "03"; // MMB
	if (Keybind == Left)
		return "41"; // "A"
	if (Keybind == Right)
		return "44"; // "D"
	if (Keybind == Forward)
		return "57"; // "W"
	if (Keybind == Back)
		return "53"; // "S"
	return "None"; // Camera is analog and won't really work here anyway
}

bool CheckControlPressed(int controllerNumber, std::string Keybind, std::string GamepadBind) {
	if (controllerNumber == -1) {
		Keybind = GetVanillaKeybind(Keybind);
		if (Keybind == "None")
			return false;
	}
	return ((controllerNumber <= 0 && (GetKeyState(std::stoi(Keybind, nullptr, 16)) & 0x8000))
		|| IsGamepadButtonPressed(controllerNumber, GamepadBind));
}

bool SetFlagsForAction(Pl0000* player, int controllerNumber, std::string Keybind, std::string GamepadBind,
					   InputBitflags bit, int* altField1 = nullptr, int* altField2 = nullptr) {
	if (CheckControlPressed(controllerNumber, Keybind, GamepadBind)) {
		player->m_nKeyHoldingFlag |= bit;
		player->m_nKeyPressedFlag |= bit;
		if (altField1) *altField1 |= bit;
		if (altField2) *altField2 |= bit;
		return true;
	}
	else {
		player->m_nKeyHoldingFlag &= ~bit;
		player->m_nKeyPressedFlag &= ~bit;
		if (altField1) *altField1 &= ~bit;
		if (altField2) *altField2 &= ~bit;
		return false;
	}
}

bool SetFlagsForAnalog(Pl0000* player, int controllerNumber, std::string Keybind, std::string GamepadBind,
					   InputBitflags bit, float* altField, bool invert) {
	if (CheckControlPressed(controllerNumber, Keybind, GamepadBind)) {
		player->m_nKeyHoldingFlag |= bit;
		*altField = invert ? -1000.0f : 1000.0f;
		if (IsGamepadButtonPressed(controllerNumber, GamepadBind))
			*altField *= GetGamepadAnalog(controllerNumber, GamepadBind);
		return true;
	}
	return false;
}

void LoadControl(CIniReader iniReader, std::string* Control, std::string* GamepadControl, std::string name) {
	*Control = TryParseVKToHex(iniReader.ReadString("MGRRMultiplayerControls", name, *Control));
	*GamepadControl = iniReader.ReadString("MGRRMultiplayerControls", "Gamepad" + name, *GamepadControl);
}


void LoadConfig() noexcept
{

	CIniReader iniReader("MGRRMultiplayerControls.ini");

	LoadControl(iniReader, &Forward, &GamepadForward, "Forward");
	LoadControl(iniReader, &Back, &GamepadBack, "Back");
	LoadControl(iniReader, &Left, &GamepadLeft, "Left");
	LoadControl(iniReader, &Right, &GamepadRight, "Right");
	LoadControl(iniReader, &NormalAttack, &GamepadNormalAttack, "NormalAttack");
	LoadControl(iniReader, &StrongAttack, &GamepadStrongAttack, "StrongAttack");
	LoadControl(iniReader, &Jump, &GamepadJump, "Jump");
	LoadControl(iniReader, &Interact, &GamepadInteract, "Interact");
	LoadControl(iniReader, &Run, &GamepadRun, "Run");
	LoadControl(iniReader, &BladeMode, &GamepadBladeMode, "BladeMode");
	LoadControl(iniReader, &Lockon, &GamepadLockon, "Lockon");
	LoadControl(iniReader, &Subweapon, &GamepadSubweapon, "Subweapon");
	LoadControl(iniReader, &CamUp, &GamepadCamUp, "CamUp");
	LoadControl(iniReader, &CamDown, &GamepadCamDown, "CamDown");
	LoadControl(iniReader, &CamLeft, &GamepadCamLeft, "CamLeft");
	LoadControl(iniReader, &CamRight, &GamepadCamRight, "CamRight");
	LoadControl(iniReader, &Taunt, &GamepadTaunt, "Taunt");
	LoadControl(iniReader, &WeaponMenu, &GamepadWeaponMenu, "WeaponMenu");
	LoadControl(iniReader, &WeaponMenu2, &GamepadWeaponMenu2, "WeaponMenu2");
	LoadControl(iniReader, &Heal, &GamepadHeal, "Heal");
	LoadControl(iniReader, &Pause, &GamepadPause, "Pause");
	LoadControl(iniReader, &Pause2, &GamepadPause2, "Codec"); // Note non-matching, Codec is apparently an enum somewhere
	LoadControl(iniReader, &Ability, &GamepadAbility, "Ability");
	LoadControl(iniReader, &CamReset, &GamepadCamReset, "CamReset");

	GamepadSpawnSam = iniReader.ReadString("MGRRMultiplayerControls", "GamepadSpawnSam", GamepadSpawnSam);
	GamepadSpawnWolf = iniReader.ReadString("MGRRMultiplayerControls", "GamepadSpawnWolf", GamepadSpawnWolf);
	GamepadSpawnBossSam = iniReader.ReadString("MGRRMultiplayerControls", "GamepadSpawnBossSam", GamepadSpawnBossSam);
	GamepadSpawnArmstrong = iniReader.ReadString("MGRRMultiplayerControls", "GamepadSpawnArmstrong", GamepadSpawnArmstrong);

	DisableNumberBinds = iniReader.ReadBoolean("MGRRMultiplayerControls", "DisableNumberBinds", false);

}

void RecalibrateBossCode() {
	if (PlayAsArmstrong)
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x909090, true);
	else
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0xFFD00EE8, true);

	if (PlayAsSam) {
		injector::WriteMemory<unsigned int>(shared::base + 0x39C32, 0x909090, true);
		injector::WriteMemory<unsigned int>(shared::base + 0x39CC5, 0x909090, true);
	}
	else {
		injector::WriteMemory<unsigned int>(shared::base + 0x39C32, 0xFFEE49E8, true);
		injector::WriteMemory<unsigned int>(shared::base + 0x39CC5, 0xFFEE49E8, true);
	}

	/*if (PlayAsSundowner)
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x90909090, true);
	else
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x909090, true);

	if (PlayAsMonsoon)
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x90909090, true);
	else
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x909090, true);

	if (PlayAsMistral)
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x90909090, true);
	else
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x909090, true);*/
}

void TeleportToMainPlayer(Pl0000* MainPlayer, int controllerIndex = -1) {
	cVec4& pos = MainPlayer->m_vecTransPos;
	cVec4& rot = MainPlayer->m_vecRotation;
	if (controllerIndex >= 0) {
		Pl0000* player = players[controllerIndex + 1];
		player->place(pos, rot);
		return;
	}
	for (Pl0000* player : players) {
		if (player)
			player->place(pos, rot);
	}
}


struct KeyState {
	bool isPressed;
	bool isHolding;
};

KeyState getKeyState(int virtualKeyCode) {
	KeyState keyState;
	static bool keyHoldingStates[256] = { false }; // Массив для отслеживания состояния удержания клавиш

	bool isKeyDown = (GetKeyState(virtualKeyCode) & 0x8000) != 0;

	keyState.isPressed = isKeyDown && !keyHoldingStates[virtualKeyCode];
	keyState.isHolding = isKeyDown;

	keyHoldingStates[virtualKeyCode] = isKeyDown;

	return keyState;
}


bool handleKeyPress(int hotKey, bool* isMenuShowPtr) {
	static bool wasHotKeyPressed = false; // Статическая переменная, сохраняет состояние между вызовами

	if (GetKeyState(hotKey) & 0x8000) {
		if (!wasHotKeyPressed) {
			*isMenuShowPtr = !(*isMenuShowPtr);
			wasHotKeyPressed = true;
		}
	}
	else {
		wasHotKeyPressed = false;
	}
	return *isMenuShowPtr; // Возвращаем текущее состояние меню
}

void UpdateBossActions(BehaviorEmBase* Enemy, unsigned int BossActions[], int controllerNumber = -1) {

	if (CheckControlPressed(controllerNumber, NormalAttack, GamepadNormalAttack)) { // Two punches (four strikes)
		Enemy->setState(BossActions[0], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, StrongAttack, GamepadStrongAttack)) { // Two punches, kick, punch (four strike w/ sheath)
		Enemy->setState(BossActions[1], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Run, GamepadRun)) { // Run QTE (Assault Rush)
		Enemy->setState(BossActions[2], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Interact, GamepadInteract)) { // Overhead with AOE (unblockable QTE)
		Enemy->setState(BossActions[3], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Jump, GamepadJump)) { // Uppercut (perfect parry QTE fail)
		Enemy->setState(BossActions[4], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Taunt, GamepadTaunt)) { // Explode (un-perfect-parryable four strike)
		Enemy->setState(BossActions[5], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, BladeMode, GamepadBladeMode)) { // Heal (taunt)
		Enemy->setState(BossActions[6], 0, 0, 0);
	}
}

void Update()
{


	if (!configLoaded) {

		injector::WriteMemory<unsigned short>(shared::base + 0x69A516, 0x9090, true); // F3 A5 // Disable normal input Sam and Wolf
		injector::WriteMemory<unsigned int>(shared::base + 0x9DB430, 0x909090, true); // E8 1B FF FF FF // Disable normal controller input
		injector::MakeNOP(shared::base + 0x69E313, 6, true); // Remove need for custom pl1400 and pl1500
		//injector::WriteMemory<unsigned char>(shared::base + 0x6C7EC3, 0xEB, true); // Disable vanilla enemy targeting (broken)

		LoadConfig();
		configLoaded = true;
	}

	Pl0000* MainPlayer = cGameUIManager::Instance.m_pPlayer;



	if (!MainPlayer) {
		for (int i = 0; i < 5; i++) {
			players[i] = nullptr;
			playerTypes[i] = (eObjID)0;
		}
		return;
	}

	// if (MainPlayer) // early return removes need for this indent
	players[0] = MainPlayer;
	playerTypes[0] = MainPlayer->m_pEntity->m_nEntityIndex;

	if (EnableDamageToPlayers)
		MainPlayer->field_640 = 0;
	else
		MainPlayer->field_640 = 1;

	if (!isPlayerAtOnce) {
		for (int itemToRequest = 0x11400; itemToRequest <= 0x11405; itemToRequest++) {
			cObjReadManager::Instance.requestWork((eObjID)itemToRequest, 0);
		}
		for (int itemToRequest = 0x11500; itemToRequest <= 0x11506; itemToRequest++) {
			cObjReadManager::Instance.requestWork((eObjID)itemToRequest, 0);
		}

		cObjReadManager::Instance.requestWork((eObjID)0x3D070, 0); // Sam projectile

		isPlayerAtOnce = true;
	}


	modelItems = injector::ReadMemory<ModelItems*>(shared::base + 0x17EA01C, true);


	//originalSword = injector::ReadMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, true);

	if (modelItems) {
		if (modelItems->m_nHair != 0x11401) originalModelItems.m_nHair = modelItems->m_nHair;
		if (modelItems->m_nVisor != 0x11402) originalModelItems.m_nVisor = modelItems->m_nVisor;
		if (modelItems->m_nSheath != 0x11404) originalModelItems.m_nSheath = modelItems->m_nSheath;
		if (modelItems->m_nHead != 0x11405) originalModelItems.m_nHead = modelItems->m_nHead;

		if (getKeyState('6').isPressed && !DisableNumberBinds) {
			Spawner((eObjID)0x11400);
			//camera back to Raiden
			((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
		}

		if (getKeyState('5').isPressed && !DisableNumberBinds) {
			Spawner((eObjID)0x11500);
			//injector::WriteMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, 0x11501, true);
			//camera back to Raiden
			((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
		}

		for (int i = 0; i < 4; i++) {
			if (playerTypes[i + 1]) continue;

			if (IsGamepadButtonPressed(i, GamepadSpawnSam))
				Spawner((eObjID)0x11400, i);
			else if (IsGamepadButtonPressed(i, GamepadSpawnWolf))
				Spawner((eObjID)0x11500, i);
			else if (IsGamepadButtonPressed(i, GamepadSpawnArmstrong)) {
				Spawner((eObjID)0x20700, i);
				PlayAsArmstrong = true;
			}
			else if (IsGamepadButtonPressed(i, GamepadSpawnBossSam)) {
				Spawner((eObjID)0x20020, i);
				PlayAsSam = true;
			}
			else {
				continue;
			}
			RecalibrateBossCode();
			//camera back to Raiden
			((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
		}
	}

	if ((GetKeyState('7') & 0x8000) || (GetKeyState('T') & 0x8000))
		((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);

	Hw::cVec4* matrix = (Hw::cVec4*)&cCameraGame::Instance.m_TranslationMatrix;

	auto& pos = matrix[0];
	auto& rotate = matrix[1];


	for (auto node = EntitySystem::Instance.m_EntityList.m_pFirst; node != EntitySystem::Instance.m_EntityList.m_pEnd; node = node->m_next) {

		auto value = node->m_value;
		if (!value) continue;

		auto player = node->m_value->getEntityInstance<Pl0000>();
		if (!player) continue;

		bool alreadyInit = false;
		for (int i = 0; i < 5; i++) {
			if (players[i] == player)
				alreadyInit = true;
		}
		if (alreadyInit) continue;

		for (int i = 0; i < 5; i++) {
			if (playerTypes[i] && !players[i] && value->m_nEntityIndex == playerTypes[i]) {
				players[i] = player;
				break;
			}
		}

	}

	for (int i = 0; i < 5; i++) {
		Pl0000* player = players[i];
		if (!player) continue;

		BehaviorEmBase* Enemy = (BehaviorEmBase*)player;
		int controllerNumber = i - 1;

		if (((Enemy->m_pEntity->m_nEntityIndex == 0x20700 || Enemy->m_pEntity->m_nEntityIndex == 0x2070A) && (PlayAsArmstrong))

			|| (Enemy->m_pEntity->m_nEntityIndex == 0x20020 && (PlayAsSam))
			) {


			if (Enemy->m_pEntity->m_nEntityIndex == 0x20700 || Enemy->m_pEntity->m_nEntityIndex == 0x2070A && PlayAsArmstrong) {
				unsigned int BossActions[] = { 0x20000, 0x20003, 0x20007, 0x20006, 0x20001, 0x20009, 0x20010 };
				UpdateBossActions(Enemy, BossActions, controllerNumber);

				if (ArmstrongCanDamagePlayer)
					Enemy->field_640 = 2;
				else
					Enemy->field_640 = 1;
			}

			if (Enemy->m_pEntity->m_nEntityIndex == 0x20020 && PlayAsSam) {
				unsigned int BossActions[] = { 0x30004, 0x30006, 0x30007, 0x30014, 0x3001C, 0x30005, 0x10006 };
				UpdateBossActions(Enemy, BossActions, controllerNumber);

				if (BossSamCanDamagePlayer)
					Enemy->field_640 = 2;
				else
					Enemy->field_640 = 1;
			}





			float rotation = 0;
			float v9;
			float field_X = 0;
			float field_Y = 0;
			float v10;
			float v11;
			cVec4 v54;
			long double tann = 0;
			//forward

			cCameraGame camera;
			//UpdateMovement(Enemy, &camera);

			unsigned int StandingState = 0x10000;
			if (Enemy->m_pEntity->m_nEntityIndex == 0x20020) StandingState = 0x10000; // TODO: find correct state

			unsigned int WalkingState = 0x10001;
			if (Enemy->m_pEntity->m_nEntityIndex == 0x20020) WalkingState = 0x10002;


			if (CheckControlPressed(controllerNumber, Forward, GamepadForward)) {
				if (Enemy->m_nCurrentAction == StandingState) Enemy->setState(WalkingState, 0, 0, 0);
				field_Y = -1000;
				if (IsGamepadButtonPressed(controllerNumber, GamepadForward))
					field_Y *= GetGamepadAnalog(controllerNumber, GamepadForward);
			}




			//back
			if (CheckControlPressed(controllerNumber, Back, GamepadBack)) {
				//if (GetAsyncKeyState(0x4B)) {
				if (Enemy->m_nCurrentAction == StandingState) Enemy->setState(WalkingState, 0, 0, 0);
				field_Y = 1000;
				if (IsGamepadButtonPressed(controllerNumber, GamepadBack))
					field_Y *= GetGamepadAnalog(controllerNumber, GamepadBack);
			}

			//left
			if (CheckControlPressed(controllerNumber, Left, GamepadLeft)) {
				if (Enemy->m_nCurrentAction == StandingState) Enemy->setState(WalkingState, 0, 0, 0);
				field_X = -1000;
				if (IsGamepadButtonPressed(controllerNumber, GamepadLeft))
					field_X *= GetGamepadAnalog(controllerNumber, GamepadLeft);
			}

			//right
			if (CheckControlPressed(controllerNumber, Right, GamepadRight)) {
				if (Enemy->m_nCurrentAction == StandingState) Enemy->setState(WalkingState, 0, 0, 0);
				field_X = 1000;
				if (IsGamepadButtonPressed(controllerNumber, GamepadRight))
					field_X *= GetGamepadAnalog(controllerNumber, GamepadRight);
			}

			if (field_X != 0 || field_Y != 0) {
				float angle = atan2(field_X, field_Y);
				Enemy->m_vecRotation.y = angle;
			}
			else {
				if (Enemy->m_nCurrentAction == WalkingState) Enemy->setState(StandingState, 0, 0, 0);
			}


			//v9 = field_X * field_X + field_Y * field_Y;




			//D3DXMATRIX matrix;
			//D3DXMatrixIdentity(matrix);

			//((void(__thiscall*)(BehaviorEmBase* Enemy, D3DXMATRIX* matrix))(shared::base + 0x77A260))(Enemy, &matrix);

		/*	const D3DXMATRIX* ConstMatrix;

			//ConstMatrix = matrix;

			if (v9 > 0.0)
			{
				v10 = -field_X;
				v11 = -field_Y;
				//Enemy->field_D2C = atan2(v10, v11);
				v54.x = v10;
				v54.y = v11;
				v54.z = 0.0;
				D3DXVECTOR3 vector;
				vector.x = v54.x;
				vector.y = v54.y;
				vector.z = v54.z;
				//D3DXVec3TransformNormal(vector,vector, matrix);
				//((D3DXVECTOR3*(__cdecl*)(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, const D3DXMATRIX* pM))(shared::base + 0x1036F2C))(&vector,&vector,&matrix);
				//D3DXVec3TransformNormal(&vector,&vector, &matrix);
				v54.x = Enemy->m_vecTransPos.x + v54.x;
				v54.y = Enemy->m_vecTransPos.y + v54.y;
				v54.z = Enemy->m_vecTransPos.z + v54.z;
				v54.w = Enemy->m_vecTransPos.w + v54.w;
				Enemy->m_vecRotation.y = ((long double(__thiscall*)(Behavior* Enemy, cVec4 * vec4))(shared::base + 0x68EC30))(Enemy, &v54);
			}


			/*player->field_D30 = player->m_vecRotation.y;
			*/

		}
		

		if ((player->m_pEntity->m_nEntityIndex == (eObjID)0x11400 || player->m_pEntity->m_nEntityIndex == (eObjID)0x11500)
			&& modelItems) {
			modelItems->m_nHair = originalModelItems.m_nHair;
			modelItems->m_nVisor = originalModelItems.m_nVisor;
			modelItems->m_nSheath = originalModelItems.m_nSheath;
			modelItems->m_nHead = originalModelItems.m_nHead;

			if (EnableDamageToPlayers)
				player->field_640 = 0;
			else
				player->field_640 = 1;

			player->m_nKeyHoldingFlag = 0;
			player->field_D0C = 0;
			player->field_D08 = 0;
			player->field_D10 = 0;
			player->field_D14 = 0;

			static bool wasJumpSam[5] = { false }; // For Sam, whether jump was already pressed on the previous frame
			static bool wasCrouchWolf[5] = { false }; // For Wolf, whether Ability was already pressed


			//injector::WriteMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, originalSword, true);


			if (getKeyState('9').isPressed && !DisableNumberBinds) {
				AutoNormalAttackEnable = !AutoNormalAttackEnable;
				AutoStrongAttackEnable = false;
			}

			if (getKeyState('0').isPressed && !DisableNumberBinds) {
				AutoStrongAttackEnable = !AutoStrongAttackEnable;
				AutoNormalAttackEnable = false;
			}

			//Change camera between players

			if ((GetKeyState('8') & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadLockon))
				((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(player);

			if ((GetKeyState(std::stoi(Pause2, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadPause2))
				TeleportToMainPlayer(MainPlayer, controllerNumber);


			bool isAny = false;

			// Special cases where keyPressedFlag is important
			if (player->m_nModelIndex == 0x11400) {
				if (wasJumpSam[i]) {
					wasJumpSam[i] = SetFlagsForAction(player, controllerNumber, Jump, GamepadJump, JumpBit, &player->field_D00, &player->field_D04);
					player->m_nKeyPressedFlag &= ~JumpBit;
				}
				else {
					wasJumpSam[i] = SetFlagsForAction(player, controllerNumber, Jump, GamepadJump, JumpBit, &player->field_D00, &player->field_D04);
				}
				isAny |= wasJumpSam[i];
			}
			else {
				isAny |= SetFlagsForAction(player, controllerNumber, Jump, GamepadJump, JumpBit, &player->field_D00, &player->field_D04);
			}

			if (player->m_nModelIndex == 0x11500) {
				if (wasCrouchWolf[i]) {
					wasCrouchWolf[i] = SetFlagsForAction(player, controllerNumber, Ability, GamepadAbility, AbilityBit);
					player->m_nKeyPressedFlag &= ~AbilityBit;
				}
				else {
					wasCrouchWolf[i] = SetFlagsForAction(player, controllerNumber, Ability, GamepadAbility, AbilityBit);
				}
				isAny |= wasCrouchWolf[i];
			}
			else {
				isAny |= SetFlagsForAction(player, controllerNumber, Ability, GamepadAbility, AbilityBit);
			}

			// Left stick
			isAny |= SetFlagsForAnalog(player, controllerNumber, Forward, GamepadForward, ForwardBit, &player->field_D0C, true);
			isAny |= SetFlagsForAnalog(player, controllerNumber, Back, GamepadBack, BackwardBit, &player->field_D0C, false);
			isAny |= SetFlagsForAnalog(player, controllerNumber, Left, GamepadLeft, LeftBit, &player->field_D08, true);
			isAny |= SetFlagsForAnalog(player, controllerNumber, Right, GamepadRight, RightBit, &player->field_D08, false);
			// Right stick
			isAny |= SetFlagsForAnalog(player, controllerNumber, CamUp, GamepadCamUp, CamUpBit, &player->field_D14, true);
			isAny |= SetFlagsForAnalog(player, controllerNumber, CamDown, GamepadCamDown, CamDownBit, &player->field_D14, false);
			isAny |= SetFlagsForAnalog(player, controllerNumber, CamLeft, GamepadCamLeft, CamLeftBit, &player->field_D10, true);
			isAny |= SetFlagsForAnalog(player, controllerNumber, CamRight, GamepadCamRight, CamRightBit, &player->field_D10, false);
			// Face buttons
			isAny |= SetFlagsForAction(player, controllerNumber, NormalAttack, GamepadNormalAttack, LightAttackBit, &player->field_D04);
			if (AutoNormalAttackEnable) {
				player->m_nKeyHoldingFlag |= LightAttackBit;
				player->m_nKeyPressedFlag |= LightAttackBit;
				player->field_D04 |= LightAttackBit;
			}
			isAny |= SetFlagsForAction(player, controllerNumber, StrongAttack, GamepadStrongAttack, HeavyAttackBit, &player->field_D04);
			if (AutoStrongAttackEnable) {
				player->m_nKeyHoldingFlag |= HeavyAttackBit;
				player->m_nKeyPressedFlag |= HeavyAttackBit;
				player->field_D04 |= HeavyAttackBit;
			}
			isAny |= SetFlagsForAction(player, controllerNumber, Interact, GamepadInteract, InteractBit);
			// Triggers and bumpers (lock-on takes camera control instead)
			isAny |= SetFlagsForAction(player, controllerNumber, Run, GamepadRun, RunBit, &player->field_D00, &player->field_D04);
			isAny |= SetFlagsForAction(player, controllerNumber, BladeMode, GamepadBladeMode, BladeModeBit, &player->field_D00, &player->field_D04);
			isAny |= SetFlagsForAction(player, controllerNumber, Subweapon, GamepadSubweapon, SubWeaponBit);
			// D-pad
			isAny |= SetFlagsForAction(player, controllerNumber, Taunt, GamepadTaunt, TauntBit);
			isAny |= SetFlagsForAction(player, controllerNumber, Heal, GamepadHeal, HealBit); // Plays effect, does not heal
			//isAny |= SetFlagsForAction(player, controllerNumber, WeaponMenu, GamepadWeaponMenu, WeaponMenuBit);
			//isAny |= SetFlagsForAction(player, controllerNumber, WeaponMenu2, GamepadWeaponMenu2, WeaponMenu2Bit);
			// Other
			isAny |= SetFlagsForAction(player, controllerNumber, CamReset, GamepadCamReset, CamResetBit);
			//isAny |= SetFlagsForAction(player, controllerNumber, Pause, GamepadPause, PauseBit); // Does not work
			//isAny |= SetFlagsForAction(player, controllerNumber, Pause2, GamepadPause2, CodecBit); // Non-Raidens don't have codec

			if (!isAny) {
				player->m_nKeyPressedFlag = 0;
				player->field_D04 = 0;
			}

		}
	}


}


class Plugin
{
public:
	static inline void InitGUI()
	{
		Events::OnDeviceReset.before += gui::OnReset::Before;
		Events::OnDeviceReset.after += gui::OnReset::After;
		Events::OnEndScene += gui::OnEndScene; 
		/* // Or if you want to switch it to Present
		Events::OnPresent += gui::OnEndScene;
		*/

		Events::OnTickEvent += []()
			{
				Update();
			};
	}

	Plugin()
	{
		InitGUI();
	}
} plugin;




void gui::RenderWindow()
{
	isMenuShow = handleKeyPress(HotKey, &isMenuShow);

	static bool paused = false;

	if (isMenuShow && g_GameMenuStatus == InGame)
	{
		Trigger::StaFlags.STA_PAUSE = true;
		paused = true;
	}

	if (!isMenuShow && paused && g_GameMenuStatus == InGame)
	{
		Trigger::StaFlags.STA_PAUSE = false;
		paused = false;
	}

	if (!isMenuShow)
		Trigger::StpFlags.STP_GAME_UPDATE = false;

	if (isMenuShow) {

		//Trigger::StpFlags.STP_MOUSE_UPDATE = isMenuShow && g_GameMenuStatus == InGame;
		//Trigger::StpFlags.STP_PL_CAM_KEY = isMenuShow && g_GameMenuStatus == InGame;
		//Trigger::StpFlags.STP_PL_ATTACK_KEY = isMenuShow && g_GameMenuStatus == InGame;

		Trigger::StpFlags.STP_GAME_UPDATE = g_GameMenuStatus == 1;
		ImGui::Begin("Multiplayer mod", NULL, ImGuiWindowFlags_NoCollapse);
		ImGui::SetNextWindowSize({ 900, 600 });
		ImGuiIO io = ImGui::GetIO();
		io.MouseDrawCursor = true;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		if (ImGui::BeginTabBar("##WindowTab", ImGuiTabBarFlags_NoTooltip))
		{
			if (ImGui::BeginTabItem("Main Menu"))
			{
				Pl0000* MainPlayer = cGameUIManager::Instance.m_pPlayer;

				if (ImGui::Button("Spawn Sam as next player") && MainPlayer) {
					Spawner((eObjID)0x11400);
					//camera back to Raiden
					((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
				}


				if (ImGui::Button("Spawn Wolf as next player") && MainPlayer) {
					Spawner((eObjID)0x11500);
					//camera back to Raiden
					((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
				}

				ImGui::Checkbox("Armstrong is player-controlled", &PlayAsArmstrong);
				ImGui::Checkbox("Armstrong can damage player", &ArmstrongCanDamagePlayer);
				if (ImGui::Button("Spawn Armstrong as next player") && MainPlayer) {
					Spawner((eObjID)0x20700);
					PlayAsArmstrong = true;
				}

				ImGui::Checkbox("Boss Sam is player-controlled", &PlayAsSam);
				ImGui::Checkbox("Boss Sam can damage player", &BossSamCanDamagePlayer);
				if (ImGui::Button("Spawn boss Sam as next player") && MainPlayer) {
					Spawner((eObjID)0x20020);
					PlayAsSam = true;
				}


				ImGui::Checkbox("Allow damage to another player", &EnableDamageToPlayers);
				// Debug print Sam's flags
//#define PRINTSAM
//#define PRINTENEMY
				
				for (auto node = EntitySystem::Instance.m_EntityList.m_pFirst; node != EntitySystem::Instance.m_EntityList.m_pEnd; node = node->m_next) {

					auto value = node->m_value;
					if (!value) continue;
#ifdef PRINTSAM
					auto player = value->getEntityInstance<Pl0000>();
					if (!player) continue;
					
					if (player->m_pEntity->m_nEntityIndex == (eObjID)0x11400) {
						ImGui::InputInt("La li lu le lo", &player->m_nKeyHoldingFlag, 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
						ImGui::InputInt("pressed flag", &player->m_nKeyPressedFlag, 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
						ImGui::InputInt("D00", &player->field_D00, 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
						ImGui::InputInt("D04", &player->field_D04, 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
					}
#endif
#ifdef PRINTENEMY
					auto Enemy = value->getEntityInstance<BehaviorEmBase>();
					if (!Enemy) continue;
					if ((value->m_nEntityIndex & 0xF0000) == 0x20000) {
						//ImGui::InputInt("the fuck?", (int*)&node->m_value->m_nEntityIndex, 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
						int twerpsenemyflag = (int)&Enemy->m_pEnemy;
						ImGui::InputInt("twerp's enemy flag", &twerpsenemyflag, 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
						ImGui::InputInt("Some twerp", (int*)&(Enemy->m_pEnemy->m_pEntity->m_nEntityIndex), 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
					}
#endif
				}


				
				//ImGui::Checkbox("Player 2 is Sundowner", &PlayAsSundowner);
				//ImGui::Checkbox("Player 2 is Monsoon", &PlayAsMonsoon);
				//ImGui::Checkbox("Player 2 is Mistral", &PlayAsMistral);

				RecalibrateBossCode();


				if (ImGui::Button("Teleport all players to Raiden") && MainPlayer) {

					TeleportToMainPlayer(MainPlayer);

				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Current Players")) {
				ImGui::Text("Player 1 (keyboard): %x\n", playerTypes[0]);
				ImGui::Text("Player 2 (keyboard/controller): %x\n", playerTypes[1]);
				ImGui::Text("Player 3 (controller): %x\n", playerTypes[2]);
				ImGui::Text("Player 4 (controller): %x\n", playerTypes[3]);
				ImGui::Text("Player 5 (controller): %x\n", playerTypes[4]);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
		ImGui::EndFrame();
		ImGui::Render();
	}
}