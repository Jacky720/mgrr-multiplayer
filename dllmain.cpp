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
bool SamSpawned = false;
bool WolfSpawned = false;

bool AutoNormalAttackEnable = false;
bool AutoStrongAttackEnable = false;


bool ArmstrongAtOnce = false;
bool SundownerAtOnce = false;
bool MistralAtOnce = false;
bool MonsoonAtOnce = false;
bool SamAtOnce = false;


bool PlayAsArmstrong = false;
bool ArmstrongCanDamagePlayer = true;

bool BossSamCanDamagePlayer = true;

bool PlayAsMistral = false;
bool PlayAsMonsoon = false;
bool PlayAsSundowner = false;
bool PlayAsSam = false;


bool EnableDamageToPlayers = false;

unsigned int sword = 0x0;
unsigned int originalSword = 0x0;


bool isMenuShow = false;


eObjID mObjId = (eObjID)0x0;

unsigned int HotKey = 0x2D; //Hotkey for menu show


std::string Forward = "26";
std::string Back = "28";
std::string Left = "25";
std::string Right = "27";
std::string NormalAttack = "49";
std::string StrongAttack = "4F";
std::string Jump = "50";
std::string Run = "55";
std::string Taunt = "59";
std::string GamepadForward = "XINPUT_GAMEPAD_LEFT_THUMB_UP";
std::string GamepadBack = "XINPUT_GAMEPAD_LEFT_THUMB_DOWN";
std::string GamepadLeft = "XINPUT_GAMEPAD_LEFT_THUMB_LEFT";
std::string GamepadRight = "XINPUT_GAMEPAD_LEFT_THUMB_RIGHT";
std::string GamepadNormalAttack = "XINPUT_GAMEPAD_A";
std::string GamepadStrongAttack = "XINPUT_GAMEPAD_B";
std::string GamepadJump = "XINPUT_GAMEPAD_X";
std::string GamepadRun = "XINPUT_GAMEPAD_Y";
std::string GamepadTaunt = "XINPUT_GAMEPAD_RIGHT_THUMB_UP";



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
	unsigned int key_forward = 0x57;
	unsigned int key_back = 0x53;
	unsigned int key_left = 0x41;
	unsigned int key_right = 0x44;
	unsigned int key_slowWalking = 0x9;
	unsigned int key_jump = 0x20;
	unsigned int key_normalAttack = 0x80000001;
	unsigned int key_strongAttack = 0x80000002;
	unsigned int key_bladeMode = 0x9F;
	unsigned int key_run = 0x9B;
	unsigned int key_action = 0x46;
	unsigned int key_ripperMode = 0x52;
	unsigned int key_target = 0x45;
	unsigned int key_useSecondaryWeapon = 0x56;
	unsigned int key_useItem = 0x51;
	unsigned int key_visor = 0x31;
	unsigned int key_weaponSelect = 0x32;
	unsigned int key_codec = 0x33;
	unsigned int key_pause = 0x91;
	unsigned int key_finish = 0x91;
	unsigned int key_dodge = 0x80000004;

};

struct Keys {
	unsigned int m_nKeys[28];
};

Keys* keys = injector::ReadMemory<Keys*>(shared::base + 0x177B7C0, true);

unsigned int keyArray[] = {
	0x57, 0x53, 0x41, 0x44, 0x9, 0x20, 0x80000001, 0x80000002, 0x9F,
	0x9B, 0x46, 0x52, 0x45, 0x56
};

Player1_Keys pl1Keys;




KeysStruct currentKey;


bool isPlayerAtOnce = false;


