#include "pch.h"
#include <assert.h>
#include "gui.h"
#include <Events.h>
#include "imgui/imgui.h"
#include "spawner.cpp"
#include <Trigger.h>
#include <GameMenuStatus.h>
#include <Pl0000.h>
#include <EntitySystem.h>
#include <Entity.h>
#include <cGameUIManager.h>
#include <injector/injector.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <PlayerManagerImplement.h>
#include <BehaviorEmBase.h>
#include <math.h>
#include <Windows.h>
#include "MGRCustomAI.h"
#include "MGRCustomUI.h"
#include "ModelItems.h"
int memory_address = 0x0;


#pragma comment(lib, "d3dx9.lib")
#ifdef _MSC_VER < 1700 //pre 2012
#pragma comment(lib,"Xinput.lib")
#else
#pragma comment(lib,"Xinput9_1_0.lib")
#endif
#include <Camera.h>


std::string character_titles[6] = {"sam", "blade_wolf", "boss_sam", "sundowner", "senator_armstrong", "raiden"};


bool configLoaded = false;
//bool SamSpawned = false;
//bool WolfSpawned = false;

//bool AutoNormalAttackEnable = false;
//bool AutoStrongAttackEnable = false;

/*
bool ArmstrongAtOnce = false;
bool SundownerAtOnce = false;
bool MistralAtOnce = false;
bool MonsoonAtOnce = false;
bool SamAtOnce = false;
*/

//bool SundownerBehaviorActive = true;

bool PlayAsMistral = false;
bool PlayAsMonsoon = false;
bool PlayAsSundowner = false;
bool SundownerCanDamagePlayer = false;
bool PlayAsSam = false;
bool BossSamCanDamagePlayer = false;
bool PlayAsArmstrong = false;
bool ArmstrongCanDamagePlayer = false;


bool EnableDamageToPlayers = false;

bool p1IsKeyboard = true;
bool p1WasKeyboard = p1IsKeyboard; // detect change (sloppy ik)

//unsigned int sword = 0x0;
//unsigned int originalSword = 0x0;


bool isMenuShow = false;


//eObjID mObjId = (eObjID)0x0;

unsigned int HotKey = VK_INSERT; //Hotkey for menu show


//DWORD dwResult;
//XINPUT_STATE state;

//this struct begins from player CF8 field (player key isHolding), size - 0x30;
/*struct KeysStruct
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
KeysStruct m_nKeysStruct;*/


ModelItems* modelItems;
ModelItems originalModelItems;
unsigned int* modelSword;
unsigned int originalModelSword;


//original address in game is "METAL GEAR RISING REVENGEANCE.exe"+1777E60
/*struct Player1_Keys
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

};*/

/*struct Keys {
	unsigned int m_nKeys[28];
};

Keys* keys = injector::ReadMemory<Keys*>(shared::base + 0x177B7C0, true);*/

/*unsigned int keyArray[] = {
	'W', 'S', 'A', 'D', VK_TAB, VK_SPACE, 0x80000001, 0x80000002, 0x9F,
	0x9B, 'F', 'R', 'E', 'V'
};*/

//Player1_Keys pl1Keys;

//KeysStruct currentKey;

bool isPlayerAtOnce = false;
bool gotOriginalModelItems = false;

Pl0000* players[5] = { nullptr };
eObjID playerTypes[5] = { (eObjID)0 };
int playerSpawnCheck[5] = { 0 };

void Spawner(eObjID id, int controllerIndex = -1) {
	if (controllerIndex > -1) {
		playerTypes[controllerIndex + 1] = id;
	}
	else {
		for (int i = 0; i < 5; i++) {
			if (!playerTypes[i]) {
				playerTypes[i] = id;
				controllerIndex = i - 1;
				break;
			}
		}
	}

	if (id == (eObjID)0x11400) {
		modelItems->m_nHair = 0x11401;
		modelItems->m_nVisor = 0x11402;
		modelItems->m_nSheath = 0x11404;
		modelItems->m_nHead = 0x11405;
		modelItems->m_nModel = 0x11406;
		*modelSword = 0x11403;
	}
	else if (id == (eObjID)0x10010) {
		modelItems->m_nHair = 0x11011;
		modelItems->m_nVisor = 0x11014;
		modelItems->m_nSheath = 0x11013;
		modelItems->m_nHead = 0x11017;
		modelItems->m_nModel = 0x11010;
		*modelSword = 0x11012;
	}
	else if (id == (eObjID)0x11500) {
		modelItems->m_nModel = 0x11505;
	}

	m_EntQueue.push_back({ .mObjId = id, .iSetType = 0,.bWorkFail = !isObjExists(id) });
	
	// Frame counter, if it hits zero and the player does not exist, resets playertype
	playerSpawnCheck[controllerIndex + 1] = 30;

	//injector::WriteMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, 0x11501, true);

}

