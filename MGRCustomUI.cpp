#include <d3dx9.h>
#include <cGameUIManager.h>
#include <Hw.h>
#include <Pl0000.h>
#include <BehaviorEmBase.h>
#include <filesystem>
#include <map>
#include <string>

using namespace std;

namespace fs = std::filesystem;

extern bool configLoaded;
extern Pl0000* players[5];

class MGRFontCharacter {
public:
	char character;
	LPDIRECT3DTEXTURE9 sprite;
	int width;

};

std::map<char, MGRFontCharacter> font_map[2];

LPDIRECT3DTEXTURE9 fc_segment;
LPDIRECT3DTEXTURE9 hp_segment;

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
		D3DXVECTOR3 position(x + tmp_x_shift, y, 0.0f);
		if (txtarray[i] != NULL) {

			pSprite->Draw(font_map[fontid][txtarray[i]].sprite, NULL, NULL, &position, color);
			tmp_x_shift -= font_map[fontid][txtarray[i]].width - 8;


		}
		else {
			tmp_x_shift += 20;
		}

	}
	pSprite->End();

	delete[] txtarray;

}


void RenderTextWithShadow(string text, float x, float y, D3DCOLOR bg = D3DCOLOR_ARGB(255, 0, 0, 0), D3DCOLOR fg = D3DCOLOR_XRGB(240, 255, 255), int fontid = 0, int justification_flag = 0) {
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
	RenderTextWithShadow(to_string((int)round((hpvalue / hpmax) * 100)) + ".", x + 330, y - 25, D3DCOLOR_ARGB(255, 30, 30, 30), D3DCOLOR_ARGB(255, 255, 227, 66), 0, 1);
	RenderTextWithShadow(to_string(decimalplace) + "_%", x + 375, y - 5, D3DCOLOR_ARGB(255, 30, 30, 30), D3DCOLOR_ARGB(255, 255, 227, 66), 1, 0);

	RenderTextWithShadow(name, x, y);
	DrawProgressBar(x, y + 23, hpvalue, hpmax, D3DCOLOR_ARGB(255, 30, 30, 30), D3DCOLOR_ARGB(255, 255, 227, 66));
	if (fcmax > 0) {
		DrawProgressBar(x, y + 28, fcvalue, fcmax, D3DCOLOR_ARGB(255, 30, 30, 30), D3DCOLOR_ARGB(255, 0, 255, 255));
	}


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
			if (player->m_pEntity->m_nEntityIndex == 0x20020) name = "sam";
			if (player->m_pEntity->m_nEntityIndex == 0x20700) name = "senator";
			if (player->m_pEntity->m_nEntityIndex == 0x2070A) name = "senator";

			if ((player->m_pEntity->m_nEntityIndex & 0xF0000) == 0x20000)
				DrawFalseMGRUI(75.0f, 105.0f + 60.0 * i, player->m_nHealth, player->m_nMaxHealth, 0, 0, name);
			else
				DrawFalseMGRUI(75.0f, 105.0f + 60.0 * i, player->getHealth(), player->getMaxHealth(),
				player->getFuelContainer(), player->getFuelCapacity(false), name);
			i++;
		}
	}
}
