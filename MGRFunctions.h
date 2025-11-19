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