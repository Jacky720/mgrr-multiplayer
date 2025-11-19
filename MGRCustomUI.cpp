#include <d3dx9.h>
#include <cGameUIManager.h>
#include <Hw.h>
#include <Pl0000.h>
#include <BehaviorEmBase.h>
#include <PlayerManagerImplement.h>
#include <cItemPossessionCure.h>
#include <filesystem>
#include <map>
#include <string>
#include <XInput.h>
#include "dllmain.h"
#include "MGRControls.h"
#include "MGRCustomUI.h"
#include "MGRFunctions.h"
#include "MPPlayer.h"
#include "imgui/imgui.h"
using namespace std;

namespace fs = std::filesystem;

int checkScreenSizeTimer = 60;

class MGRFontCharacter {
public:
	char character;
	LPDIRECT3DTEXTURE9 sprite;
	int width;

};

std::map<char, MGRFontCharacter> font_map[2];

LPDIRECT3DTEXTURE9 fc_segment;
LPDIRECT3DTEXTURE9 hp_segment;


// 0-1 Player menus, 
LPDIRECT3DTEXTURE9 radial_assets[4];

void LoadFont(fs::path directory_path, int id) {
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	int loaded_font_textures = 0;
	if (!fs::exists(directory_path)) {
		MessageBoxA(nullptr, "Failed to load GameData\\rising_multiplayer!\nPlease ensure you copied to required folders and try again.", "Rising Multiplayer", MB_ICONERROR | MB_OK);
	}

	for (const auto& entry : fs::directory_iterator(directory_path)) {
		if (entry.is_regular_file()) {

			pTexture = NULL;
			string fname = entry.path().string();
			D3DXIMAGE_INFO info;
			D3DXCreateTextureFromFileEx(
				Hw::GraphicDevice,
				fname.c_str(),
				D3DX_DEFAULT_NONPOW2,    // Width
				D3DX_DEFAULT_NONPOW2,    // Height
				D3DX_DEFAULT,            // MipLevels
				0,                       // Usage
				D3DFMT_UNKNOWN,          // Format
				D3DPOOL_MANAGED,         // Pool
				D3DX_FILTER_NONE,        // Filter
				D3DX_FILTER_NONE,        // MipFilter
				0,                       // ColorKey
				NULL,                    // pSrcInfo
				NULL,                    // pPalette
				&pTexture
			);

			D3DXGetImageInfoFromFile(fname.c_str(), &info);
			if (pTexture != NULL) {
				int n = fname.length();
				char* arr = new char[n + 1];
				fname = fname.substr(fname.find_last_of("\\"), fname.find_last_of("."));
				strcpy_s(arr, n + 1, fname.c_str());
				MGRFontCharacter character;
				character.sprite = pTexture;
				character.character = arr[1];
				character.width = info.Width;
				font_map[id].insert({ arr[1], character });
				loaded_font_textures++;
			}
		}
	}
}





void LoadUIData() {

	string data_dir = "rising_multiplayer\\";

	LoadFont("rising_multiplayer\\mgfont\\", 0);
	LoadFont("rising_multiplayer\\mgfont_2\\", 1);



	D3DXCreateTextureFromFile(Hw::GraphicDevice, (data_dir + "\\ui\\fc_seg.png").c_str(), &fc_segment);
	D3DXCreateTextureFromFile(Hw::GraphicDevice, (data_dir + "\\ui\\hp_seg.png").c_str(), &hp_segment);



}


void DrawLine(LPDIRECT3DDEVICE9 Device_Interface, int bx, int by, int bw, D3DCOLOR COLOR, int thickness = 6)
{
	D3DRECT rec;
	rec.x1 = bx;
	rec.y1 = by;
	rec.x2 = bx + bw;//makes line longer/shorter going right
	rec.y2 = by + thickness;//makes line one pixel tall
	Device_Interface->Clear(1, &rec, D3DCLEAR_TARGET, COLOR, 0, 0);

}

