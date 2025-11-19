#pragma once
#include <Pl0000.h>
#include <BehaviorEmBase.h>
#include "MGRCustomUI.h"
#include "ModelItems.h"

#include <string>
#include <vector>
#include <map>

enum eSpecialSpawn {
	Default,
	SundownerPhase2,
	SpawnUnarmed,
	StockFenrir
};

struct SpawnOption {
	std::string optionName;
	std::string gameName;
	eObjID objID;
	ModelItems subParts;
	int setType;
	eSpecialSpawn special;
};

// unsigned my ass
#define NILBODY 0xffffffff
__declspec(selectany) std::vector<SpawnOption> spawnOptions[] = {
	{ {"sam_(human)",  "sam", eObjID(0x11400), costumesList[Costumes::Sam]},
	  {"sam_(cyborg)", "sam", eObjID(0x11400), {eObjID(0x20020), eObjID(0x11401), eObjID(0x20022), eObjID(0x11404), eObjID(0x11405)}} },

	{ {"blade_wolf", "blade_wolf", eObjID(0x11500), costumesList[Costumes::LQ84i]},
	  {"fenrir",     "fenrir",     eObjID(0x11500), costumesList[Costumes::LQ84i], 0, StockFenrir}},

	{ {"boss_sam", "jetstream_sam", eObjID(0x20020)} },

	{ {"sundowner",                "sundowner", eObjID(0x20310)},
	  {"sundowner_(second_phase)", "sundowner", eObjID(0x20310), {}, 0, SundownerPhase2} },

	{ {"senator_armstrong_(shirt)",     "senator", eObjID(0x20700)},
	  {"senator_armstrong_(shirtless)", "senator", eObjID(0x2070A)}},

	{ {"raiden_(custom_body)",    "raiden", eObjID(0x10010), costumesList[Costumes::CustomBody]},
	  {"raiden_(blue_body)",      "raiden", eObjID(0x10010), costumesList[Costumes::CustomBodyBlue]},
	  {"raiden_(red_body)",       "raiden", eObjID(0x10010), costumesList[Costumes::CustomBodyRed]},
	  {"raiden_(yellow_body)",    "raiden", eObjID(0x10010), costumesList[Costumes::CustomBodyYellow]},
	  {"raiden_(desperado)",      "raiden", eObjID(0x10010), costumesList[Costumes::DesperadoBody]},
	  {"raiden_(suit)",           "raiden", eObjID(0x10010), costumesList[Costumes::Suit]},
	  // {"raiden_(mariachi)",    "raiden", eObjID(0x10010), costumesList[Costumes::Mariachi]}, // Different hair spawning, wouldn't work
	  {"raiden_(prologue)",       "raiden", eObjID(0x10010), costumesList[Costumes::PrologueBody]},
	  {"raiden_(original)",       "raiden", eObjID(0x10010), costumesList[Costumes::OriginalBody]},
	  {"gray_fox",                "gray_fox", eObjID(0x10010), costumesList[Costumes::GrayFox]},
	  {"raiden_(white_armor)",    "raiden", eObjID(0x10010), costumesList[Costumes::WhiteArmor]},
	  {"raiden_(inferno_armor)",  "raiden", eObjID(0x10010), costumesList[Costumes::InfernoArmor]},
	  {"raiden_(commando_armor)", "raiden", eObjID(0x10010), costumesList[Costumes::CommandoArmor]} },

	{ {"raiden_(unarmed)", "raiden",   eObjID(0x10010), {0x11010, 0x11011, 0x11014, 0x11083, 0x11017}, 0, SpawnUnarmed}, // Desperado sheath for no light
	  {"civilian_a",       "civilian", eObjID(0x10010), {0x10800, NILBODY, NILBODY, 0x11083, NILBODY}, 0, SpawnUnarmed},
	  {"civilian_b",       "civilian", eObjID(0x10010), {0x10801, NILBODY, NILBODY, 0x11083, NILBODY}, 0, SpawnUnarmed},
	  {"civilian_c",       "civilian", eObjID(0x10010), {0x10a00, NILBODY, NILBODY, 0x11083, NILBODY}, 0, SpawnUnarmed},
	  {"civilian_d",       "civilian", eObjID(0x10010), {0x10a01, NILBODY, NILBODY, 0x11083, NILBODY}, 0, SpawnUnarmed},
	  {"nmani",            "nmani",    eObjID(0x10010), {0x2031a, NILBODY, NILBODY, 0x11083, NILBODY}, 0, SpawnUnarmed} },

	{ {"dwarf_gekko", "dwarf_gekko", eObjID(0x12040), {}, 1} }
};

#define character_count (sizeof(spawnOptions) / sizeof(std::vector<SpawnOption>))

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
	std::string playerName;
	bool unarmed;
	bool stockFenrir;

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
		playerName = "";
		unarmed = false;
		stockFenrir = false;
	}

	static inline MPPlayer* players[maxPlayerCount];

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