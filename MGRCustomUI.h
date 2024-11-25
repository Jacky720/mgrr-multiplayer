#pragma once
#include <d3dx9.h>

extern LPD3DXSPRITE pSprite;
extern int controller_flag[4];

void LoadUIData();

void Present();

void ResetControllerAllFlags();