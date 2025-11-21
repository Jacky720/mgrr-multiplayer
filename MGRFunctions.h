#pragma once


// -- EM0310/SUNDOWNER --

typedef int* (__thiscall* Sub_18AE10_t)(Behavior* _this, unsigned int a2);
static Sub_18AE10_t sundownerPhase2Create = (Sub_18AE10_t)(shared::base + 0x188E40);

// -- Anything else --
inline int giveVanillaCameraControl(Pl0000* player) {
	return ((int(__thiscall*)(Pl0000*))(shared::base + 0x784B90))(player);
}

inline void __cdecl Se_PlayEvent(const char* event) {
	((void(__cdecl*)(const char*))(shared::base + 0xA5E1B0))(event);
}

inline bool isAnimationFinished(Behavior* behavior, int a2) {
	return ((BOOL(__thiscall*)(Behavior*, int))(shared::base + 0x694CE0))(behavior, a2);
}

inline int requestAnimationByMap(Behavior* behavior, int mapId, int a3, float f4, float f5, int flags, float f7, float f8) {
	return ((int(__thiscall*)(Behavior*, int, int, float, float, int, float, float))(shared::base + 0x6A4080))(behavior, mapId, a3, f4, f5, flags, f7, f8);
}