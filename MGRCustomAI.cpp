#include "MGRControls.h"
#include <BehaviorEmBase.h>
#include <cCameraGame.h>
#include <Pl0000.h>

extern void TeleportToMainPlayer(Pl0000* mainPlayer, int controllerIndex = -1);
extern Pl0000* MainPlayer;

int healTimers[5] = { -1, -1, -1, -1, -1 };
bool EveryHeal = true;

bool SetFlagsForAction(Pl0000* player, int controllerNumber, std::string Keybind, std::string GamepadBind,
	InputBitflags bit, int* altField1 = nullptr, int* altField2 = nullptr) {
	if (CheckControlPressed(controllerNumber, Keybind, GamepadBind)) {
		player->m_nKeyHoldingFlag |= bit;
		player->m_nKeyPressedFlag |= bit;
		if (altField1) *altField1 |= bit;
		if (altField2) *altField2 |= bit;
		return true;
	}
	else {
		player->m_nKeyHoldingFlag &= ~bit;
		player->m_nKeyPressedFlag &= ~bit;
		if (altField1) *altField1 &= ~bit;
		if (altField2) *altField2 &= ~bit;
		return false;
	}
}

bool SetFlagsForAnalog(Pl0000* player, int controllerNumber, std::string Keybind, std::string GamepadBind,
	InputBitflags bit, float* altField, bool invert) {
	if (CheckControlPressed(controllerNumber, Keybind, GamepadBind)) {
		player->m_nKeyHoldingFlag |= bit;
		*altField = invert ? -1000.0f : 1000.0f;
		if (IsGamepadButtonPressed(controllerNumber, GamepadBind))
			*altField *= GetGamepadAnalog(controllerNumber, GamepadBind);
		return true;
	}
	return false;
}

void UpdateBossActions(BehaviorEmBase* Enemy, unsigned int BossActions[], int controllerNumber = -1) {

	if (CheckControlPressed(controllerNumber, NormalAttack, GamepadNormalAttack)
		&& Enemy->m_nCurrentAction != BossActions[2]) { // Two punches (four strikes)
		Enemy->setState(BossActions[2], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, StrongAttack, GamepadStrongAttack)
		&& Enemy->m_nCurrentAction != BossActions[3]) { // Two punches, kick, punch (four strike w/ sheath)
		Enemy->setState(BossActions[3], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Run, GamepadRun)
		&& Enemy->m_nCurrentAction != BossActions[4]) { // Run QTE (Assault Rush)
		Enemy->setState(BossActions[4], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Interact, GamepadInteract)
		&& Enemy->m_nCurrentAction != BossActions[5]) { // Overhead with AOE (unblockable QTE)
		Enemy->setState(BossActions[5], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Jump, GamepadJump)
		&& Enemy->m_nCurrentAction != BossActions[6]) { // Uppercut (perfect parry QTE fail)
		Enemy->setState(BossActions[6], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Taunt, GamepadTaunt)
		&& Enemy->m_nCurrentAction != BossActions[7]) { // Explode (taunt)
		Enemy->setState(BossActions[7], 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, BladeMode, GamepadBladeMode)
		&& Enemy->m_nCurrentAction != BossActions[8]) { // Heal (un-perfect-parryable four strike)
		Enemy->setState(BossActions[8], 0, 0, 0);
	}
}

