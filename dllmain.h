#pragma once
#include <Pl0000.h>

#include <string>
#include <vector>

//bool handleKeyPress(int hotKey, bool* isMenuShowPtr);
void RecalibrateBossCode();
void TeleportToMainPlayer(Pl0000* mainPlayer, int controllerIndex = -1);
void SpawnCharacter(int id, int controller, int costumeIndex = 0);

extern unsigned int HotKey;
extern bool isMenuShow;
extern bool PlayAsMistral;
extern bool PlayAsMonsoon;
extern bool PlayAsSundowner;
extern bool PlayAsSam;
extern bool PlayAsArmstrong;
extern bool EnableFriendlyFire;
extern bool p1IsKeyboard;
extern eObjID playerTypes[5];
extern Pl0000* MainPlayer;
extern Pl0000* players[5];
extern std::vector<std::string> character_titles[7];
#define character_count (sizeof(character_titles) / sizeof(std::vector<std::string>))
extern bool isInit;
extern float maxAllowedDist;