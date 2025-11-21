#pragma once
#include <Pl0000.h>
#include <BehaviorEmBase.h>
#include <PlayerManagerImplement.h>
#include "MGRCustomUI.h"
#include "MGRFunctions.h"
#include "ModelItems.h"

#include <string>
#include <vector>
#include <map>

enum eSpecialSpawn {
	Default,
	SundownerPhase2,
	SpawnUnarmed
};

struct SpawnOption {
	std::string optionName;
	std::string gameName;
	eObjID objID;
	ModelItems subParts;
	eObjID sword;
	eSpecialSpawn special;
	std::vector<std::string> disableMeshes;
	int setType;
};

// unsigned my ass
#define NILBODY 0xffffffff
__declspec(selectany) std::vector<SpawnOption> spawnOptions[] = {
	{ {"Sam (human)",  "Sam", eObjID(0x11400), costumesList[Sam], eObjID(0x11403)},
	  {"Sam (cyborg)", "Sam", eObjID(0x11400), {0x20020, 0x11401, 0x20022, 0x11404, 0x11405}, eObjID(0x11403), Default, {"dam1"}} },

	{ {"Blade Wolf", "Blade Wolf", eObjID(0x11500), costumesList[LQ84i], eObjID(0x11501)},
	  {"Fenrir",     "Fenrir",     eObjID(0x11500), costumesList[LQ84i], eObjID(0x11501), Default, {"cover_a_DEC", "faceArmor", "EFD"}}},

	{ {"Boss Sam", "Jetstream Sam", eObjID(0x20020)} },

	{ {"Sundowner",                "Sundowner", eObjID(0x20310)},
	  {"Sundowner (second phase)", "Sundowner", eObjID(0x20310), {}, eObjID(0), SundownerPhase2}},

	{ {"Senator Armstrong (shirt)",     "Senator", eObjID(0x20700)},
	  {"Senator Armstrong (shirtless)", "Senator", eObjID(0x2070A)} },

	{ {"Raiden (Custom Body)",    "Raiden", eObjID(0x10010), costumesList[CustomBody], eObjID(0x11012)},
	  {"Raiden (Blue Body)",      "Raiden", eObjID(0x10010), costumesList[CustomBodyBlue], eObjID(0x11012)},
	  {"Raiden (Red Body)",       "Raiden", eObjID(0x10010), costumesList[CustomBodyRed], eObjID(0x11012)},
	  {"Raiden (Yellow Body)",    "Raiden", eObjID(0x10010), costumesList[CustomBodyYellow], eObjID(0x11012)},
	  {"Raiden (Desperado)",      "Raiden", eObjID(0x10010), costumesList[DesperadoBody], eObjID(0x11012)},
	  {"Raiden (Suit)",           "Raiden", eObjID(0x10010), costumesList[Suit], eObjID(0x11012)},
	  // {"Raiden (Mariachi)",    "Raiden", eObjID(0x10010), costumesList[Mariachi], eObjID(0x11012)}, // Different hair spawning, wouldn't work
	  {"Raiden (Prologue)",       "Raiden", eObjID(0x10010), costumesList[PrologueBody], eObjID(0x11012)},
	  {"Raiden (Original)",       "Raiden", eObjID(0x10010), costumesList[OriginalBody], eObjID(0x11012)},
	  {"Gray Fox",                "Gray Fox", eObjID(0x10010), costumesList[GrayFox], eObjID(0x11012)},
	  {"Raiden (White Armor)",    "Raiden", eObjID(0x10010), costumesList[WhiteArmor], eObjID(0x11012)},
	  {"Raiden (Inferno Armor)",  "Raiden", eObjID(0x10010), costumesList[InfernoArmor], eObjID(0x11012)},
	  {"Raiden (Commando Armor)", "Raiden", eObjID(0x10010), costumesList[CommandoArmor], eObjID(0x11012)} },

	{ // 0x11083 (Desperado sheath) to avoid sheath light
	  {"Raiden (Unarmed)", "Raiden",   eObjID(0x10010), {0x11010, 0x11011, 0x11014, 0x11083, 0x11017}, eObjID(0x11012), SpawnUnarmed}, // Desperado sheath for no light
	  {"Civilian A",       "Civilian", eObjID(0x10010), {0x10800, NILBODY, NILBODY, 0x11083, NILBODY}, eObjID(0x11012), SpawnUnarmed, {"skin_in"}},
	  {"Civilian B",       "Civilian", eObjID(0x10010), {0x10801, NILBODY, NILBODY, 0x11083, NILBODY}, eObjID(0x11012), SpawnUnarmed, {"skin_in"}},
	  {"Civilian C",       "Civilian", eObjID(0x10010), {0x10a00, NILBODY, NILBODY, 0x11083, NILBODY}, eObjID(0x11012), SpawnUnarmed, {"skin_in"}},
	  {"Civilian D",       "Civilian", eObjID(0x10010), {0x10a01, NILBODY, NILBODY, 0x11083, NILBODY}, eObjID(0x11012), SpawnUnarmed, {"skin_in"}},
	  {"Nmani",            "Nmani",    eObjID(0x10010), {0x2031a, NILBODY, NILBODY, 0x11083, NILBODY}, eObjID(0x11012), SpawnUnarmed} },

	{ {"Dwarf Gekko", "Dwarf Gekko", eObjID(0x12040), {}, eObjID(0), Default, {}, 1} },

	{ {"Your Custom Skin Here", ""} } // Empty string blocks drop-in code
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
	enum eSunP2Init {
		Uninitialized,
		PlayingAnimation,
		Initialized
	} sundownerPhase2Init;
	float dwarfGekkoAngle;
	bool zoomingCamera;
	int controlIndex;
	bool dpadInputs[4];
	std::string playerName;
	bool unarmed;
	std::vector<std::string> disableMeshes;

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
		sundownerPhase2Init = Uninitialized;
		dwarfGekkoAngle = 0.0;
		zoomingCamera = false;
		dpadInputs[Up] = false;
		dpadInputs[Left] = false;
		dpadInputs[Down] = false;
		dpadInputs[Right] = false;
		playerName = "";
		unarmed = false;
		disableMeshes = {};
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

	void DropOut() {
		controllerFlag = Out;
		characterSelection = 0;
		Se_PlayEvent("core_se_sys_decide_l");
		if ((playerType & 0xf0000) == 0x20000) {
			// Go directly to hell
			playerObj->place({ 0, -10000, 0, 0 }, { 0, 0, 0, 0 });
		}
		else { // TODO: "destruction queue" (it crashes if unarmed drops)
			playerObj->m_pEntity->~Entity();
			// playerDestroyQueue.insert(player->playerObj); // Still crashes nvm
		}
		playerObj = nullptr;
		playerType = (eObjID)0;
	}

	void Disarm() {
		if (playerType != eObjID(0x10010)) return;
		playerObj->setSwordLost(true);
		playerObj->m_SwordState = 1;
		// TODO: need to load, heck preload wp2040 to keep this from crashing
		*selectedCustomWeapon = &validCustomWeapons[CustomWeapons::Unarmed];
		playerObj->rebuildCustomWeapon(); // (Thanks Genos)
		PlayerManagerImplement::ms_Instance->setCustomWeaponEquipped(5); // Unarmed
		playerObj->setIdle(0);
		unarmed = true;
	}

	void Arm() {
		if (playerType != eObjID(0x10010)) return;
		playerObj->setSwordLost(false);
		playerObj->m_SwordState = 0;
		playerObj->setIdle(0);
		unarmed = false;
	}

	void ToggleArmed() {
		if (unarmed)
			Arm();
		else
			Disarm();
	}

	static Pl0000** GetPlayerObjs() {
		for (int i = 0; i < maxPlayerCount; i++) {
			allPlayerObjs[i] = players[i]->playerObj;
		}
		return allPlayerObjs;
	}
};
#define players MPPlayer::players