void RenderTextMGR(string text, int x, int y, D3DCOLOR color, int fontid = 0, float scale = 1.0) {
	// Copy string to a cstring array and then draw using the map, which allows for infinite expansion, just add the character to the mgfonts folder

	int n = text.length();
	char* txtarray = new char[n + 1];
	strcpy_s(txtarray, n + 1, text.c_str());
	D3DXMATRIX scaleMat = {
		scale, 0, 0, 0,
		0, scale, 0, 0,
		0, 0, scale, 0,
		0, 0, 0, 1
	};

	pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	pSprite->SetTransform(&scaleMat);
	int tmp_x_shift = 0;
	for (int i = 0; i < n + 1; i++) {
		D3DXVECTOR3 position((float)(x + tmp_x_shift) / scale, (float)y / scale, 0.0f);
		if (txtarray[i] != NULL) {
			// Handle missing characters more gracefully
			if (txtarray[i] == '<') txtarray[i] = '(';
			if (txtarray[i] == '>') txtarray[i] = ')';
			if (txtarray[i] == ' ') txtarray[i] = '_';
			if (txtarray[i] >= 'A' && txtarray[i] <= 'Z') txtarray[i] += 'a' - 'A';
			pSprite->Draw(font_map[fontid][txtarray[i]].sprite, NULL, NULL, &position, color);
			tmp_x_shift += (int)((font_map[fontid][txtarray[i]].width - 8) * scale);


		}
		else {
			tmp_x_shift += (int)(20 * scale);
		}

	}
	pSprite->End();

	delete[] txtarray;

}

void RenderTextMGR_RightLeft(string text, int x, int y, D3DCOLOR color, int fontid = 0, float scale = 1.0) {
	// Copy string to a cstring array and then draw using the map, which allows for infinite expansion, just add the character to the mgfonts folder
	int n = text.length();
	char* txtarray = new char[n + 1];
	strcpy_s(txtarray, n + 1, text.c_str());
	D3DXMATRIX scaleMat = {
		scale, 0, 0, 0,
		0, scale, 0, 0,
		0, 0, scale, 0,
		0, 0, 0, 1
	};

	pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	pSprite->SetTransform(&scaleMat);
	int tmp_x_shift = 0;
	for (int i = n - 1; i >= 0; i--) {
		tmp_x_shift -= (int)((font_map[fontid][txtarray[i]].width - 8) * scale); // Shift BEFORE since we're going left, THROUGH the character's space
		D3DXVECTOR3 position((float)(x + tmp_x_shift) / scale, (float)y / scale, 0.0f);
		if (txtarray[i] != NULL) {
			pSprite->Draw(font_map[fontid][txtarray[i]].sprite, NULL, NULL, &position, color);
		}

	}
	pSprite->End();

	delete[] txtarray;

}

#define C_BLACK D3DCOLOR_XRGB(0, 0, 0)
#define C_LTGRAY D3DCOLOR_XRGB(240, 255, 255)
#define C_LTGRAYFADE D3DCOLOR_ARGB(100, 240, 255, 255)
#define C_CYAN D3DCOLOR_XRGB(0, 255, 255)
#define C_DKGRAY D3DCOLOR_XRGB(30, 30, 30)
#define C_HPYELLOW D3DCOLOR_XRGB(255, 227, 66)
#define C_RED D3DCOLOR_XRGB(255, 100, 100)
#define C_FCYELLOW D3DCOLOR_XRGB(255, 245, 55)
#define LEFT_JUSTIFIED 0
#define RIGHT_JUSTIFIED 1

void RenderTextWithShadow(string text, int x, int y, D3DCOLOR bg = C_BLACK, D3DCOLOR fg = C_LTGRAY, int fontid = 0, int justification_flag = LEFT_JUSTIFIED, float scale = 1.0) {
	static int offsets[9][2] = {
	{-1, -1},
	{-1, 0},
	{-1, 1},
	{0, -1},
	//{0, 0},
	{0, 1},
	{1, -1},
	{1, 0},
	{1, 1},
	};

	if (justification_flag != LEFT_JUSTIFIED && justification_flag != RIGHT_JUSTIFIED)
		return; // Can't render

	auto renderFunc = (justification_flag == LEFT_JUSTIFIED) ? RenderTextMGR : RenderTextMGR_RightLeft;

	// Draw shadow
	for (int i = 0; i < 8; i++) {
		renderFunc(text, x + offsets[i][0], y + offsets[i][1], bg, fontid, scale);
	}

	// Draw text
	renderFunc(text, x, y, fg, fontid, scale);

}

void DrawProgressBar(int x, int y, float value, float maxvalue, D3DCOLOR fg, int barlength = 300, D3DCOLOR bg = C_DKGRAY) {
	DrawLine(Hw::GraphicDevice, x, y, barlength, bg, 8);
	DrawLine(Hw::GraphicDevice, x, y, (int)(barlength * value / maxvalue), fg, 8);
}