bool overrideCamera = false;
float camLateralScale = 1.0;
float camHeightScale = 1.0;
float camYaw = 0.0;

int __fastcall CameraHacked(void* ecx, void* edx, float a2) {
	if (overrideCamera) {
		return ((INT(__thiscall*)(void*))(shared::base + 0x9d03e0))(ecx);
	}
	else {
		return ((INT(__thiscall*)(void*, float))(shared::base + 0x9d1a30))(ecx, a2);
	}
}


void RecalibrateBossCode() {
	if (PlayAsArmstrong)
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x909090, true);
	else
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0xFFD00EE8, true);

	/*if (PlayAsSundowner)
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x90909090, true);
	else
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x909090, true);*/
	if (PlayAsSundowner) {
		injector::WriteMemory<unsigned char>(shared::base + 0x1961B0, 0xC3, true);
		//injector::WriteMemory<unsigned int>(shared::base + 0x196B8B, 0x90, true);
	}
	else
		injector::WriteMemory<unsigned char>(shared::base + 0x1961B0, 0x56, true);
	// Potential Offsets:
	// 0x001011AD - Handles entity, something



	if (PlayAsSam) {
		injector::WriteMemory<unsigned int>(shared::base + 0x39C32, 0x909090, true);
		injector::WriteMemory<unsigned int>(shared::base + 0x39CC5, 0x909090, true);
	}
	else {
		injector::WriteMemory<unsigned int>(shared::base + 0x39C32, 0xFFEE49E8, true);
		injector::WriteMemory<unsigned int>(shared::base + 0x39CC5, 0xFFEDB6E8, true);
	}

	/*
	if (PlayAsMonsoon)
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x90909090, true);
	else
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x909090, true);

	if (PlayAsMistral)
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x90909090, true);
	else
		injector::WriteMemory<unsigned int>(shared::base + 0x1C656D, 0x909090, true);*/
}

void TeleportToMainPlayer(Pl0000* mainPlayer, int controllerIndex = -1) {
	cVec4& pos = mainPlayer->m_vecTransPos;
	cVec4& rot = mainPlayer->m_vecRotation;
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
	static bool keyHoldingStates[256] = { false }; // Ìàññèâ äëÿ îòñëåæèâàíèÿ ñîñòîÿíèÿ óäåðæàíèÿ êëàâèø

	bool isKeyDown = (GetKeyState(virtualKeyCode) & 0x8000) != 0;

	keyState.isPressed = isKeyDown && !keyHoldingStates[virtualKeyCode];
	keyState.isHolding = isKeyDown;

	keyHoldingStates[virtualKeyCode] = isKeyDown;

	return keyState;
}


bool handleKeyPress(int hotKey, bool* isMenuShowPtr) {
	static bool wasHotKeyPressed = false; // Ñòàòè÷åñêàÿ ïåðåìåííàÿ, ñîõðàíÿåò ñîñòîÿíèå ìåæäó âûçîâàìè

	if (GetKeyState(hotKey) & 0x8000) {
		if (!wasHotKeyPressed) {
			*isMenuShowPtr = !(*isMenuShowPtr);
			wasHotKeyPressed = true;
		}
	}
	else {
		wasHotKeyPressed = false;
	}
	return *isMenuShowPtr; // Âîçâðàùàåì òåêóùåå ñîñòîÿíèå ìåíþ
}



Pl0000* MainPlayer = cGameUIManager::Instance.m_pPlayer;


