#include "pch.h"
#include "Camera.h"
#include "gui.h"
#include "MGRCustomAI.h"
#include "MGRCustomUI.h"
#include "ModelItems.h"
#include "MGRFunctions.h"
#include "Injection.h"
#include "dllmain.h"
#include "MPPlayer.h"

#include <assert.h>
#include <Events.h>
#include "imgui/imgui.h"
#include "spawner.cpp"
#include <Trigger.h>
#include <GameMenuStatus.h>
#include <Pl0000.h>
#include <EntitySystem.h>
#include <Entity.h>
#include <cGameUIManager.h>
#include <PhaseManager.h>
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

static WNDPROC oWndProc = NULL;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK hkWindowProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam) > 0)
		return 1L;
	return ::CallWindowProcA(oWndProc, hwnd, uMsg, wParam, lParam);
}

#pragma comment(lib, "d3dx9.lib")

//pre 2012
#ifdef _MSC_VER < 1700
#pragma comment(lib,"Xinput.lib")
//post 2012
#else
#pragma comment(lib,"Xinput9_1_0.lib")
#endif

#include <Camera.h>
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"


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

bool p1WasKeyboard = p1IsKeyboard; // detect change (sloppy ik)

//unsigned int sword = 0x0;
//unsigned int originalSword = 0x0;

//eObjID mObjId = (eObjID)0x0;

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

//bool isPlayerAtOnce = false;
bool gotOriginalModelItems = false;

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
		//injector::WriteMemory<unsigned int>(shared::base + 0x196B8B, 0x90, true);
		injector::WriteMemory<unsigned char>(shared::base + 0x1961B0, 0xC3, true); // ret
	}
	else {
		injector::WriteMemory<unsigned char>(shared::base + 0x1961B0, 0x56, true); // push esi
	}
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

void TeleportToMainPlayer(MPPlayer* mainPlayer, int controllerIndex) {
	TeleportToMainPlayer(mainPlayer->playerObj, controllerIndex);
}

void TeleportToMainPlayer(Pl0000* mainPlayer, int controllerIndex) {
	cVec4& pos = mainPlayer->m_vecTransPos;
	cVec4& rot = mainPlayer->m_vecRotation;
	if (controllerIndex >= 0) {
		Pl0000* player = players[controllerIndex]->playerObj;
		player->place(pos, rot);
		return;
	}
	for (MPPlayer* player : players) {
		if (player->playerObj)
			player->playerObj->place(pos, rot);
	}
}

/*
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
*/

void Spawner(eObjID id, int controllerIndex = -1, int costumeIndex = 0) {
	if (characterModelItems.contains(id)) {
		*modelItems = characterModelItems[id][costumeIndex];
	}

	int setType = 0;
	if (id == (eObjID)0x12040)
		setType = 1;
	if (id == (eObjID)0x10011) { // Unarmed Raiden
		id = (eObjID)0x10010;
		setType = 2; // Hardcoded in spawner.cpp look I'm just throwing stuff at the wall rn
	}

	if (controllerIndex > -1) {
		players[controllerIndex]->playerType = id;
	}
	else {
		for (int i = 0; i < maxPlayerCount; i++) {
			if (!players[controllerIndex]->playerType) {
				players[controllerIndex]->playerType = id;
				controllerIndex = i;
				break;
			}
		}
	}

	if (id == (eObjID)0x11400) {
		if (PhaseManager::ms_Instance.isDLCPhase())
			*modelSword = 0x11403;
		else
			*modelSword = 0x13005;
	}
	else if (id == (eObjID)0x10010) {
		if (PhaseManager::ms_Instance.isDLCPhase())
			*modelSword = 0x11403;
		else
			*modelSword = 0x11012;
	}
	else if (id == (eObjID)0x11500) {
		*modelSword = 0x11501;
	}

	m_EntQueue.push_back({ .mObjId = id, .iSetType = setType, .bWorkFail = !isObjExists(id) });

	// Frame counter, if it hits zero and the player does not exist, resets playertype
	players[controllerIndex]->spawnFailTimer = 30;

	//injector::WriteMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, 0x11501, true);

}

void SpawnCharacter(int id, int controller, int costumeIndex) {

	if (id == 0)
		Spawner((eObjID)0x11400, controller, costumeIndex);
	else if (id == 1)
		Spawner((eObjID)0x11500, controller, costumeIndex);
	else if (id == 2) {
		Spawner((eObjID)0x20020, controller);
		PlayAsSam = true;
	}
	else if (id == 3) {
		Spawner((eObjID)0x20310, controller);
		// phase 2 code go here
		players[controller]->isSundownerPhase2 = (costumeIndex % 2 != 0); // Every other costume

		PlayAsSundowner = true;
	}
	else if (id == 4) {
		if (costumeIndex == 1)
			Spawner((eObjID)0x2070A, controller);
		else
			Spawner((eObjID)0x20700, controller);
		PlayAsArmstrong = true;
	}
	else if (id == 5) {
		Spawner((eObjID)0x10010, controller, costumeIndex);
	}
	else if (id == 6) {
		Spawner((eObjID)0x10011, controller, costumeIndex); // Hardcoded to change to pl0010 but unarmed
	}
	else if (id == 7) {
		Spawner((eObjID)0x12040, controller);
	}
	RecalibrateBossCode();
	//camera back to Raiden
	giveVanillaCameraControl(MainPlayer);

}