void FullHandleAIBoss(BehaviorEmBase* Enemy, int controllerNumber, bool CanDamagePlayer) {

	/*
	if (Enemy->m_pEntity->m_nEntityIndex == 0x20700 || Enemy->m_pEntity->m_nEntityIndex == 0x2070A && PlayAsArmstrong) {
		unsigned int BossActions[] = { 0x20000, 0x20003, 0x20007, 0x20006, 0x20001, 0x20009, 0x20010 };
		UpdateBossActions(Enemy, BossActions, controllerNumber);

		if (ArmstrongCanDamagePlayer)
			Enemy->field_640 = 2;
		else
			Enemy->field_640 = 1;
	}

	if (Enemy->m_pEntity->m_nEntityIndex == 0x20020 && PlayAsSam) {
		unsigned int BossActions[] = { 0x30004, 0x30006, 0x30007, 0x30014, 0x3001C, 0x30005, 0x10006 };
		UpdateBossActions(Enemy, BossActions, controllerNumber);

		if (BossSamCanDamagePlayer)
			Enemy->field_640 = 2;
		else
			Enemy->field_640 = 1;
	}
	*/

	if (CheckControlPressed(controllerNumber, Pause2, GamepadPause2))
		TeleportToMainPlayer(MainPlayer, controllerNumber);



	float rotation = 0;
	float v9;
	float field_X = 0;
	float field_Y = 0;
	float v10;
	float v11;
	cVec4 v54;
	long double tann = 0;
	//forward

	cCameraGame camera = cCameraGame::Instance;
	//UpdateMovement(Enemy, &camera);


	// Buttons: Idle, Walking, X, Y, RT, B, A, up, LT
	static unsigned int ArmstrongBossActions[] = { 0x10000, 0x10001, 0x20000, 0x20003, 0x20007, 0x20006, 0x20001, 0x20009, 0x20010 };
	static unsigned int SamBossActions[] = { 0x20000, 0x10002, 0x30004, 0x30006, 0x30007, 0x30014, 0x3001C, 0x10006, 0x30005 };
	static unsigned int SundownerBossActions[] = { 0x10000, 0x10001, 0x20001, 0x20000, 0x20008, 0x20002, 0x20007, 0x10006, 0x20009 };
	// Default here for Armstrong (em0700)
	unsigned int* BossActions = ArmstrongBossActions;

	if (Enemy->m_pEntity->m_nEntityIndex == 0x20020) {
		BossActions = SamBossActions;
	}
	if (Enemy->m_pEntity->m_nEntityIndex == 0x20310) {
		BossActions = SundownerBossActions;
	}

	UpdateBossActions(Enemy, BossActions, controllerNumber);

	if (CanDamagePlayer)
		Enemy->field_640 = 2;
	else
		Enemy->field_640 = 1;


	if (CheckControlPressed(controllerNumber, Forward, GamepadForward)) {
		field_Y = -1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadForward))
			field_Y *= GetGamepadAnalog(controllerNumber, GamepadForward);
	}




	//back
	if (CheckControlPressed(controllerNumber, Back, GamepadBack)) {
		//if (GetAsyncKeyState(0x4B)) {
		field_Y = 1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadBack))
			field_Y *= GetGamepadAnalog(controllerNumber, GamepadBack);
	}

	//left
	if (CheckControlPressed(controllerNumber, Left, GamepadLeft)) {
		field_X = -1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadLeft))
			field_X *= GetGamepadAnalog(controllerNumber, GamepadLeft);
	}

	//right
	if (CheckControlPressed(controllerNumber, Right, GamepadRight)) {
		field_X = 1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadRight))
			field_X *= GetGamepadAnalog(controllerNumber, GamepadRight);
	}

	if (field_X != 0 || field_Y != 0) {
		if (Enemy->m_nCurrentAction == BossActions[0]) Enemy->setState(BossActions[1], 0, 0, 0);
		float angle = atan2(field_X, field_Y);
		angle += camera.cCameraTypes::field_4 + PI;
		Enemy->m_vecRotation.y = angle;
	}
	else {
		if (Enemy->m_nCurrentAction == BossActions[1]) Enemy->setState(BossActions[0], 0, 0, 0);
	}


	//v9 = field_X * field_X + field_Y * field_Y;




	//D3DXMATRIX matrix;
	//D3DXMatrixIdentity(matrix);

	//((void(__thiscall*)(BehaviorEmBase* Enemy, D3DXMATRIX* matrix))(shared::base + 0x77A260))(Enemy, &matrix);

/*	const D3DXMATRIX* ConstMatrix;

	//ConstMatrix = matrix;

	if (v9 > 0.0)
	{
		v10 = -field_X;
		v11 = -field_Y;
		//Enemy->field_D2C = atan2(v10, v11);
		v54.x = v10;
		v54.y = v11;
		v54.z = 0.0;
		D3DXVECTOR3 vector;
		vector.x = v54.x;
		vector.y = v54.y;
		vector.z = v54.z;
		//D3DXVec3TransformNormal(vector,vector, matrix);
		//((D3DXVECTOR3*(__cdecl*)(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, const D3DXMATRIX* pM))(shared::base + 0x1036F2C))(&vector,&vector,&matrix);
		//D3DXVec3TransformNormal(&vector,&vector, &matrix);
		v54.x = Enemy->m_vecTransPos.x + v54.x;
		v54.y = Enemy->m_vecTransPos.y + v54.y;
		v54.z = Enemy->m_vecTransPos.z + v54.z;
		v54.w = Enemy->m_vecTransPos.w + v54.w;
		Enemy->m_vecRotation.y = ((long double(__thiscall*)(Behavior* Enemy, cVec4 * vec4))(shared::base + 0x68EC30))(Enemy, &v54);
	}


	/*player->field_D30 = player->m_vecRotation.y;
	*/

}