void DrawFalseMGRUI(int x, int y, float hpvalue, float hpmax, float truehpmax, float fcvalue, float fcmax, string name, bool ripper) {
	// Health bar length. 100% = 300, 200% = 750 (2.5x)
	int hpBarLength = (int)(300 + (truehpmax / hpmax - 1) * 450);
	int decimalplace = static_cast<int>(((hpvalue / hpmax) * 100) * 10) % 10;
	// e.g. [100.][0 %], with coordinates justified between the ][
	RenderTextWithShadow(to_string((int)floor(100 * hpvalue / hpmax)) + ".", x + hpBarLength - 50, y - 25, C_DKGRAY, C_HPYELLOW, 0, RIGHT_JUSTIFIED);
	RenderTextWithShadow(to_string(decimalplace) + " %", x + hpBarLength - 50, y - 5, C_DKGRAY, C_HPYELLOW, 1, LEFT_JUSTIFIED);

	RenderTextWithShadow(name, x, y - 8, C_BLACK, C_LTGRAY, 0, LEFT_JUSTIFIED, 1.5);
	DrawProgressBar(x, y + 23, hpvalue, truehpmax, C_HPYELLOW, hpBarLength);
	auto fcCol = C_CYAN;
	if (fcvalue < 400) fcCol = C_FCYELLOW;
	if (ripper) fcCol = C_RED;
	if (fcmax > 0) {
		DrawProgressBar(x, y + 32, min(fcvalue, 400), 400, fcCol);
	}
	for (int fcOff = 400, xOff = 305; fcmax > fcOff; fcOff += 280, xOff += 90) {
		DrawProgressBar(x + xOff, y + 32, (float)clamp((int)(fcvalue - fcOff), 0, 280), 280, fcCol, 85);
	}


}

void DrawCharacterSelector(int offset_x, int y, int controller_id) {
	// Render character selection
	if (checkScreenSizeTimer > 0) {
		RECT rect;
		checkScreenSizeTimer--;
		if (checkScreenSizeTimer == 0
		    && GetWindowRect(Hw::OSWindow, &rect))
		{
			

			screenWidth = rect.right - rect.left;
			screenHeight = rect.bottom - rect.top;
			checkScreenSizeTimer = -1;
		}

		if (checkScreenSizeTimer == 0) checkScreenSizeTimer = 60;
	}


	string numbername;
	switch (controller_id) {
	case 0: numbername = "Zero"; break;
	case 1: numbername = "One"; break;
	case 2: numbername = "Two"; break;
	case 3: numbername = "Three"; break;
	case 4: numbername = "Four"; break;
	case 5: numbername = "Five"; break;
	case 6: numbername = "Six"; break;
	case 7: numbername = "Seven"; break;
	case 8: numbername = "Eight"; break;
	case 9: numbername = "Nine"; break;
	default: numbername = "(Unknown)";
	}

	MPPlayer* player = players[controller_id];

	bool freshDpad[4] = {false, false, false, false}; // Up, left, down, right
	for (int i = 0; i < 4; i++) {
		bool newState = IsGamepadButtonPressed(controller_id, MPPlayer::dpadKeys[i]);
		if (!player->dpadInputs[i]) { // Do not set freshDpad if the existing state was already on (no input repeating, basically)
			freshDpad[i] = newState;
		}
		player->dpadInputs[i] = newState;
	}

	
	if (freshDpad[Up]) {
		player->characterSelection--;
		if (player->characterSelection < 0) {
			player->characterSelection = character_count - 1;
		}
		Se_PlayEvent("core_se_sys_custom_item_window_corsor");
	}

	if (freshDpad[Down]) {
		player->characterSelection++;
		if (player->characterSelection >= character_count) {
			player->characterSelection = 0;
		}
		Se_PlayEvent("core_se_sys_custom_item_window_corsor");
	}

	int curChara = player->characterSelection;

	if (freshDpad[Left]) {
		player->costumeSelection[curChara]--;
		if (player->costumeSelection[curChara] < 0) {
			player->costumeSelection[curChara] = spawnOptions[curChara].size() - 1;
		}
		Se_PlayEvent("core_se_sys_custom_item_window_corsor");
	}

	if (freshDpad[Right]) {
		player->costumeSelection[curChara]++;
		if (player->costumeSelection[curChara] >= (signed)spawnOptions[curChara].size()) {
			player->costumeSelection[curChara] = 0;
		}
		Se_PlayEvent("core_se_sys_custom_item_window_corsor");
	}
	

	RenderTextWithShadow("Player " + numbername + " joining as", screenWidth - offset_x, y, C_BLACK, C_HPYELLOW, 0, RIGHT_JUSTIFIED);

	for (int i = 0; i < character_count; i++) {
		auto fgCol = C_LTGRAYFADE;
		if (i == player->characterSelection) {
			fgCol = C_LTGRAY;
		}
		
		RenderTextWithShadow(spawnOptions[i][player->costumeSelection[i]].optionName, screenWidth - offset_x, y + 20 + i * 20, C_BLACK, fgCol, 0, RIGHT_JUSTIFIED);
	}
	
	//RenderTextMGR_RightLeft("sundowner", screenWidth - offset_x, y + 60, C_LTGRAY, 0);
	//RenderTextMGR_RightLeft("raiden", screenWidth - offset_x, y + 80, C_LTGRAYFADE, 0);

}

