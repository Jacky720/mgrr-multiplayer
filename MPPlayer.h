#pragma once
#include <Pl0000.h>
#include <BehaviorEmBase.h>
#include "MGRCustomUI.h"
#include "ModelItems.h"

#include <string>
#include <vector>
#include <map>

__declspec(selectany) std::vector<std::string> character_titles[] = {
	{"sam"},
	{"blade_wolf"},
	{"boss_sam"},
	{"sundowner", "sundowner_(second_phase)"},
	{"senator_armstrong_(shirt)", "senator_armstrong_(shirtless)"},
	{"raiden_(custom_body)", "raiden_(blue_body)", "raiden_(red_body)", "raiden_(yellow_body)",
	 "raiden_(desperado)", "raiden_(suit)", "raiden_(prologue)", "raiden_(original)",
	 "gray_fox", "raiden_(white_armor)", "raiden_(inferno_armor)", "raiden_(commando_armor)"}, // Mariachi omitted
	{"raiden_(unarmed)", "civilian_a", "civilian_b", "civilian_c", "civilian_d", "nmani"},
	{"dwarf_gekko"} };

// unsigned my ass
#define NILBODY 0xffffffff
__declspec(selectany) std::map<eObjID, std::vector<ModelItems>> characterModelItems = {
	{(eObjID)0x11400, { costumesList[Costumes::Sam] }},
	{(eObjID)0x11500, { {0x11505, 0, 0, 0, 0} }},
	{(eObjID)0x10010, { costumesList[Costumes::CustomBody],
						costumesList[Costumes::CustomBodyBlue],
						costumesList[Costumes::CustomBodyRed],
						costumesList[Costumes::CustomBodyYellow],
						costumesList[Costumes::DesperadoBody],
						costumesList[Costumes::Suit],
						costumesList[Costumes::StandardBody],
						costumesList[Costumes::OriginalBody],
						costumesList[Costumes::GrayFox],
						costumesList[Costumes::WhiteArmor],
						costumesList[Costumes::InfernoArmor],
						costumesList[Costumes::CommandoArmor] }},
	{(eObjID)0x10011, { {0x11010, 0x11011, 0x11014, 0x11013, 0x11017}, // Unarmed costumes
						{0x10800, NILBODY, NILBODY, 0x11013, NILBODY},
						{0x10801, NILBODY, NILBODY, 0x11013, NILBODY},
						{0x10a00, NILBODY, NILBODY, 0x11013, NILBODY},
						{0x10a01, NILBODY, NILBODY, 0x11013, NILBODY},
						{0x2031a, NILBODY, NILBODY, 0x11013, NILBODY} }}
						// Armstrong has custom model items, in the sense that he instead spawns em070a. See SpawnCharacter.
};

#define character_count (sizeof(character_titles) / sizeof(std::vector<std::string>))

#define maxPlayerCount 5
enum eDirection {
	Up,
	Left,
	Down,
	Right
};

class MPPlayer {
	static inline Pl0000* allPlayerObjs[maxPlayerCount];
public:
	eObjID playerType;
	union {
		Pl0000* playerObj;
		BehaviorEmBase* enemyObj;
		Behavior* dgObj;
	};
	int spawnFailTimer;
	ControllerFlags controllerFlag;
	int characterSelection;
	int costumeSelection[character_count];
	int healTimer;
	unsigned int prevPressed;
	bool isSundownerPhase2;
	bool sundownerInitialized;
	float dwarfGekkoAngle;
	bool zoomingCamera;
	int controlIndex;
	bool dpadInputs[4];
	static inline std::string dpadKeys[4];

	MPPlayer() {
		playerType = eObjID(0);
		playerObj = nullptr;
		spawnFailTimer = 0;
		controllerFlag = Out;
		characterSelection = 0;
		for (int& costume : costumeSelection)
			costume = 0;
		healTimer = -1;
		prevPressed = 0x0;
		isSundownerPhase2 = false;
		sundownerInitialized = false;
		dwarfGekkoAngle = 0.0;
		zoomingCamera = false;
		dpadInputs[Up] = false;
		dpadInputs[Left] = false;
		dpadInputs[Down] = false;
		dpadInputs[Right] = false;
	}

	static inline MPPlayer* players[5];

	static void InitPlayers() {
		dpadKeys[Up] = "XINPUT_GAMEPAD_DPAD_UP";
		dpadKeys[Left] = "XINPUT_GAMEPAD_DPAD_LEFT";
		dpadKeys[Down] = "XINPUT_GAMEPAD_DPAD_DOWN";
		dpadKeys[Right] = "XINPUT_GAMEPAD_DPAD_RIGHT";
		for (int i = 0; i < maxPlayerCount; i++) {
			players[i] = new MPPlayer();
			players[i]->controlIndex = i;
		}
	}

	static void EmptyPlayers() {
		for (MPPlayer* player : players) {
			player->playerObj = nullptr;
			player->playerType = eObjID(0);
		}
	}
	
	static void FixIndexes() {
		for (int i = 0; i < maxPlayerCount; i++) {
			players[i]->controlIndex = i;
			players[i]->controllerFlag = (players[i]->playerType) ? In : Out;
		}
	}

	bool TryingToSpawn() {
		return playerType && !playerObj;
	}

	static Pl0000** GetPlayerObjs() {
		for (int i = 0; i < maxPlayerCount; i++) {
			allPlayerObjs[i] = players[i]->playerObj;
		}
		return allPlayerObjs;
	}
};
#define players MPPlayer::players