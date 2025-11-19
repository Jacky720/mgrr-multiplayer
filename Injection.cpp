#include <injector/injector.hpp>
#include <shared.h>
#include <BehaviorEmBase.h>
#include <PlayerManagerImplement.h>

#include "Camera.h"
#include "dllmain.h"


Entity* __fastcall TargetHacked(BehaviorEmBase* ecx) {
	float minDist = INFINITY;
	Entity* closestPlayer = PlayerManagerImplement::ms_Instance->getEntity(0);
	cVec4 ePos = ecx->m_vecTransPos;
	for (Pl0000* player : players) {
		if (!player || (BehaviorEmBase*)player == ecx) continue;
		cVec4 pPos = player->m_vecTransPos;
		float dist = sqrt((pPos.x - ePos.x) * (pPos.x - ePos.x) + (pPos.y - ePos.y) * (pPos.y - ePos.y) + (pPos.z - ePos.z) * (pPos.z - ePos.z));
		if (dist < minDist) {
			closestPlayer = player->m_pEntity;
			minDist = dist;
		}
	}

	return closestPlayer;
}

int __cdecl ParseMenuController() {
	if (MainPlayer)
		return 0;
	return ((int(__cdecl*)())(shared::base + 0x9DB350))();
}

void __fastcall HealAll(Pl0000* p1, void* edx, int healAmt) {
	if (!p1->field_4E4 && p1->m_FuelContainers.m_size) {
		for (int i = 0; i < 5; i++) {
			if (players[i]) {
				players[i]->m_nHealth += healAmt;
				if (players[i]->m_nHealth > players[i]->getMaxHealth())
					players[i]->m_nHealth = players[i]->getMaxHealth();
			}
		}
		/*p1->m_nHealth += healAmt;
		if (p1->m_nHealth > p1->getMaxHealth()) p1->m_nHealth = p1->getMaxHealth();*/
	}
}

int __fastcall CheckZPress(Pl0000* player) {
	return (int)((player == players[0]) && cInput::isKeybindDown(cInput::KEYBIND_DEFFENSIVE_OFFENSIVE));
}

int __fastcall CheckXPress(Pl0000* player) {
	return (int)((player == players[0]) && cInput::isKeybindDown(cInput::KEYBIND_EXECUTION));
}

int __fastcall CheckRPress(Pl0000* player) {
	// Should be cInput::isKeybindPressed at some point in the future
	return (int)((player == players[0]) && ((BOOL(__cdecl*)(cInput::eSaveKeybind))(shared::base + 0x61D2D0))(cInput::KEYBIND_RIPPERMODE));
}

void __fastcall ApplyBladeMode(Pl0000* player, int* enemy) {
	if ((enemy[35] & 8) == 0) return; // idk it's in the original code
	if (player == MainPlayer) {
		if (PlayerManagerImplement::ms_Instance->getMainWeaponEquipped() == 6) {
			if (ReturnCallVMTFunc<BOOL, 239, Pl0000*>(player)) { // Has the special effect upgrade
				enemy[36] |= 0x20000;
			}
			else {
				enemy[36] |= 0x40000;
			}
		}
	}
	if (player->isBladeModeActive())
		enemy[36] |= 0x40000;
}


void MakeInjections() {

	injector::MakeNOP(shared::base + 0x69A516, 2, true); // F3 A5 // Disable normal input Sam and Wolf
	injector::MakeNOP(shared::base + 0x7937E6, 2, true); // Disable normal input Raiden
	// Dwarf Gekko has some more complex code
	injector::MakeNOP(shared::base + 0x1F5E56, 2, true); // Disable normal input Dwarf Gekko
	injector::MakeNOP(shared::base + 0x1F5E35, 6, true); // Disable joystick reset Dwarf Gekko (1)
	injector::MakeNOP(shared::base + 0x1F5E3C, 6, true); // Disable joystick reset Dwarf Gekko (2)
	//injector::MakeNOP(shared::base + 0x1F5E56, 2, true); // Disable input copy Dwarf Gekko
	// Disable fixRotation on Em0040_0015.mot
	injector::WriteMemory<unsigned short>(shared::base + 0x1FA661, 0xEED9, true); // FLDZ
	injector::MakeNOP(shared::base + 0x1FA663, 4, true);

	// i forget what exactly this is for, actually
	injector::MakeRET(shared::base + 0x1F62A0, 0, true); // this function crashes due to undefined field_A18, fix root cause instead
	// Disable normal controller input
	//injector::MakeNOP(shared::base + 0x9DB430, 5, true); // E8 1B FF FF FF
	injector::MakeCALL(shared::base + 0x9DB430, &ParseMenuController, true);
	// Remove need for custom pl1400 and pl1500
	injector::MakeNOP(shared::base + 0x69E313, 6, true);

	// Camera override
	injector::MakeCALL(shared::base + 0x823765, &CameraHacked, true); // Disable camera sometimes
	// Enemy targeting
	injector::MakeNOP(shared::base + 0x6C7E9C, 16, true); // Clear out redundant enemy target call
	injector::MakeCALL(shared::base + 0x6C7E9C, &TargetHacked, true);

	// Nanopaste
	injector::MakeCALL(shared::base + 0x54DD58, &HealAll, true);

	// Disable Z, X, and R on controller players
#define MakeZCheck(off) do { \
	injector::MakeNOP(shared::base + off, 10, true); \
	injector::WriteMemory<unsigned short>(shared::base + off, 0xD989, true); /* mov ecx,ebx */ \
	injector::MakeCALL(shared::base + off + 2, &CheckZPress, true); \
} while (false)

#define MakeXCheck(off) do { \
	injector::MakeNOP(shared::base + off, 10, true); \
	injector::WriteMemory<unsigned short>(shared::base + off, 0xD989, true); /* mov ecx,ebx */ \
	injector::MakeCALL(shared::base + off + 2, &CheckXPress, true); \
} while (false)

#define MakeRCheck(off) do { \
	injector::MakeNOP(shared::base + off, 10, true); \
	injector::WriteMemory<unsigned short>(shared::base + off, 0xCE8B, true); /* mov ecx,esi */ \
	injector::MakeCALL(shared::base + off + 2, &CheckRPress, true); \
} while (false)
	// Raiden
	MakeZCheck(0x79403A);
	MakeXCheck(0x794090);
	MakeRCheck(0x810597);
	MakeRCheck(0x8106AD);
	// DLC characters
	MakeZCheck(0x69AD1B);
	MakeXCheck(0x69AD71);

	// Make enemies cuttable
	injector::MakeNOP(shared::base + 0x7E7406, 0x32, true);
	injector::WriteMemory<unsigned short>(shared::base + 0x7E7406, 0xF289, true); // mov edx,esi
	injector::WriteMemory<unsigned int>(shared::base + 0x7E7408, 0x18244C8B, true); // ecx setup
	//injector::MakeCALL(shared::base + 0x7E740C, &ApplyBladeMode, true); // TODO: Doesn't work rn, blade mode attacks aren't associated with the actual player?

}