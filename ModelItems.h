#pragma once
struct ModelItems
{
	unsigned int m_nModel;
	unsigned int m_nHair;
	unsigned int m_nVisor;
	unsigned int m_nSheath;
	unsigned int m_nHead;
};

enum Costumes {
	CustomBody,
	CustomBodyBlue,
	CustomBodyRed,
	CustomBodyYellow,
	DesperadoBody,
	Suit,
	Mariachi,
	StandardBody,
	OriginalBody,
	GrayFox,
	WhiteArmor,
	InfernoArmor,
	CommandoArmor,
	CustomBodyDamaged,
	PrologueBody,
	Sam,
	LQ84i
};

enum CustomWeapons {
	None,
	Axe,
	Polearm,
	Sai,
	Pincers,
	Unarmed
};

__declspec(selectany) ModelItems* costumesList = (ModelItems*)(shared::base + 0x14A9828);
__declspec(selectany) eObjID* validCustomWeapons = (eObjID*)(shared::base + 0x14A99F0);
__declspec(selectany) eObjID** selectedCustomWeapon = (eObjID**)(shared::base + 0x17E9FCC);
