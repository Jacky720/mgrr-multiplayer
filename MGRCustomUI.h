#pragma once
#include <d3dx9.h>

__declspec(selectany) LPD3DXSPRITE pSprite = NULL;
__declspec(selectany) int screenWidth = GetSystemMetrics(SM_CXSCREEN);
__declspec(selectany) int screenHeight = GetSystemMetrics(SM_CYSCREEN);

void LoadUIData();

void Present();

void ResetControllerAllFlags();

enum ControllerFlags {
	Out,
	TaggingIn,
	In,
	TaggingOut
};