void Spawner(eObjID id) {

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


bool IsGamepadButtonPressed(int controllerIndex, const std::string& button)
{
	XINPUT_STATE state;
	DWORD dwResult = XInputGetState(controllerIndex, &state);


	if (dwResult == ERROR_SUCCESS)
	{
		if (button == "XINPUT_GAMEPAD_A" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_B" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_X" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_Y" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_LEFT_SHOULDER" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_RIGHT_SHOULDER" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_BACK" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_START" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_DPAD_UP" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_DPAD_DOWN" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_DPAD_LEFT" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_DPAD_RIGHT" && (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0)
			return true;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_UP" && state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			return true;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_DOWN" && state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			return true;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_LEFT" && state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			return true;
		else if (button == "XINPUT_GAMEPAD_LEFT_THUMB_RIGHT" && state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			return true;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_UP" && state.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			return true;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_DOWN" && state.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			return true;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_LEFT" && state.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			return true;
		else if (button == "XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT" && state.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			return true;
	}

	return false;
}

void LoadConfig() noexcept
{
	injector::WriteMemory<unsigned short>(shared::base + 0x69A516, 0x9090, true);

	CIniReader iniReader("MGRRMultiplayerControls.ini");


	Forward = iniReader.ReadString("MGRRMultiplayerControls", "Forward", "26");
	Back = iniReader.ReadString("MGRRMultiplayerControls", "Back", "28");
	Left = iniReader.ReadString("MGRRMultiplayerControls", "Left", "25");
	Right = iniReader.ReadString("MGRRMultiplayerControls", "Right", "27");
	NormalAttack = iniReader.ReadString("MGRRMultiplayerControls", "NormalAttack", "49");
	StrongAttack = iniReader.ReadString("MGRRMultiplayerControls", "StrongAttack", "4F");
	Jump = iniReader.ReadString("MGRRMultiplayerControls", "Jump", "50");
	Run = iniReader.ReadString("MGRRMultiplayerControls", "Run", "55");
	Taunt = iniReader.ReadString("MGRRMultiplayerControls", "Taunt", "59");
	GamepadForward = iniReader.ReadString("MGRRMultiplayerControls", "GamepadForward", "XINPUT_GAMEPAD_LEFT_THUMB_UP");
	GamepadBack = iniReader.ReadString("MGRRMultiplayerControls", "GamepadBack", "XINPUT_GAMEPAD_LEFT_THUMB_DOWN");
	GamepadLeft = iniReader.ReadString("MGRRMultiplayerControls", "GamepadLeft", "XINPUT_GAMEPAD_LEFT_THUMB_LEFT");
	GamepadRight = iniReader.ReadString("MGRRMultiplayerControls", "GamepadRight", "XINPUT_GAMEPAD_LEFT_THUMB_RIGHT");
	GamepadNormalAttack = iniReader.ReadString("MGRRMultiplayerControls", "GamepadNormalAttack", "XINPUT_GAMEPAD_X");
	GamepadStrongAttack = iniReader.ReadString("MGRRMultiplayerControls", "GamepadStrongAttack", "XINPUT_GAMEPAD_Y");
	GamepadJump = iniReader.ReadString("MGRRMultiplayerControls", "GamepadJump", "XINPUT_GAMEPAD_A");
	GamepadRun = iniReader.ReadString("MGRRMultiplayerControls", "GamepadRun", "XINPUT_GAMEPAD_B");
	GamepadTaunt = iniReader.ReadString("MGRRMultiplayerControls", "GamepadTaunt", "XINPUT_GAMEPAD_RIGHT_THUMB_UP");


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

void UpdateBossActions(BehaviorEmBase* Enemy, unsigned int BossActions[]) {
	
	int controllerNumber = 0;

	if ((GetKeyState(0x61) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadNormalAttack)) {
		Enemy->setState(BossActions[0], 0, 0, 0);
	}

	if ((GetKeyState(0x62) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadStrongAttack)) {
		Enemy->setState(BossActions[1], 0, 0, 0);
	}

	if ((GetKeyState(0x63) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadRun)) {
		Enemy->setState(BossActions[2], 0, 0, 0);
	}

	if ((GetKeyState(0x64) & 0x8000)) {
		Enemy->setState(BossActions[3], 0, 0, 0);
	}

	if ((GetKeyState(0x65) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadJump)) {
		Enemy->setState(BossActions[4], 0, 0, 0);
	}

	if ((GetKeyState(0x66) & 0x8000)) {
		Enemy->setState(BossActions[5], 0, 0, 0);
	}

	if ((GetKeyState(0x67) & 0x8000)) {
		Enemy->setState(BossActions[6], 0, 0, 0);
	}
}

void Update()
{
	

	if (!configLoaded) {



		injector::WriteMemory<unsigned short>(shared::base + 0x69A516, 0x9090, true);
		injector::WriteMemory<unsigned int>(shared::base + 0x9DB430, 0x909090, true);

		LoadConfig();
		configLoaded = true;
	}

	Pl0000* MainPlayer = cGameUIManager::Instance.m_pPlayer;



	if (!MainPlayer) {
		WolfSpawned = false;
		SamSpawned = false;
		ArmstrongAtOnce = false;
		SamAtOnce = false;
	}

	if (MainPlayer) {


		if (EnableDamageToPlayers) 
			MainPlayer->field_640 = 0;
		else 
			MainPlayer->field_640 = 1;

		if (!isPlayerAtOnce) {
			cObjReadManager::Instance.requestWork((eObjID)0x11400, 0);
			cObjReadManager::Instance.requestWork((eObjID)0x11401, 0);
			cObjReadManager::Instance.requestWork((eObjID)0x11402, 0);
			cObjReadManager::Instance.requestWork((eObjID)0x11403, 0);
			cObjReadManager::Instance.requestWork((eObjID)0x11404, 0);
			cObjReadManager::Instance.requestWork((eObjID)0x11405, 0);


			cObjReadManager::Instance.requestWork((eObjID)0x11500, 0);
			cObjReadManager::Instance.requestWork((eObjID)0x11501, 0);
			cObjReadManager::Instance.requestWork((eObjID)0x11502, 0);
			cObjReadManager::Instance.requestWork((eObjID)0x11503, 0);
			cObjReadManager::Instance.requestWork((eObjID)0x11504, 0);
			cObjReadManager::Instance.requestWork((eObjID)0x11505, 0);
			cObjReadManager::Instance.requestWork((eObjID)0x11506, 0);
			isPlayerAtOnce = true;
		}


		modelItems = injector::ReadMemory<ModelItems*>(shared::base + 0x17EA01C, true);


		originalSword = injector::ReadMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, true);

		if (modelItems) {
			if (modelItems->m_nHair != 0x11401) originalModelItems.m_nHair = modelItems->m_nHair;
			if (modelItems->m_nVisor != 0x11402) originalModelItems.m_nVisor = modelItems->m_nVisor;
			if (modelItems->m_nSheath != 0x11404) originalModelItems.m_nSheath = modelItems->m_nSheath;
			if (modelItems->m_nHead != 0x11405) originalModelItems.m_nHead = modelItems->m_nHead;
		}


		if (!SamSpawned && (GetKeyState(0x36) & 0x8000) && modelItems) {
			Spawner((eObjID)0x11400);
			SamSpawned = true;
			//camera back to Raiden
			((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
		}

		if (!WolfSpawned && (GetKeyState(0x35) & 0x8000) && modelItems) {
			Spawner((eObjID)0x11500);
			//injector::WriteMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, 0x11501, true);
			WolfSpawned = true;
			//camera back to Raiden
			((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
		}

		Hw::cVec4* matrix = (Hw::cVec4*)&cCameraGame::Instance.m_TranslationMatrix;

		auto& pos = matrix[0];
		auto& rotate = matrix[1];



		if (SamSpawned || WolfSpawned || (ArmstrongAtOnce || PlayAsArmstrong) || (SamAtOnce || PlayAsSam))
			for (auto node = EntitySystem::Instance.m_EntityList.m_pFirst; node != EntitySystem::Instance.m_EntityList.m_pEnd; node = node->m_next) {

				auto value = node->m_value;
				if (!value) continue;

				auto player = node->m_value->getEntityInstance<Pl0000>();
				if (!player) continue;


				auto Enemy = node->m_value->getEntityInstance<BehaviorEmBase>();
				if (!Enemy) continue;

				int controllerNumber = 0;
			//	if (mObjId == (eObjID)0x11400) controllerNumber = 0;
			//	if (mObjId == (eObjID)0x11500) controllerNumber = 1;

				bool isMove = false;
				bool isJump = false;
				bool isAttack = false;
				bool isRunning = false;
				bool isTaunt = false;

				if ( ((Enemy->m_pEntity->m_nEntityIndex == 0x20700 || Enemy->m_pEntity->m_nEntityIndex == 0x2070A) && (PlayAsArmstrong || ArmstrongAtOnce))
					
					|| (Enemy->m_pEntity->m_nEntityIndex == 0x20020 && (PlayAsSam || SamAtOnce))
					) {

				
					if (Enemy->m_pEntity->m_nEntityIndex == 0x20700 || Enemy->m_pEntity->m_nEntityIndex == 0x2070A && PlayAsArmstrong) {
						unsigned int BossActions[] = {0x20000, 0x20003, 0x20007, 0x20006, 0x20001, 0x20009, 0x20010};
						UpdateBossActions(Enemy,BossActions);

						if (ArmstrongCanDamagePlayer)
							Enemy->field_640 = 2;
						else
							Enemy->field_640 = 1;
					}

					if (Enemy->m_pEntity->m_nEntityIndex == 0x20020 && PlayAsSam) {
						unsigned int BossActions[] = {0x30004, 0x30006, 0x30007, 0x30014, 0x3001C, 0x30005, 0x10006};
						UpdateBossActions(Enemy, BossActions);

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



					unsigned int WalkingState = 0x10001;
					if (Enemy->m_pEntity->m_nEntityIndex == 0x20020) WalkingState = 0x10002;


					if ((GetKeyState(std::stoi(Forward, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadForward)) {
						if (Enemy->m_nCurrentAction != WalkingState && Enemy->m_nCurrentAction == 0x10000) Enemy->setState(WalkingState, 0, 0, 0);
						field_Y = -1000;
					}

					
					

					//back
					if ((GetKeyState(std::stoi(Back, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadBack)) {
						//if (GetAsyncKeyState(0x4B)) {
						if (Enemy->m_nCurrentAction != WalkingState && Enemy->m_nCurrentAction == 0x10000) Enemy->setState(WalkingState, 0, 0, 0);
						field_Y = 1000;
					}

					//left
					if ((GetKeyState(std::stoi(Left, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadLeft)) {
						if (Enemy->m_nCurrentAction != WalkingState && Enemy->m_nCurrentAction == 0x10000) Enemy->setState(WalkingState, 0, 0, 0);
						field_X = -1000;
					}

					//right
					if ((GetKeyState(std::stoi(Right, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadRight)) {
						if (Enemy->m_nCurrentAction != WalkingState && Enemy->m_nCurrentAction == 0x10000) Enemy->setState(WalkingState, 0, 0, 0);
						field_X = 1000;
					}

					if (field_X != 0 || field_Y != 0) {
						float angle = atan2(field_X, field_Y);
						Enemy->m_vecRotation.y = angle;
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

				if ((player->m_pEntity->m_nEntityIndex == (eObjID)0x11400 || player->m_pEntity->m_nEntityIndex == (eObjID)0x11500) && modelItems) {




					modelItems->m_nHair = originalModelItems.m_nHair;
					modelItems->m_nVisor = originalModelItems.m_nVisor;
					modelItems->m_nSheath = originalModelItems.m_nSheath;
					modelItems->m_nHead = originalModelItems.m_nHead;



					if (EnableDamageToPlayers)
						player->field_640 = 0;
					else
						player->field_640 = 1;

					//injector::WriteMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, originalSword, true);


					if (GetKeyState(0x39)) {
						AutoNormalAttackEnable = !AutoNormalAttackEnable;
						AutoStrongAttackEnable = false;
					}

					if (GetKeyState(0x30)) {
						AutoStrongAttackEnable = !AutoStrongAttackEnable;
						AutoNormalAttackEnable = false;
					}

					//Change camera between players
					if ((GetKeyState(0x37) & 0x8000)) {
						((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
					}

					if ((GetKeyState(0x38) & 0x8000)) {
						((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(player);
					}

					player->m_nKeyHoldingFlag = 0;
					player->field_D0C = 0;
					player->field_D08 = 0;

					//forward
					if ((GetKeyState(std::stoi(Forward, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadForward)) {
						player->m_nKeyHoldingFlag |= 0x400000;
						player->field_D0C = -1000.0f;
						isMove = true;
					}

					//back
					if ((GetKeyState(std::stoi(Back, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber,GamepadBack)) {
						//if (GetAsyncKeyState(0x4B)) {
						player->m_nKeyHoldingFlag |= 0x800000;
						player->field_D0C = 1000.0f;
						isMove = true;
					}

					//left
					if ((GetKeyState(std::stoi(Left, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadLeft)) {
						player->m_nKeyHoldingFlag |= 0x100000;
						player->field_D08 = -1000.0f;
						isMove = true;
					}

					//right
					if ((GetKeyState(std::stoi(Right, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadRight)) {
						player->m_nKeyHoldingFlag |= 0x200000;
						player->field_D08 = 1000.0f;
						isMove = true;
					}

					// Check if the jump key is held down
					if (getKeyState(std::stoi(Jump, nullptr, 16)).isHolding || IsGamepadButtonPressed(controllerNumber, GamepadJump)) {




					//	if (getKeyState(std::stoi(Jump, nullptr, 16)).isPressed) 
							player->m_nKeyPressedFlag |= 0x10;
						if (getKeyState(std::stoi(Jump, nullptr, 16)).isHolding) 
							player->m_nKeyHoldingFlag |= 0x10; // Set the flag to indicate that the key is held down
						player->field_D00 |= 0x10;
						player->field_D04 |= 0x10;
						isJump = true;
					}
					else {
						player->m_nKeyPressedFlag &= ~0x10; // Reset the flag to indicate that the key is not held down
						player->m_nKeyHoldingFlag &= ~0x10; // Reset the flag to indicate that the key is not held down
						isJump = false;
					}


					//normal attack
					if ((GetKeyState(std::stoi(NormalAttack, nullptr, 16)) & 0x8000) || AutoNormalAttackEnable || IsGamepadButtonPressed(controllerNumber, GamepadNormalAttack)) {
						player->m_nKeyHoldingFlag |= 0x40;
						player->m_nKeyPressedFlag |= 0x40;
						player->field_D04 |= 0x40;
						isAttack = true;
					}

					//strong attack
					if ((GetKeyState(std::stoi(StrongAttack, nullptr, 16)) & 0x8000) || AutoStrongAttackEnable || IsGamepadButtonPressed(controllerNumber, GamepadStrongAttack)) {
						player->m_nKeyHoldingFlag |= 0x80;
						player->m_nKeyPressedFlag |= 0x80;
						player->field_D04 |= 0x80;
						isAttack = true;
					}

					//Run
					if ((GetKeyState(std::stoi(Run, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadRun)) {
						player->m_nKeyHoldingFlag |= 0x2000;
						isRunning = true;
					}

					//Taunt
					if ((GetKeyState(std::stoi(Taunt, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadTaunt)) {
						player->m_nKeyHoldingFlag |= 0x8;
						player->m_nKeyPressedFlag |= 0x8;
						isTaunt = true;
					}

					if (!isMove && !isJump && !isAttack && !isRunning && !isTaunt) {
						player->m_nKeyHoldingFlag = 0;
						player->m_nKeyPressedFlag = 0;
						player->field_D04 = 0;
					}
					if (!isMove && !isRunning) {
						player->field_D0C = 0;
						player->field_D08 = 0;
					}

					if (!isAttack) {
						player->m_nKeyHoldingFlag &= ~0x80;
						player->m_nKeyPressedFlag &= ~0x80;
						player->field_D04 &= ~0x80;

						player->m_nKeyHoldingFlag &= ~0x40;
						player->m_nKeyPressedFlag &= ~0x40;
						player->field_D04 &= ~0x40;
					}

					if (!isJump) {
						//player->field_CF8 &= ~0x10;
						//player->field_CFC &= ~0x10;
						//player->field_D04 &= ~0x10;
					}

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
		ImGui::Begin("2 Players mod", NULL, ImGuiWindowFlags_NoCollapse);
		ImGui::SetNextWindowSize({ 900, 600 });
		ImGuiIO io = ImGui::GetIO();
		io.MouseDrawCursor = true;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		if (ImGui::BeginTabBar("##WindowTab", ImGuiTabBarFlags_NoTooltip))
		{
			if (ImGui::BeginTabItem("Main Menu"))
			{
				Pl0000* MainPlayer = cGameUIManager::Instance.m_pPlayer;

				if (ImGui::Button("Spawn Sam as second player") && MainPlayer && !SamSpawned) {
					Spawner((eObjID)0x11400);
					SamSpawned = true;
					//camera back to Raiden
					((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
				}


				if (ImGui::Button("Spawn Wolf as second player") && MainPlayer && !WolfSpawned) {
					Spawner((eObjID)0x11500);
					WolfSpawned = true;
					//camera back to Raiden
					((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
				}

				ImGui::Checkbox("Player 2 is Armstrong", &PlayAsArmstrong);
				ImGui::Checkbox("Armstrong can damage player", &ArmstrongCanDamagePlayer);
				if (ImGui::Button("Spawn Armstrong as second player") && MainPlayer && !ArmstrongAtOnce && PlayAsArmstrong) {
					Spawner((eObjID)0x20700);
					ArmstrongAtOnce = true;
				}

				ImGui::Checkbox("Player 2 is Boss Sam", &PlayAsSam);
				ImGui::Checkbox("Boss Sam can damage player", &BossSamCanDamagePlayer);
				if (ImGui::Button("Spawn boss Sam as second player") && MainPlayer && !SamAtOnce && PlayAsSam) {
					Spawner((eObjID)0x20020);
					SamAtOnce = true;
				}


				ImGui::Checkbox("Allow damage to another player", &EnableDamageToPlayers);



				
				//ImGui::Checkbox("Player 2 is Boss Sam", &PlayAsArmstrong);
				//ImGui::Checkbox("Player 2 is Sundowner", &PlayAsArmstrong);
				//ImGui::Checkbox("Player 2 is Monsoon", &PlayAsArmstrong);
				//ImGui::Checkbox("Player 2 is Mistral", &PlayAsArmstrong);


				if (ArmstrongAtOnce || PlayAsArmstrong)
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

				/*if (PlayAsArmstrong)
					injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x90909090, true);
				else
					injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x909090, true);

				if (PlayAsArmstrong)
					injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x90909090, true);
				else
					injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x909090, true);

				if (PlayAsArmstrong)
					injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x90909090, true);
				else
					injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x909090, true);*/


				if (ImGui::Button("Teleport second player to Raiden") && MainPlayer) {

					for (auto node = EntitySystem::Instance.m_EntityList.m_pFirst; node != EntitySystem::Instance.m_EntityList.m_pEnd; node = node->m_next) {

						auto value = node->m_value;
						if (!value) continue;

						auto player = node->m_value->getEntityInstance<Pl0000>();
						if (!player) continue;

						if (player->m_pEntity->m_nEntityIndex == (eObjID)0x11500 || player->m_pEntity->m_nEntityIndex == (eObjID)0x11400 || (PlayAsArmstrong && player->m_pEntity->m_nEntityIndex == (eObjID)0x20700)) {

							player->m_vecTransPos.x = MainPlayer->m_vecTransPos.x;
							player->m_vecTransPos.y = MainPlayer->m_vecTransPos.y;
							player->m_vecTransPos.z = MainPlayer->m_vecTransPos.z;
							player->m_vecTransPos.w = MainPlayer->m_vecTransPos.w;
						}

					}


				}

				ImGui::EndTabItem();
			}



			ImGui::EndTabBar();
		}
		ImGui::End();
		ImGui::EndFrame();
		ImGui::Render();
	}
}