void Update()
{
	for (int i = 0; i < 5; i++) {
		if (playerSpawnCheck[i]) playerSpawnCheck[i]--;
	}

	overrideCamera = false;

	if (!configLoaded) {

		injector::WriteMemory<unsigned short>(shared::base + 0x69A516, 0x9090, true); // F3 A5 // Disable normal input Sam and Wolf
		injector::WriteMemory<unsigned short>(shared::base + 0x7937E6, 0x9090, true); // Disable normal input Raiden
		injector::WriteMemory<unsigned int>(shared::base + 0x9DB430, 0x909090, true); // E8 1B FF FF FF // Disable normal controller input
		injector::MakeNOP(shared::base + 0x69E313, 6, true); // Remove need for custom pl1400 and pl1500
		//injector::WriteMemory<unsigned char>(shared::base + 0x6C7EC3, 0xEB, true); // Disable vanilla enemy targeting (broken)
		injector::WriteMemory<unsigned int>(shared::base + 0x823766, *(unsigned int*)(shared::base + 0x823766) - 5712, true); // Disable camera
		//injector::MakeCALL(shared::base + 0x823765, &CameraHacked, true); // Disable camera sometimes

		// Load image data
		LoadUIData();


		// Absolutely required
		if (pSprite == NULL) {
			D3DXCreateSprite(Hw::GraphicDevice, &pSprite);
		}

		LoadConfig();
		configLoaded = true;
	}

	MainPlayer = cGameUIManager::Instance.m_pPlayer;

	if (!MainPlayer) {
		for (int i = 0; i < 5; i++) {
			players[i] = nullptr;
			playerTypes[i] = (eObjID)0;
		}
		ResetControllerAllFlags();
		isPlayerAtOnce = false;
		return;
	}

	// Quick check to cancel execution in the customize menu. Thanks Frouk.
	int* gScenarioManagerImplement = *(int**)(shared::base + 0x17E9A30);

	if (gScenarioManagerImplement && gScenarioManagerImplement[45])
	{
		int* phase = (int*)gScenarioManagerImplement[45];
		if (phase[1] == 0xf05) // customize menu
		{
			return;
		}
	}

#define p1Index (p1IsKeyboard ? 0 : 1)

	if (p1IsKeyboard != p1WasKeyboard) { // swap
		p1WasKeyboard = p1IsKeyboard;
		Pl0000* temp = players[0];
		players[0] = players[1];
		players[1] = temp;
		eObjID temp2 = playerTypes[0];
		playerTypes[0] = playerTypes[1];
		playerTypes[1] = temp2;
		controller_flag[0] = (players[1] ? 2 : 0);
	}

	if (players[p1Index] != MainPlayer) {
		for (int i = 0; i < 5; i++) {
			players[i] = nullptr;
			playerTypes[i] = (eObjID)0;
		}
		players[p1Index] = MainPlayer;
		playerTypes[p1Index] = MainPlayer->m_pEntity->m_nEntityIndex;
	}

	if (!isPlayerAtOnce) {
		// Sam
		for (int itemToRequest = 0x11401; itemToRequest <= 0x11406; itemToRequest++)
			cObjReadManager::Instance.requestWork((eObjID)itemToRequest, 0);
		cObjReadManager::Instance.requestWork((eObjID)0x3D070, 0); // Sam projectile
		
		// Wolf
		for (int itemToRequest = 0x11501; itemToRequest <= 0x11506; itemToRequest++)
			cObjReadManager::Instance.requestWork((eObjID)itemToRequest, 0);
		
		// Raiden
		for (int itemToRequest = 0x11010; itemToRequest <= 0x11014; itemToRequest++)
			cObjReadManager::Instance.requestWork((eObjID)itemToRequest, 0);
		cObjReadManager::Instance.requestWork((eObjID)0x11017, 0);


		isPlayerAtOnce = true;
		gotOriginalModelItems = false;
	}


	modelItems = injector::ReadMemory<ModelItems*>(shared::base + 0x17EA01C, true);
	modelSword = injector::ReadMemory<unsigned int*>(shared::base + 0x17E9FF4, true);


	//originalSword = injector::ReadMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, true);

	if (modelItems) {
		if (!gotOriginalModelItems) {
			gotOriginalModelItems = true;
			originalModelItems.m_nHair = modelItems->m_nHair;
			originalModelItems.m_nVisor = modelItems->m_nVisor;
			originalModelItems.m_nSheath = modelItems->m_nSheath;
			originalModelItems.m_nHead = modelItems->m_nHead;
			originalModelItems.m_nModel = modelItems->m_nModel;
			originalModelSword = *modelSword;
		}

		if (getKeyState('6').isPressed) {
			Spawner((eObjID)0x11400);
			//camera back to Raiden
			//((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
		}

		if (getKeyState('5').isPressed) {
			Spawner((eObjID)0x11500);
			//injector::WriteMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, 0x11501, true);
			//camera back to Raiden
			//((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
		}

	}

	// MainPlayer take camera control
	if ((GetKeyState('7') & 0x8000) || (GetKeyState('T') & 0x8000))
		((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);

	//Hw::cVec4* matrix = (Hw::cVec4*)&cCameraGame::Instance.m_TranslationMatrix;

	//auto& pos = matrix[0];
	//auto& rotate = matrix[1];

	// Detect newly-spawned players
	bool needNewPlayer = false;
	for (int i = 0; i < 5; i++) {
		if (playerTypes[i] && !players[i])
			needNewPlayer = true;
	}

	if (needNewPlayer) {
		for (auto node = EntitySystem::Instance.m_EntityList.m_pFirst; node != EntitySystem::Instance.m_EntityList.m_pLast; node = node->m_next) {

			auto value = node->m_value;
			if (!value) continue;

			auto player = value->getEntityInstance<Pl0000>();
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
					if (gotOriginalModelItems) {
						modelItems->m_nHair = originalModelItems.m_nHair;
						modelItems->m_nVisor = originalModelItems.m_nVisor;
						modelItems->m_nSheath = originalModelItems.m_nSheath;
						modelItems->m_nHead = originalModelItems.m_nHead;
						modelItems->m_nModel = originalModelItems.m_nModel;
						*modelSword = originalModelSword;
					}
					break;
				}
			}

		}
	}

	// Reset players who fail to spawn
	for (int i = 0; i < 5; i++) {
		if (!playerSpawnCheck[i] && playerTypes[i] && !players[i]) {
			playerTypes[i] = (eObjID)0;
			if (i > 0)
				controller_flag[i - 1] = 1;
		}
	}

	//cVec4 playerPos[5];
	//int playerCount = 0;

	// Player control overrides
	for (int i = 0; i < 5; i++) {
		Pl0000* player = players[i];
		if (!player) continue;

		//playerPos[playerCount] = player->m_vecTransPos;
		//playerCount++;

		BehaviorEmBase* Enemy = (BehaviorEmBase*)player;
		int controllerNumber = i - 1;

		if (((Enemy->m_pEntity->m_nEntityIndex == 0x20700 || Enemy->m_pEntity->m_nEntityIndex == 0x2070A) && PlayAsArmstrong)
			|| (Enemy->m_pEntity->m_nEntityIndex == 0x20020 && PlayAsSam)
			|| (Enemy->m_pEntity->m_nEntityIndex == 0x20310 && PlayAsSundowner)
			) {
			bool CanDamagePlayer = ArmstrongCanDamagePlayer;

			if (Enemy->m_pEntity->m_nEntityIndex == 0x20020)
				CanDamagePlayer = BossSamCanDamagePlayer;
			if (Enemy->m_pEntity->m_nEntityIndex == 0x20310)
				CanDamagePlayer = SundownerCanDamagePlayer;

			FullHandleAIBoss(Enemy, controllerNumber, CanDamagePlayer);
		}

		if ((player->m_pEntity->m_nEntityIndex == (eObjID)0x11400
			|| player->m_pEntity->m_nEntityIndex == (eObjID)0x11500)
			&& modelItems) {
			FullHandleAIPlayer(player, controllerNumber, EnableDamageToPlayers);

		}

		if (player->m_pEntity->m_nEntityIndex == (eObjID)0x10010) {
			FullHandleAIPlayer(player, controllerNumber, EnableDamageToPlayers);
		}
	}

	overrideCamera = true;
	cCameraGame* camera = &cCameraGame::Instance;
	cVec4* oldPos = &camera->m_TranslationMatrix.m_vecPosition;
	cVec4* oldTarget = &camera->m_TranslationMatrix.m_vecLookAtPosition;

	float maxDist = 0.0;
	cVec4 targetCenter = { 0.0, INFINITY, 0.0, 1.0 };
	cVec4 cameraPos = { 0.0, 0.0, 0.0, 1.0 };

