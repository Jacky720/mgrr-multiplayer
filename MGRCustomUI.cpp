#include <d3dx9.h>
#include <cGameUIManager.h>
#include <Hw.h>
#include <Pl0000.h>
#include <BehaviorEmBase.h>
#include <filesystem>
#include <map>
#include <string>
#include <XInput.h>
#include "MGRControls.h"

using namespace std;


namespace fs = std::filesystem;

void __cdecl Se_PlayEvent(const char* event)
{
	((void(__cdecl*)(const char*))(shared::base + 0xA5E1B0))(event);
}

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
int checkScreenSizeTimer = 60;

int selection_ids[4] = { 0, 0, 0, 0 };
int controller_flag[4] = { 0, 0, 0, 0 };
// 0 - Unknown/Not Connected
// 1 - Requires selection
// 2 - Character selected, ready


extern void SpawnCharacter(int, int);

extern bool configLoaded;
extern std::string character_titles[5];
extern Pl0000* players[5];

bool dpad_up_pressed[6] = { false, false, false, false, false, false };
bool dpad_down_pressed[6] = { false, false, false, false, false, false };

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

LPD3DXSPRITE pSprite = NULL;

void LoadFont(fs::path directory_path, int id) {
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	int loaded_font_textures = 0;
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


void DrawLine(LPDIRECT3DDEVICE9 Device_Interface, int bx, int by, int bw, D3DCOLOR COLOR)
{
	D3DRECT rec;
	rec.x1 = bx;
	rec.y1 = by;
	rec.x2 = bx + bw;//makes line longer/shorter going right
	rec.y2 = by + 6;//makes line one pixel tall
	Device_Interface->Clear(1, &rec, D3DCLEAR_TARGET, COLOR, 0, 0);

}

void DrawLine(LPDIRECT3DDEVICE9 Device_Interface, int bx, int by, int bw, D3DCOLOR COLOR, int thickness)
{
	D3DRECT rec;
	rec.x1 = bx;
	rec.y1 = by;
	rec.x2 = bx + bw;//makes line longer/shorter going right
	rec.y2 = by + thickness;//makes line one pixel tall
	Device_Interface->Clear(1, &rec, D3DCLEAR_TARGET, COLOR, 0, 0);

}

void RenderTextMGR(string text, float x, float y, D3DCOLOR color, int fontid = 0) {
	// Copy string to a cstring array and then draw using the map, which allows for infinite expansion, just add the character to the mgfonts folder

	int n = text.length();
	char* txtarray = new char[n + 1];
	strcpy_s(txtarray, n + 1, text.c_str());

	pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	int tmp_x_shift = 0;
	for (int i = 0; i < n + 1; i++) {
		D3DXVECTOR3 position(x + tmp_x_shift, y, 0.0f);
		if (txtarray[i] != NULL) {

			pSprite->Draw(font_map[fontid][txtarray[i]].sprite, NULL, NULL, &position, color);
			tmp_x_shift += font_map[fontid][txtarray[i]].width - 8;


		}
		else {
			tmp_x_shift += 20;
		}

	}
	pSprite->End();

	delete[] txtarray;

}

void RenderTextMGR_RightLeft(string text, float x, float y, D3DCOLOR color, int fontid = 0) {
	// Copy string to a cstring array and then draw using the map, which allows for infinite expansion, just add the character to the mgfonts folder
	int n = text.length();
	char* txtarray = new char[n + 1];
	strcpy_s(txtarray, n + 1, text.c_str());


	pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	int tmp_x_shift = 0;
	for (int i = n - 1; i >= 0; i--) {
		tmp_x_shift -= font_map[fontid][txtarray[i]].width - 8; // Shift BEFORE since we're going left, THROUGH the character's space
		D3DXVECTOR3 position(x + tmp_x_shift, y, 0.0f);
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

void RenderTextWithShadow(string text, float x, float y, D3DCOLOR bg = C_BLACK, D3DCOLOR fg = C_LTGRAY, int fontid = 0, int justification_flag = 0) {
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

	for (int i = 0; i < 8; i++) {
		if (justification_flag == 0) {
			RenderTextMGR(text, x + offsets[i][0], y + offsets[i][1], bg, fontid);
		}
		else if (justification_flag == 1) {
			RenderTextMGR_RightLeft(text, x + offsets[i][0], y + offsets[i][1], bg, fontid);
		}


	}
	if (justification_flag == 0) {
		RenderTextMGR(text, x, y, fg, fontid);
	}
	else if (justification_flag == 1) {
		RenderTextMGR_RightLeft(text, x, y, fg, fontid);
	}

}



void DrawProgressBar(float x, float y, float value, float maxvalue, D3DCOLOR bg, D3DCOLOR fg) {

	DrawLine(Hw::GraphicDevice, x, y, (400), bg, 4);
	DrawLine(Hw::GraphicDevice, x, y, ((value / maxvalue) * 400), fg, 4);
}





void DrawFalseMGRUI(float x, float y, float hpvalue, float hpmax, float fcvalue, float fcmax, string name) {
	int decimalplace = static_cast<int>(((hpvalue / hpmax) * 100) * 10) % 10;
	RenderTextWithShadow(to_string((int)floor((hpvalue / hpmax) * 100)) + ".", x + 350, y - 25, C_DKGRAY, C_HPYELLOW, 0, 1);
	RenderTextWithShadow(to_string(decimalplace) + "_%", x + 350, y - 5, C_DKGRAY, C_HPYELLOW, 1, 0);

	RenderTextWithShadow(name, x, y);
	DrawProgressBar(x, y + 23, hpvalue, hpmax, C_DKGRAY, C_HPYELLOW);
	if (fcmax > 0) {
		DrawProgressBar(x, y + 28, fcvalue, fcmax, C_DKGRAY, C_CYAN);
	}


}

// Hey, this "controller_id" is actually a player ID (1-4 for controllers, not 0-3)
void DrawCharacterSelector(float offset_x, float y, int controller_id) {
	// Render character selection
	if (checkScreenSizeTimer > 0) {
		RECT rect;
		checkScreenSizeTimer--;
		if (checkScreenSizeTimer == 0
		    && GetWindowRect(FindWindowA(NULL, "METAL GEAR RISING: REVENGEANCE"), &rect))
		{
			screenWidth = rect.right - rect.left;
			screenHeight = rect.bottom - rect.top;
			checkScreenSizeTimer = -1;
		}

		if (checkScreenSizeTimer == 0) checkScreenSizeTimer = 60;
	}


	string numbername;
	switch (controller_id + 1) {
	case 0: numbername = "zero"; break;
	case 1: numbername = "one"; break;
	case 2: numbername = "two"; break;
	case 3: numbername = "three"; break;
	case 4: numbername = "four"; break;
	case 5: numbername = "five"; break;
	case 6: numbername = "six"; break;
	case 7: numbername = "seven"; break;
	case 8: numbername = "eight"; break;
	case 9: numbername = "nine"; break;
	default: numbername = "unknown";
	}
	
	if (IsGamepadButtonPressed(controller_id, "XINPUT_GAMEPAD_DPAD_UP") && !dpad_up_pressed[controller_id]) {
		dpad_up_pressed[controller_id] = true;
		selection_ids[controller_id]--;
		if (selection_ids[controller_id] < 0) {
			selection_ids[controller_id] = sizeof(character_titles) / sizeof(std::string) - 1;
		}
		Se_PlayEvent("core_se_sys_custom_item_window_corsor");
	}
	else if (!IsGamepadButtonPressed(controller_id, "XINPUT_GAMEPAD_DPAD_UP")) {
		dpad_up_pressed[controller_id] = false;
	}
	if (IsGamepadButtonPressed(controller_id, "XINPUT_GAMEPAD_DPAD_DOWN") && !dpad_down_pressed[controller_id]) {
		dpad_down_pressed[controller_id] = true;

		selection_ids[controller_id]++;
		if (selection_ids[controller_id] >= sizeof(character_titles)/sizeof(std::string)) {
			selection_ids[controller_id] = 0;
		}
		Se_PlayEvent("core_se_sys_custom_item_window_corsor");
	}
	else if (!IsGamepadButtonPressed(controller_id, "XINPUT_GAMEPAD_DPAD_DOWN")) {
		dpad_down_pressed[controller_id] = false;
	}
	

	RenderTextWithShadow("player_" + numbername + "_joining_as", screenWidth - offset_x, y + 20, C_BLACK, C_HPYELLOW, 0, 1);

	for (int i = 0; i < 5; i++) {
		if (i == selection_ids[controller_id]) {
			RenderTextWithShadow(character_titles[i], screenWidth - offset_x, y + 40 + i * 20, C_BLACK, C_LTGRAY, 0, 1);
		}
		else {
			RenderTextWithShadow(character_titles[i], screenWidth - offset_x, y + 40 + i * 20, C_BLACK, C_LTGRAYFADE,  0, 1);
		}
		
	}
	
	//RenderTextMGR_RightLeft("sundowner", screenWidth - offset_x, y + 60, C_LTGRAY, 0);
	//RenderTextMGR_RightLeft("raiden", screenWidth - offset_x, y + 80, C_LTGRAYFADE, 0);

}

void ResetControllerAllFlags() {
	controller_flag[0] = 0;
	controller_flag[1] = 0;
	controller_flag[2] = 0;
	controller_flag[3] = 0;
}


void Present() {


	Pl0000* MainPlayer = cGameUIManager::Instance.m_pPlayer;
	if (configLoaded && MainPlayer) { // Keep this IF statment to ensure UI textures are loaded
		// also _ = space, but i assume you got that
		//DrawFalseMGRUI(75.0f, 105.0f, 100, 100, 100, 100, "jetstream_sam");
		int i = 0;
		for (Pl0000* player : players) {
			if (player == nullptr) continue;

			string name = "";
			if (player->m_pEntity->m_nEntityIndex == 0x10010) name = "raiden";
			if (player->m_pEntity->m_nEntityIndex == 0x11400) name = "sam";
			if (player->m_pEntity->m_nEntityIndex == 0x11500) name = "wolf";
			if (player->m_pEntity->m_nEntityIndex == 0x20020) name = "jetstream_sam";
			if (player->m_pEntity->m_nEntityIndex == 0x20700) name = "senator";
			if (player->m_pEntity->m_nEntityIndex == 0x2070A) name = "senator";
			if (player->m_pEntity->m_nEntityIndex == 0x20310) name = "sundowner";

			if ((player->m_pEntity->m_nEntityIndex & 0xF0000) == 0x20000)
				DrawFalseMGRUI(75.0f, 105.0f + 60.0 * i, player->m_nHealth, player->m_nMaxHealth, 0, 0, name);
			else
				DrawFalseMGRUI(75.0f, 105.0f + 60.0 * i, player->getHealth(), player->getMaxHealth(),
				player->getFuelContainer(), player->getFuelCapacity(false), name);
			i++;

		}

		int draw_offset = 0;
		for (int ctrlr = 0; ctrlr < 4; ctrlr++) {


			if (IsGamepadButtonPressed(ctrlr, "XINPUT_GAMEPAD_START") && controller_flag[ctrlr] == 0) {
				controller_flag[ctrlr] = 1;
			}

			if (controller_flag[ctrlr] == 1) {
				DrawCharacterSelector(60, (draw_offset * 140), ctrlr);
				draw_offset++;

				if (IsGamepadButtonPressed(ctrlr, "XINPUT_GAMEPAD_A")) {
					controller_flag[ctrlr] = 2;
					Se_PlayEvent("core_se_sys_decide_l");
					SpawnCharacter(selection_ids[ctrlr], ctrlr);
				}
				else if (IsGamepadButtonPressed(ctrlr, "XINPUT_GAMEPAD_B")) {
					controller_flag[ctrlr] = 0;
					selection_ids[ctrlr] = 0;
				}
			}

		}

	}
}