void DrawDropMenu(int offset_x, int y, int controller_id) {
	string numbername;
	switch (controller_id) {
	case 0: numbername = "Zero"; break;
	case 1: numbername = "One"; break;
	case 2: numbername = "Two"; break;
	case 3: numbername = "Three"; break;
	case 4: numbername = "Four"; break;
	case 5: numbername = "Five"; break;
	case 6: numbername = "Six"; break;
	case 7: numbername = "Seven"; break;
	case 8: numbername = "Eight"; break;
	case 9: numbername = "Nine"; break;
	default: numbername = "(Unknown)";
	}

	RenderTextWithShadow("Player " + numbername + " Pause", screenWidth - offset_x, y, C_BLACK, C_HPYELLOW, 0, RIGHT_JUSTIFIED);

	RenderTextWithShadow("Drop", screenWidth - offset_x, y + 20, C_BLACK, C_LTGRAY, 0, RIGHT_JUSTIFIED);
}

void ResetControllerAllFlags() {
	for (MPPlayer* player : players) {
		player->controllerFlag = Out;
	}
}

BOOL __stdcall WorldToScreen(const cVec4& worldPosition, cVec4& screenPos)
{
	return ((BOOL(__stdcall*)(const cVec4&, cVec4&))(shared::base + 0x8B76D0))(worldPosition, screenPos);
}

