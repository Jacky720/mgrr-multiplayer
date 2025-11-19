#include "gui.h"
#include "Camera.h"
#include "dllmain.h"
#include "MGRCustomAI.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include <injector/injector.hpp>
#include <Hw.h>

#include <cGameUIManager.h>
#include <EntitySystem.h>
#include <GameMenuStatus.h>
#include <Pl0000.h>
#include <Trigger.h>
#include "MGRFunctions.h"

#include <format>

extern Sub_18AE10_t sundownerPhase2Create;

void gui::OnReset::Before()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void gui::OnReset::After()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::OnEndScene()
{

}

void gui::LoadStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	
	// your style settings here
}

// test


void gui::RenderWindow()
{
	static bool wasHotKeyPressed = false; // Ñòàòè÷åñêàÿ ïåðåìåííàÿ, ñîõðàíÿåò ñîñòîÿíèå ìåæäó âûçîâàìè

	if (GetKeyState(HotKey) & 0x8000 && !wasHotKeyPressed) {
		isMenuShow = !isMenuShow;
	}
	wasHotKeyPressed = GetKeyState(HotKey) & 0x8000;

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

				ImGui::Checkbox("Armstrong is player-controlled", &PlayAsArmstrong);
				ImGui::Checkbox("Boss Sam is player-controlled", &PlayAsSam);
				ImGui::Checkbox("Sundowner is player-controlled", &PlayAsSundowner);
				//ImGui::Checkbox("Player 2 is Monsoon", &PlayAsMonsoon);
				//ImGui::Checkbox("Player 2 is Mistral", &PlayAsMistral);

				ImGui::Checkbox("Allow friendly fire", &EnableFriendlyFire);
				ImGui::Checkbox("Player 1 uses keyboard (else Controller 1)", &p1IsKeyboard);

				// Sundowner's Head: 1581929


				RecalibrateBossCode();

				/*if (MainPlayer && ImGui::Button("Teleport all players to Raiden")) {
					TeleportToMainPlayer(MainPlayer);
				}*/
				ImGui::InputFloat("Maximum diameter between players", &maxAllowedDist);

				ImGui::Checkbox("All players can heal (30 second cooldown)", &EveryHeal);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Current Players")) {
				char p1Label[] = " (Main player)";
				ImGui::Text("Keyboard%s: %x\n", p1IsKeyboard ? p1Label : "", playerTypes[0]);
				if (players[0]) {
					if (ImGui::Button("Teleport all players to keyboard player")) TeleportToMainPlayer(players[0]);
					if (!customCamera && ImGui::Button("Move camera to keyboard player")) giveVanillaCameraControl(players[0]);
				}
				for (int i = 1; i <= 4; i++) {
					ImGui::Text("Controller %d%s: %x\n", i, (!p1IsKeyboard && i == 1) ? p1Label : "", playerTypes[i]);
					if (players[i]) {
						if (ImGui::Button(std::format("Teleport all players to controller {} player", i).c_str())) TeleportToMainPlayer(players[i]);
						if (!customCamera &&
							ImGui::Button(std::format("Move camera to controller {} player", i).c_str())) giveVanillaCameraControl(players[i]);
					}
				}

//#define PRINTACTIONS
#ifdef PRINTACTIONS
				ImGui::Text("");
				for (int i = 0; i < 5; i++) {
					if (players[i])
						ImGui::Text("Player %d action: %x %x", i + 1, players[i]->getCurrentAction(), players[i]->getCurrentActionId());
				}
#endif
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Camera")) {
				ImGui::Checkbox("Custom camera", &customCamera);
				if (customCamera) {
					ImGui::Checkbox("Vanilla camera for QTEs", &qteCamera);
					ImGui::InputDouble("Camera sensitivity", &camSensitivity);
					ImGui::Checkbox("Allow vertical camera movement", &enableCameraY);
					ImGui::Checkbox("Invert vertical camera movement", &invertCameraY);
					ImGui::InputDouble("Camera lateral distance", &camLateralScale);
					ImGui::InputDouble("Camera vertical distance", &camHeightScale);
					if (enableCameraY) {
						ImGui::InputDouble("Minimum lateral distance", &camLateralMin);
						ImGui::InputDouble("Maximum lateral distance", &camLateralMax);
						ImGui::InputDouble("Minimum vertical distance", &camHeightMin);
						ImGui::InputDouble("Maximum vertical distance", &camHeightMax);
					}
					ImGui::InputDouble("Camera zoom-in FOV", &zoomInFOV);
					ImGui::InputDouble("Camera zoom-out FOV", &zoomOutFOV);
				}
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Sundowner")) {
				
				for (int i = 0; i < 5; i++) {
					if (!players[i]) continue;

					if (players[i]->m_ObjId == 0x20310) {
						ImGui::Text("Sundowner %i", i);
						ImGui::SameLine();
						if (ImGui::Button("Phase 2-ify")) {
							sundownerPhase2Create((Behavior*)players[i], 0);
						}
						ImGui::SameLine();
						ImGui::Checkbox("Is Controller ID Phase 2", &isControllerIDSundownerPhase2[i]);
					}



				}
				


				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Dev")) {
				static int memory_address = 0x0;
				ImGui::InputInt("Memory Address:", &memory_address);
				if (ImGui::Button("NOP Memory Address") && MainPlayer) {
					injector::MakeNOP(shared::base + memory_address, 3, true);
				}
				auto firstEnt = EntitySystem::ms_Instance.m_EntityList.m_pFirst;
				ImGui::Text("First entity pointer: 0x%x", (unsigned int)firstEnt);

				// Debug print stuff
//#define PRINTSAM
//#define PRINTENEMY
//#define SHOWBOSSACTION
//#define PRINTBMs
//#define PRINTPHASE
#ifdef PRINTPHASE
				int** gScenarioManagerImplement = *(int***)(shared::base + 0x17E9A30);
				int phase = 0;

				if (gScenarioManagerImplement && gScenarioManagerImplement[45])
				{
					int* phaseHandle = gScenarioManagerImplement[45];
					if (phaseHandle)
						phase = phaseHandle[1];
				}
				ImGui::Text("Phase p%3x", phase);
#endif

				for (auto value : EntitySystem::ms_Instance.m_EntityList) {
					if (!value || value == (Entity*)0xEFEFEFEF) continue;
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
					if ((value->m_EntityIndex & 0xF0000) == 0x20000) {
						//ImGui::InputInt("the fuck?", (int*)&node->m_value->m_nEntityIndex, 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
						int twerpsenemyflag = (int)&Enemy->m_pEnemy;
						ImGui::Text("Enemy: 0x%x", value->m_EntityIndex);
						ImGui::InputInt("twerp's enemy flag", &twerpsenemyflag, 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
						ImGui::InputInt("enemy HP", &Enemy->m_nHealth);
						ImGui::InputFloat("enemy x scale", &Enemy->m_vecSize.x);
						//ImGui::InputInt("Some twerp", (int*)&(Enemy->m_pEnemy->m_pEntity->m_EntityIndex), 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
					}
#endif
#ifdef SHOWBOSSACTION
					auto Enemy = value->getEntityInstance<BehaviorEmBase>();
					if (!Enemy) continue;
					if (value->m_nEntityIndex == 0x20020 || value->m_nEntityIndex == 0x20700) {
						ImGui::Text("Entity %x has state %x", value->m_nEntityIndex, Enemy->m_nCurrentAction);
					}
#endif
#ifdef PRINTBMs
					if ((value->m_nEntityIndex & 0xf0000) == 0xD0000) {
						ImGui::Text("Entity %x", value->m_nEntityIndex);
					}
#endif
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