void FullHandleAIPlayer(Pl0000* player, int controllerNumber, bool EnableDamageToPlayers) {
	int i = controllerNumber + 1;
	if (EnableDamageToPlayers)
		player->field_640 = 0;
	else
		player->field_640 = 1;

	player->m_nKeyHoldingFlag = 0;
	player->field_D0C = 0;
	player->field_D08 = 0;
	player->field_D10 = 0;
	player->field_D14 = 0;

	static bool wasJumpSam[5] = { false }; // For Sam, whether jump was already pressed on the previous frame
	static bool wasCrouchWolf[5] = { false }; // For Wolf, whether Ability was already pressed


	//injector::WriteMemory<unsigned int>(*(unsigned int*)shared::base + 0x17E9FF4, originalSword, true);

	/*
	if (getKeyState('9').isPressed && !DisableNumberBinds) {
		AutoNormalAttackEnable = !AutoNormalAttackEnable;
		AutoStrongAttackEnable = false;
	}

	if (getKeyState('0').isPressed && !DisableNumberBinds) {
		AutoStrongAttackEnable = !AutoStrongAttackEnable;
		AutoNormalAttackEnable = false;
	}*/

	//Change camera between players

	if ((GetKeyState('8') & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadLockon))
		((int(__thiscall*)(Pl0000 * player))(shared::base + 0x784B90))(player);

	if ((controllerNumber == 0 && GetKeyState(std::stoi(Pause2, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadPause2))
		TeleportToMainPlayer(MainPlayer, controllerNumber);


	bool isAny = false;

	// Special cases where keyPressedFlag is important
	if (player->m_nModelIndex == 0x11400) {
		if (wasJumpSam[i]) {
			wasJumpSam[i] = SetFlagsForAction(player, controllerNumber, Jump, GamepadJump, JumpBit, &player->field_D00, &player->field_D04);
			player->m_nKeyPressedFlag &= ~JumpBit;
		}
		else {
			wasJumpSam[i] = SetFlagsForAction(player, controllerNumber, Jump, GamepadJump, JumpBit, &player->field_D00, &player->field_D04);
		}
		isAny |= wasJumpSam[i];
	}
	else {
		isAny |= SetFlagsForAction(player, controllerNumber, Jump, GamepadJump, JumpBit, &player->field_D00, &player->field_D04);
	}

	if (player->m_nModelIndex == 0x11500) {
		if (wasCrouchWolf[i]) {
			wasCrouchWolf[i] = SetFlagsForAction(player, controllerNumber, Ability, GamepadAbility, AbilityBit);
			player->m_nKeyPressedFlag &= ~AbilityBit;
		}
		else {
			wasCrouchWolf[i] = SetFlagsForAction(player, controllerNumber, Ability, GamepadAbility, AbilityBit);
		}
		isAny |= wasCrouchWolf[i];
	}
	else {
		isAny |= SetFlagsForAction(player, controllerNumber, Ability, GamepadAbility, AbilityBit);
	}

	// Left stick
	isAny |= SetFlagsForAnalog(player, controllerNumber, Forward, GamepadForward, ForwardBit, &player->field_D0C, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, Back, GamepadBack, BackwardBit, &player->field_D0C, false);
	isAny |= SetFlagsForAnalog(player, controllerNumber, Left, GamepadLeft, LeftBit, &player->field_D08, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, Right, GamepadRight, RightBit, &player->field_D08, false);
	// Right stick
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamUp, GamepadCamUp, CamUpBit, &player->field_D14, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamDown, GamepadCamDown, CamDownBit, &player->field_D14, false);
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamLeft, GamepadCamLeft, CamLeftBit, &player->field_D10, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamRight, GamepadCamRight, CamRightBit, &player->field_D10, false);
	// Face buttons
	isAny |= SetFlagsForAction(player, controllerNumber, NormalAttack, GamepadNormalAttack, LightAttackBit, &player->field_D04);
	isAny |= SetFlagsForAction(player, controllerNumber, StrongAttack, GamepadStrongAttack, HeavyAttackBit, &player->field_D04);
	isAny |= SetFlagsForAction(player, controllerNumber, Interact, GamepadInteract, InteractBit);
	// Triggers and bumpers (lock-on takes camera control instead)
	isAny |= SetFlagsForAction(player, controllerNumber, Run, GamepadRun, RunBit, &player->field_D00, &player->field_D04);
	isAny |= SetFlagsForAction(player, controllerNumber, BladeMode, GamepadBladeMode, BladeModeBit, &player->field_D00, &player->field_D04);
	isAny |= SetFlagsForAction(player, controllerNumber, Subweapon, GamepadSubweapon, SubWeaponBit);
	// D-pad
	isAny |= SetFlagsForAction(player, controllerNumber, Taunt, GamepadTaunt, TauntBit);
	if (SetFlagsForAction(player, controllerNumber, Heal, GamepadHeal, HealBit)) { // Plays effect, does not heal
		isAny |= true;
		if (EveryHeal && healTimers[i] <= 0) {
			healTimers[i] = 30 * 60;
			player->setHealth(player->getMaxHealth());
		}
	}
	if (healTimers[i] > 0)
		healTimers[i]--;
	//isAny |= SetFlagsForAction(player, controllerNumber, WeaponMenu, GamepadWeaponMenu, WeaponMenuBit);
	//isAny |= SetFlagsForAction(player, controllerNumber, WeaponMenu2, GamepadWeaponMenu2, WeaponMenu2Bit);
	// Other
	isAny |= SetFlagsForAction(player, controllerNumber, CamReset, GamepadCamReset, CamResetBit);
	//isAny |= SetFlagsForAction(player, controllerNumber, Pause, GamepadPause, PauseBit); // Does not work
	//isAny |= SetFlagsForAction(player, controllerNumber, Pause2, GamepadPause2, CodecBit); // Non-Raidens don't have codec

	if (!isAny) {
		player->m_nKeyPressedFlag = 0;
		player->field_D04 = 0;
	}

}