void Present() {

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2((float)screenWidth, (float)screenHeight));
	ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
	Pl0000* MainPlayer = cGameUIManager::Instance.m_pPlayer;
	if (isInit && MainPlayer) { // Keep this IF statment to ensure UI textures are loaded
		// also _ = space, but i assume you got that
		//DrawFalseMGRUI(75.0f, 105.0f, 100, 100, 100, 100, "jetstream_sam");
		int i = 0;
		int hpDrawOffset = 0;
		for (MPPlayer* player : players) {
			if (player->playerObj == nullptr) {
				i++; // Accurately show controller IDs
				continue;
			}
			Pl0000* playerObj = player->playerObj;

			string name = player->playerName;
			if (name.empty()) {
				if (player->playerType == 0x10010) name = "Raiden";
				if (player->playerType == 0x11400) name = "Sam";
				if (player->playerType == 0x11500) name = "Wolf";
				if (player->playerType == 0x20020) name = "Jetstream Sam";
				if (player->playerType == 0x20700) name = "Senator";
				if (player->playerType == 0x2070A) name = "Senator";
				if (player->playerType == 0x20310) name = "Sundowner";
				if (player->playerType == 0x12040) name = "Dwarf Gekko";
			}

			float fcCur = 0;
			float fcMax = 0;
			float hpCur = (float)playerObj->m_nHealth;
			float hpMax = (float)playerObj->m_nMaxHealth;
			float trueHpMax = (float)playerObj->m_nMaxHealth;
			if ((player->playerType & 0xF0000) == 0x10000 // Enemies have no FC
				&& (player->playerType != 0x12040)) // Neither does Dwarf Gekko
			{
				fcCur = playerObj->getFuelContainer();
				fcMax = playerObj->getFuelCapacity(false);
				hpCur = (float)playerObj->getHealth();
				trueHpMax = (float)playerObj->getMaxHealth();
#ifdef MOUSEDEBUG
				//hpCur = CheckControlPressed(-1, CamUp, GamepadCamUp) - CheckControlPressed(-1, CamDown, GamepadCamDown);
				hpCur = GetMouseAnalog("MouseUp");
				hpMax = 100;
#endif
			}
			bool ripper = (player->playerType == 0x10010) && (playerObj->canActivateRipperMode() || playerObj->m_nRipperModeEnabled);
			DrawFalseMGRUI(140, 90 + 60 * hpDrawOffset, hpCur, hpMax, trueHpMax, fcCur, fcMax, name, ripper);
			auto pDrawList = ImGui::GetWindowDrawList();
			cVec4 player_pos = playerObj->getTransPos();
			player_pos.y += 2.3f;
			cVec4 temporary_projection = cVec4(0, 0, 0, 0);
			WorldToScreen(player_pos, temporary_projection);
			//pDrawList->AddText(ImVec2(temporary_projection.x, temporary_projection.y), ImColor(255, 255, 255), std::to_string(i).c_str());

			if (i == 0) {
				RenderTextWithShadow("Keyboard", (int)(temporary_projection.x - 50), (int)(temporary_projection.y - 10));
			}
			else {
				RenderTextWithShadow("Controller", (int)(temporary_projection.x - 50), (int)(temporary_projection.y - 10));
				RenderTextWithShadow(std::to_string(i), (int)(temporary_projection.x - 50), (int)temporary_projection.y);
			}
			i++;
			hpDrawOffset++;

		}

		// Nanopaste count
		int equippedRecovery = PlayerManagerImplement::ms_Instance->getRecoveryEquipped();
		if (equippedRecovery == 1) { // Nanopaste
			cItemPossessionCure* nanopastehandler = ((cItemPossessionCure* (__thiscall*)(unsigned long, int))(shared::base + 0x54E5E0))(shared::base + 0x1486EA0, 0x23A6F56D);
			RenderTextWithShadow(std::to_string(nanopastehandler->m_nBasePossession), 140, 100 + 60 * hpDrawOffset, C_BLACK, C_HPYELLOW);
		}
		else if (equippedRecovery == 2) { // Electrolyte packs (NOT USABLE)
			cItemPossessionCure* electrolytes = ((cItemPossessionCure * (__thiscall*)(unsigned long, int))(shared::base + 0x54E5E0))(shared::base + 0x1486EA0, 0xD92BB0F);
			RenderTextWithShadow(std::to_string(electrolytes->m_nBasePossession), 140, 100 + 60 * hpDrawOffset, C_BLACK, C_CYAN);
		}
		
		int draw_offset = 20;
		for (int ctrlr = 0; ctrlr < maxPlayerCount; ctrlr++) {
			MPPlayer* player = players[ctrlr];
			switch (player->controllerFlag) {
			case Out:
				if (IsGamepadButtonPressed(ctrlr, GamepadSpawn)) {
					player->controllerFlag = TaggingIn;
				}
				break;

			case TaggingIn:
				DrawCharacterSelector(60, draw_offset, ctrlr);
				draw_offset += 20 * (character_count + 1);

				if (IsGamepadButtonPressed(ctrlr, "XINPUT_GAMEPAD_A")) {
					player->controllerFlag = In;
					Se_PlayEvent("core_se_sys_decide_l");
					SpawnCharacter(player->characterSelection, ctrlr, player->costumeSelection[player->characterSelection]);
				}
				else if (IsGamepadButtonPressed(ctrlr, "XINPUT_GAMEPAD_B")) {
					player->controllerFlag = Out;
					player->characterSelection = 0;
				}
				break;

			case 2:
				if (IsGamepadButtonPressed(ctrlr, "XINPUT_GAMEPAD_START")
					&& !(ctrlr == 0 && !p1IsKeyboard)) { // Do not let main player drop
					player->controllerFlag = TaggingOut;
				}
				break;

			case 3:
				DrawDropMenu(60, draw_offset, ctrlr);
				draw_offset += 40;

				if (IsGamepadButtonPressed(ctrlr, "XINPUT_GAMEPAD_A")) {
					player->controllerFlag = Out;
					player->characterSelection = 0;
					Se_PlayEvent("core_se_sys_decide_l");
					if ((player->playerType & 0xf0000) == 0x20000) {
						// Go directly to hell
						player->playerObj->place({0, -10000, 0, 0}, {0, 0, 0, 0});
					}
					else { // TODO: "destruction queue" (it crashes if unarmed drops during attack, need to await idle)
						player->playerObj->m_pEntity->~Entity();
					}
					player->playerObj = nullptr;
					player->playerType = (eObjID)0;
				}
				else if (IsGamepadButtonPressed(ctrlr, "XINPUT_GAMEPAD_B")) {
					player->controllerFlag = In;
				}
				break;
			}

		}
		

	}
	ImGui::End();
}