#define getYaw(x, z) (((z) != 0) ? atan((x)/(z)) : DegreeToRadian(90))

	for (Pl0000* player : players) {
		if (!player) continue;
		cVec4 p1Pos = player->m_vecTransPos;
		for (Pl0000* player2 : players) {
			if (!player2) continue;
			cVec4 p2Pos = player2->m_vecTransPos;
			float dist = sqrt((p2Pos.x - p1Pos.x) * (p2Pos.x - p1Pos.x)
				+ (p2Pos.z - p1Pos.z) * (p2Pos.z - p1Pos.z));
			if (dist >= 15.0) {
				// Move players closer
				float distMoveBack = (dist - 15.0) / 2;
				float xVecNrm = (p2Pos.x - p1Pos.x) / dist;
				float zVecNrm = (p2Pos.z - p1Pos.z) / dist;
				player->m_vecTransPos.x += distMoveBack * xVecNrm;
				player->m_vecTransPos.z += distMoveBack * zVecNrm;

				player2->m_vecTransPos.x -= distMoveBack * xVecNrm;
				player2->m_vecTransPos.z -= distMoveBack * zVecNrm;
			}
			if (dist >= maxDist) {
				maxDist = dist;
				targetCenter.x = p1Pos.x / 2 + p2Pos.x / 2;
				targetCenter.z = p1Pos.z / 2 + p2Pos.z / 2;
			}
		}
		targetCenter.y = min(targetCenter.y, p1Pos.y + 1.0);
	}
	cameraPos.x = targetCenter.x + camLateralScale * sin(camYaw);
	cameraPos.y = targetCenter.y + max(maxDist, 5.0) * camHeightScale;
	cameraPos.z = targetCenter.z + camLateralScale * cos(camYaw);

	/* // Old implementation, more horizontal
	float curYaw = getYaw(oldTarget->x - oldPos->x, oldTarget->z - oldPos->z);
	float newDirection[2];
	for (int i = 0; i < playerCount; i++) {
		for (int j = i; j < playerCount; j++) {
			float dist = sqrt((playerPos[j].x - playerPos[i].x) * (playerPos[j].x - playerPos[i].x)
				+ (playerPos[j].z - playerPos[i].z) * (playerPos[j].z - playerPos[i].z));
			if (dist >= maxDist) {
				maxDist = dist;
				targetCenter.x = playerPos[j].x / 2 + playerPos[i].x / 2;
				targetCenter.z = playerPos[j].z / 2 + playerPos[i].z / 2;
				newDirection[0] = -(playerPos[j].z - playerPos[i].z);
				newDirection[1] = (playerPos[j].x - playerPos[i].x);
				float newYaw = getYaw(newDirection[0], newDirection[1]);
				float yawDiff = newYaw - curYaw;
				if (yawDiff < 0.0) {
					yawDiff += 2 * PI;
				}
				else if (yawDiff >= 2 * PI) {
					yawDiff -= 2 * PI;
				}
				if (yawDiff > PI / 2 && yawDiff < 3 * PI / 2) {
					newDirection[0] *= -1.0;
					newDirection[1] *= -1.0;
				}
			}
		}
		targetCenter.y = min(targetCenter.y, playerPos[i].y + 1.0);
	}
	cameraPos.y = targetCenter.y + 2.0 * camHeightScale;
	

	//cameraPos.x -= 10.0 * playerCount;
	if (maxDist <= 0.2) { // Don't die
		newDirection[0] = 0.0;
		newDirection[1] = -3.0;
	} else if (maxDist <= 3.0) { // Limit zoom
		newDirection[0] *= 3.0 / maxDist;
		newDirection[1] *= 3.0 / maxDist;
	}
	// Screw you, top down only
	newDirection[0] = 0.0;
	newDirection[1] = -3.0;
	cameraPos.y += maxDist * camHeightScale;
	newDirection[0] *= camLateralScale;
	newDirection[1] *= camLateralScale;

	cameraPos.x = targetCenter.x + newDirection[0];
	cameraPos.z = targetCenter.z + newDirection[1];
	//cameraPos.z -= 1.0;
	//cameraPos.y += 15.0;
	*/

