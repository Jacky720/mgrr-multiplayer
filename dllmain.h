#pragma once
#include <Pl0000.h>
#include <cGameUIManager.h>
#include "ModelItems.h"
#include "MPPlayer.h"

#include <string>
#include <vector>
#include <map>
#include <set>

//bool handleKeyPress(int hotKey, bool* isMenuShowPtr);
void RecalibrateBossCode();
void TeleportToMainPlayer(MPPlayer* mainPlayer, int controllerIndex = -1);
void TeleportToMainPlayer(Pl0000* mainPlayer, int controllerIndex = -1);
void SpawnCharacter(int id, int controller, int costumeID = 0);
int giveVanillaCameraControl(Pl0000* player);

__declspec(selectany) unsigned int HotKey = VK_INSERT; //Hotkey for menu show

__declspec(selectany) bool isMenuShow = false;
__declspec(selectany) bool PlayAsMistral = false;
__declspec(selectany) bool PlayAsMonsoon = false;
__declspec(selectany) bool PlayAsSundowner = false;
__declspec(selectany) bool PlayAsSam = false;
__declspec(selectany) bool PlayAsArmstrong = false;
__declspec(selectany) bool EnableFriendlyFire = false;
__declspec(selectany) bool p1IsKeyboard = true;

__declspec(selectany) Pl0000* MainPlayer = cGameUIManager::Instance.m_pPlayer;

__declspec(selectany) bool isInit = false;
__declspec(selectany) float maxAllowedDist = 15.0f;
__declspec(selectany) std::set<Pl0000*> playerDestroyQueue;
__declspec(selectany) float hudScale = 0.9;