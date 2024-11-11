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



#pragma comment(lib, "d3dx9.lib")
#ifdef _MSC_VER < 1700 //pre 2012
#pragma comment(lib,"Xinput.lib")
#else
#pragma comment(lib,"Xinput9_1_0.lib")
#endif
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



Pl0000* MainPlayer = cGameUIManager::Instance.m_pPlayer;


void Update()
{


	if (!configLoaded) {

		injector::WriteMemory<unsigned short>(shared::base + 0x69A516, 0x9090, true); // F3 A5 // Disable normal input Sam and Wolf
		injector::WriteMemory<unsigned int>(shared::base + 0x9DB430, 0x909090, true); // E8 1B FF FF FF // Disable normal controller input
		injector::MakeNOP(shared::base + 0x69E313, 6, true); // Remove need for custom pl1400 and pl1500
		//injector::WriteMemory<unsigned char>(shared::base + 0x6C7EC3, 0xEB, true); // Disable vanilla enemy targeting (broken)

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
		return;
	}

	// if (MainPlayer) // early return removes need for this indent
	if (players[0] != MainPlayer) {
		for (int i = 0; i < 5; i++) {
			players[i] = nullptr;
			playerTypes[i] = (eObjID)0;
		}
		players[0] = MainPlayer;
		playerTypes[0] = MainPlayer->m_pEntity->m_nEntityIndex;
	}

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

		if (getKeyState('6').isPressed) {
			Spawner((eObjID)0x11400);
			//camera back to Raiden
			((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(MainPlayer);
		}

		if (getKeyState('5').isPressed) {
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

			bool CanDamagePlayer = ArmstrongCanDamagePlayer;

			if (Enemy->m_pEntity->m_nEntityIndex == 0x20020)
				CanDamagePlayer = BossSamCanDamagePlayer;
			
			FullHandleAIBoss(Enemy, controllerNumber, CanDamagePlayer);

		}
		

		if ((player->m_pEntity->m_nEntityIndex == (eObjID)0x11400 || player->m_pEntity->m_nEntityIndex == (eObjID)0x11500)
			&& modelItems) {
			modelItems->m_nHair = originalModelItems.m_nHair;
			modelItems->m_nVisor = originalModelItems.m_nVisor;
			modelItems->m_nSheath = originalModelItems.m_nSheath;
			modelItems->m_nHead = originalModelItems.m_nHead;
			FullHandleAIPlayer(player, controllerNumber, EnableDamageToPlayers);

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

			ImGui::EndTabBar();
		}
		ImGui::End();
		ImGui::EndFrame();
		ImGui::Render();
	}
}