void InitMod() {

	MakeInjections();

	MPPlayer::InitPlayers();

	// Load image data
	LoadUIData();


	// Absolutely required
	if (pSprite == NULL) {
		D3DXCreateSprite(Hw::GraphicDevice, &pSprite);
	}

	LoadInputConfig();
	LoadCameraConfig();
}


void Update()
{

	overrideCamera = false;

	if (!isInit) {
		InitMod();
		isInit = true;
	}

	for (MPPlayer* player : players) {
		if (player->spawnFailTimer) player->spawnFailTimer--;
	}

	MainPlayer = cGameUIManager::Instance.m_pPlayer;

	if (!MainPlayer) {
		if (p1IsKeyboard != p1WasKeyboard) p1WasKeyboard = p1IsKeyboard;
		MPPlayer::EmptyPlayers();
		ResetControllerAllFlags();
		gotOriginalModelItems = false;
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
		MPPlayer* temp = players[0];
		players[0] = players[1];
		players[1] = temp;
		MPPlayer::FixIndexes();
	}

	if (players[p1Index]->playerObj != MainPlayer) {
		MPPlayer::EmptyPlayers();
		players[p1Index]->playerObj = MainPlayer;
		players[p1Index]->playerType = MainPlayer->m_pEntity->m_EntityIndex;
		// Don't let controller 1 tag in twice!
		MPPlayer::FixIndexes();
	}

	modelItems = injector::ReadMemory<ModelItems*>(shared::base + 0x17EA01C, true);
	modelSword = injector::ReadMemory<unsigned int*>(shared::base + 0x17E9FF4, true);


	//originalSword = injector::ReadMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, true);

	if (modelItems && !gotOriginalModelItems) {
		gotOriginalModelItems = true;
		originalModelItems.m_nHair = modelItems->m_nHair;
		originalModelItems.m_nVisor = modelItems->m_nVisor;
		originalModelItems.m_nSheath = modelItems->m_nSheath;
		originalModelItems.m_nHead = modelItems->m_nHead;
		originalModelItems.m_nModel = modelItems->m_nModel;
		originalModelSword = *modelSword;
	}

	// MainPlayer take camera control
	if (GetKeyState('T') & 0x8000)
		giveVanillaCameraControl(MainPlayer);

	//Hw::cVec4* matrix = (Hw::cVec4*)&cCameraGame::Instance.m_TranslationMatrix;

	//auto& pos = matrix[0];
	//auto& rotate = matrix[1];

	// Detect newly-spawned players
	bool needNewPlayer = false;
	for (MPPlayer* player : players) {
		needNewPlayer |= player->TryingToSpawn();
	}

	if (needNewPlayer) {
		for (auto value : EntitySystem::ms_Instance.m_EntityList) {
			if (!value) continue;

			auto player = value->getEntityInstance<Pl0000>();
			if (!player) continue;

			bool alreadyInit = false;
			for (MPPlayer *playerIt : players) {
				if (playerIt->playerObj == player)
					alreadyInit = true;
			}
			if (alreadyInit) continue;

			for (MPPlayer* playerIt : players) {
				if (playerIt->TryingToSpawn() && value->m_EntityIndex == playerIt->playerType) {
					playerIt->playerObj = player;
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
	for (MPPlayer* player : players) {
		if (!player->spawnFailTimer && player->TryingToSpawn()) {
			player->playerType = (eObjID)0;
			player->controllerFlag = TaggingIn;
			m_EntQueue.clear();
		}
	}


	//cVec4 playerPos[5];
	//int playerCount = 0;

	// Player control overrides
	for (int i = 0; i < maxPlayerCount; i++) {
		Pl0000* player = players[i]->playerObj;
		if (!player) continue;

		//playerPos[playerCount] = player->m_vecTransPos;
		//playerCount++;

		if ((players[i]->playerType & 0xF0000) == 0x20000) {
			FullHandleAIBoss(players[i]);
		}
		else if (players[i]->playerType == (eObjID)0x12040) { // Not actually Pl0000*
			FullHandleDGPlayer(players[i]);
		}
		else { // Raiden, Sam, Wolf
			FullHandleAIPlayer(players[i]);
		}
	}

	OverrideCameraPos();
}


static bool GUIinit = false;
class Plugin
{
public:
	Plugin()
	{
		Events::OnDeviceReset.before += gui::OnReset::Before;
		Events::OnDeviceReset.after += gui::OnReset::After;
		Events::OnEndScene += gui::OnEndScene;
		/* // Or if you want to switch it to Present
		Events::OnPresent += gui::OnEndScene;
		*/
		Events::OnPresent += []() {
			if (!GUIinit)
			{
				oWndProc = (WNDPROC)::SetWindowLongPtr(Hw::OSWindow, GWLP_WNDPROC, (LONG)hkWindowProc);

				ImGui::CreateContext();
				ImGui_ImplWin32_Init(Hw::OSWindow);
				ImGui_ImplDX9_Init(Hw::GraphicDevice);

				gui::LoadStyle();

				GUIinit = true;
			}

			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			Present();

			gui::RenderWindow();

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		};

		Events::OnTickEvent += Update;
	}
} plugin;