#define posUpdateSpeed 0.2
	oldPos->x = oldPos->x * (1 - posUpdateSpeed) + cameraPos.x * posUpdateSpeed;
	oldPos->y = oldPos->y * (1 - posUpdateSpeed) + cameraPos.y * posUpdateSpeed;
	oldPos->z = oldPos->z * (1 - posUpdateSpeed) + cameraPos.z * posUpdateSpeed;

	oldTarget->x = oldTarget->x * (1 - posUpdateSpeed) + targetCenter.x * posUpdateSpeed;
	oldTarget->y = oldTarget->y * (1 - posUpdateSpeed) + targetCenter.y * posUpdateSpeed;
	oldTarget->z = oldTarget->z * (1 - posUpdateSpeed) + targetCenter.z * posUpdateSpeed;
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
		Events::OnPresent += []() {
			Present();
		};

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




void SpawnCharacter(int id, int controller) {

	if (id == 0)
		Spawner((eObjID)0x11400, controller);
	else if (id == 1)
		Spawner((eObjID)0x11500, controller);
	else if (id == 4) {
		Spawner((eObjID)0x20700, controller);
		PlayAsArmstrong = true;
	}
	else if (id == 3) {
		Spawner((eObjID)0x20310, controller);
		PlayAsSundowner = true;
	}
	else if (id == 2) {
		Spawner((eObjID)0x20020, controller);
		PlayAsSam = true;
	}
	else if (id == 5) {
		Spawner((eObjID)0x10010, controller);
	}
	RecalibrateBossCode();
	//camera back to Raiden
	//((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);

}

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

				if (ImGui::Button("Spawn Raiden as next player") && MainPlayer) {
					Spawner((eObjID)0x10010);
					//camera back to P1
					//((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
				}

				if (ImGui::Button("Spawn Sam as next player") && MainPlayer) {
					Spawner((eObjID)0x11400);
					//camera back to Raiden
					//((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
				}


				if (ImGui::Button("Spawn Wolf as next player") && MainPlayer) {
					Spawner((eObjID)0x11500);
					//camera back to Raiden
					//((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
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


				ImGui::Checkbox("Sundowner is player-controlled", &PlayAsSundowner);
				ImGui::Checkbox("Sundowner can damage player", &SundownerCanDamagePlayer);
				if (ImGui::Button("Spawn Sundowner as next player") && MainPlayer) {
					Spawner((eObjID)0x20310);
					PlayAsSundowner = true;
				}



				ImGui::Checkbox("Allow damage to another player", &EnableDamageToPlayers);
				ImGui::Checkbox("Player 1 uses keyboard (else Controller 1)", &p1IsKeyboard);
				

				// Sundowner's Head: 1581929


				// Debug print Sam's flags
//#define PRINTSAM
//#define PRINTENEMY
//#define SHOWBOSSACTION
				
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
#ifdef SHOWBOSSACTION
					auto Enemy = value->getEntityInstance<BehaviorEmBase>();
					if (!Enemy) continue;
					if (value->m_nEntityIndex == 0x20020 || value->m_nEntityIndex == 0x20700) {
						ImGui::Text("Entity %x has state %x", value->m_nEntityIndex, Enemy->m_nCurrentAction);
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

				ImGui::Checkbox("All players can heal (30 second cooldown)", &EveryHeal);

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
			if (ImGui::BeginTabItem("Dev")) {
				ImGui::InputInt("Memory Address:", &memory_address);
				if (ImGui::Button("NOP Memory Address") && MainPlayer) {
					injector::WriteMemory<unsigned int>(shared::base + memory_address, 0x909090, true);
				}
				ImGui::InputFloat("Camera lateral scale", &camLateralScale);
				ImGui::InputFloat("Camera vertical scale", &camHeightScale);
				ImGui::EndTabItem();
			}


			ImGui::EndTabBar();
		}
		ImGui::End();
		ImGui::EndFrame();
		ImGui::Render();
		
	}
}