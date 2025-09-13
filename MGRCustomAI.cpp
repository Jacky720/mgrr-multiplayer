#include "Camera.h"
#include "dllmain.h"
#include "MGRControls.h"

#include <BehaviorEmBase.h>
#include <cCameraGame.h>
#include <Pl0000.h>

int healTimers[5] = { -1, -1, -1, -1, -1 };
int prevPressed[5] = { 0 };
bool EveryHeal = true;
bool isARPressed = false;
bool wasARPressed = false;

bool SetFlagsForAction(Pl0000* player, int controllerNumber, std::string Keybind, std::string GamepadBind,
	InputBitflags bit, int* altField1 = nullptr, int* altField2 = nullptr) {
	int playerIndex = -1;
	for (int i = 0; i < 5; i++) {
		if (players[i] == player)
			playerIndex = i;
	}

	if (CheckControlPressed(controllerNumber, Keybind, GamepadBind)) {
		player->m_nKeyHoldingFlag |= bit;
		if (!(prevPressed[playerIndex] & bit)) {
			player->m_nKeyPressedFlag |= bit;
		}
		else {
			player->m_nKeyPressedFlag &= ~bit;
		}
		prevPressed[playerIndex] |= bit;
		if (altField1) *altField1 |= bit;
		if (altField2) *altField2 |= bit;
		player->field_D00 |= bit;
		player->field_D04 |= bit;
		return true;
	}
	else {
		player->m_nKeyHoldingFlag &= ~bit;
		player->m_nKeyPressedFlag &= ~bit;
		prevPressed[playerIndex] &= ~bit;
		if (altField1) *altField1 &= ~bit;
		if (altField2) *altField2 &= ~bit;
		player->field_D00 &= ~bit;
		player->field_D04 &= ~bit;
		return false;
	}
}

bool SetFlagsForAnalog(Pl0000* player, int controllerNumber, std::string Keybind, std::string GamepadBind,
	InputBitflags bit, float* altField, bool invert) {
	if (CheckControlPressed(controllerNumber, Keybind, GamepadBind)) {
		player->m_nKeyHoldingFlag |= bit;
		player->field_D00 |= bit;
		player->field_D04 |= bit;
		*altField = invert ? -1000.0f : 1000.0f;
		if (IsGamepadButtonPressed(controllerNumber, GamepadBind))
			*altField *= GetGamepadAnalog(controllerNumber, GamepadBind);
		return true;
	}
	player->field_D00 &= ~bit;
	player->field_D04 &= ~bit;
	return false;
}


typedef struct actionList {
	unsigned int Idle;
	unsigned int Walking;
	unsigned int LightAttack;
	unsigned int HeavyAttack;
	unsigned int StartRun;
	unsigned int Interaction;
	unsigned int Jumping;
	unsigned int Taunting;
	unsigned int Special;
	unsigned int EndSpecial;
	unsigned int EndRun;
	unsigned int MidRun;
} ActionList;

void UpdateBossActions(BehaviorEmBase* Enemy, ActionList* BossActions, int controllerNumber = -1) {

	// Armstrong move (Sam move)
	if (CheckControlPressed(controllerNumber, NormalAttack, GamepadNormalAttack)
		&& Enemy->m_nCurrentAction != BossActions->LightAttack) { // Two punches (four strikes)
		Enemy->setState(BossActions->LightAttack, 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, StrongAttack, GamepadStrongAttack)
		&& Enemy->m_nCurrentAction != BossActions->HeavyAttack) { // Two punches, kick, punch (four strike w/ sheath)
		Enemy->setState(BossActions->HeavyAttack, 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Run, GamepadRun)
		&& (
			CheckControlPressed(controllerNumber, Forward, GamepadForward)
		    || CheckControlPressed(controllerNumber, Back, GamepadBack)
		    || CheckControlPressed(controllerNumber, Left, GamepadLeft)
		    || CheckControlPressed(controllerNumber, Right, GamepadRight))) { // Run QTE (Assault Rush)
		if (Enemy->m_nCurrentAction != BossActions->StartRun && Enemy->m_nCurrentAction != BossActions->MidRun)
			Enemy->setState(BossActions->StartRun, 0, 0, 0);
	}
	else if (Enemy->m_nCurrentAction == BossActions->MidRun) { // Running should end
		Enemy->setState(BossActions->EndRun, 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Interact, GamepadInteract)
		&& Enemy->m_nCurrentAction != BossActions->Interaction) { // Overhead with AOE (unblockable QTE)
		Enemy->setState(BossActions->Interaction, 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Jump, GamepadJump)
		&& Enemy->m_nCurrentAction != BossActions->Jumping) { // Uppercut (perfect parry QTE fail)
		Enemy->setState(BossActions->Jumping, 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, Taunt, GamepadTaunt)
		&& Enemy->m_nCurrentAction != BossActions->Taunting) { // Explode (taunt)
		Enemy->setState(BossActions->Taunting, 0, 0, 0);
	}

	if (CheckControlPressed(controllerNumber, BladeMode, GamepadBladeMode)) { // Heal (un-perfect-parryable four strike)
		if (Enemy->m_nCurrentAction != BossActions->Special)
			Enemy->setState(BossActions->Special, 0, 0, 0);
	}
	else if (Enemy->m_nCurrentAction == BossActions->Special) { // Blade Mode should end 
		Enemy->setState(BossActions->EndSpecial, 0, 0, 0);
	}
}

void FullHandleAIBoss(BehaviorEmBase* Enemy, int controllerNumber, bool CanDamagePlayer) {

	/*
	if (Enemy->m_pEntity->m_nEntityIndex == 0x20700 || Enemy->m_pEntity->m_nEntityIndex == 0x2070A && PlayAsArmstrong) {
		unsigned int BossActions[] = { 0x20000, 0x20003, 0x20007, 0x20006, 0x20001, 0x20009, 0x20010 };
		UpdateBossActions(Enemy, BossActions, controllerNumber);

		if (EnableFriendlyFire)
			Enemy->field_640 = 2;
		else
			Enemy->field_640 = 1;
	}

	if (Enemy->m_pEntity->m_nEntityIndex == 0x20020 && PlayAsSam) {
		unsigned int BossActions[] = { 0x30004, 0x30006, 0x30007, 0x30014, 0x3001C, 0x30005, 0x10006 };
		UpdateBossActions(Enemy, BossActions, controllerNumber);

		if (EnableFriendlyFire)
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


	// Buttons:                                Idle,    Walking, X,       Y,       RT,      B,       A,       Dpad up, LT,      end LT,  end RT,  alt RT
	static ActionList ArmstrongBossActions = { 0x10000, 0x10001, 0x20000, 0x20003, 0x20007, 0x20006, 0x20001, 0x20009, 0x20010, 0x2000F, 0x10000, 0x20007 };
	static ActionList SamBossActions =       { 0x20000, 0x10002, 0x30004, 0x30006, 0x30007, 0x30014, 0x3001C, 0x10006, 0x30005, 0x20000, 0x30009, 0x30008 };
	static ActionList SundownerBossActions = { 0x10000, 0x10001, 0x20001, 0x20000, 0x20008, 0x20002, 0x20007, 0x10006, 0x20009, 0x10000, 0x10000, 0x20008 };
	// Default here for Armstrong (em0700)
	ActionList* BossActions = &ArmstrongBossActions;

	if (Enemy->m_pEntity->m_nEntityIndex == 0x20020) {
		BossActions = &SamBossActions;
	}
	if (Enemy->m_pEntity->m_nEntityIndex == 0x20310) {
		BossActions = &SundownerBossActions;
	}

	UpdateBossActions(Enemy, BossActions, controllerNumber);

	if (CanDamagePlayer)
		Enemy->field_640 = 2;
	else
		Enemy->field_640 = 1;


	//forward
	if (CheckControlPressed(controllerNumber, Forward, GamepadForward)) {
		field_Y = -1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadForward))
			field_Y *= GetGamepadAnalog(controllerNumber, GamepadForward);
	}

	//back
	if (CheckControlPressed(controllerNumber, Back, GamepadBack)) {
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
		if (Enemy->m_nCurrentAction == BossActions->Idle) Enemy->setState(BossActions->Walking, 0, 0, 0);
		float angle = atan2(field_X, field_Y);
		angle += camYaw;
		Enemy->m_vecRotation.y = angle;
	}
	else {
		if (Enemy->m_nCurrentAction == BossActions->Walking) Enemy->setState(BossActions->Idle, 0, 0, 0);
	}

	GetCameraInput(controllerNumber);

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

float getDistance(Pl0000* player1, Pl0000* player2) {
	cVec4 p1pos = player1->getTransPos();
	cVec4 p2pos = player2->getTransPos();
	float xDist = p1pos.x - p2pos.x;
	float yDist = p1pos.y - p2pos.y;
	float zDist = p1pos.z - p2pos.z;
	return sqrt(xDist*xDist + yDist*yDist + zDist*zDist);
}

float getAngle(Pl0000* player1, Pl0000* player2) {
	cVec4 p1pos = player1->getTransPos();
	cVec4 p2pos = player2->getTransPos();
	float xDist = p1pos.x - p2pos.x;
	float zDist = p1pos.z - p2pos.z;
	return atan2(xDist, zDist) + PI;
}

#define RADIANS(x) (x * PI / 180)

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

	// Left stick
	isAny |= SetFlagsForAnalog(player, controllerNumber, Forward, GamepadForward, ForwardBit, &player->field_D0C, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, Back, GamepadBack, BackwardBit, &player->field_D0C, false);
	isAny |= SetFlagsForAnalog(player, controllerNumber, Left, GamepadLeft, LeftBit, &player->field_D08, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, Right, GamepadRight, RightBit, &player->field_D08, false);

	// PVP Homing
	if (!isAny && EnableDamageToPlayers && !player->isIdle()) {
		Pl0000* nearestPlayer = nullptr;
		float nearestPlayerDist = INFINITY;
		for (Pl0000* player2 : players) {
			if (player2 == player || !player2)
				continue;
			float dist = getDistance(player, player2);
			if (dist < nearestPlayerDist) {
				nearestPlayer = player2;
				nearestPlayerDist = dist;
			}
		}

		if (nearestPlayer != nullptr) {
			static float turnSpeed = RADIANS(5);
			float curAngle = player->m_vecRotation.y;
			float targetAngle = getAngle(player, nearestPlayer);
			float diffAngle = targetAngle - curAngle;
			if (diffAngle < 0) diffAngle += 2 * PI;
			if (diffAngle >= 2 * PI) diffAngle -= 2 * PI;

			// Very close-- lock perfectly
			if (diffAngle < turnSpeed || diffAngle > 2*PI - turnSpeed)
				player->m_vecRotation.y = targetAngle;

			// "< PI" means it's faster to turn positively
			else if (diffAngle < PI) {
				player->m_vecRotation.y += turnSpeed;
				if (player->m_vecRotation.y >= 2 * PI) player->m_vecRotation.y -= 2 * PI;
			}
			else { // and vice versa
				player->m_vecRotation.y -= turnSpeed;
				if (player->m_vecRotation.y < 0) player->m_vecRotation.y += 2 * PI;
			}
		}
	}
	// Right stick
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamUp, GamepadCamUp, CamUpBit, &player->field_D14, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamDown, GamepadCamDown, CamDownBit, &player->field_D14, false);
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamLeft, GamepadCamLeft, CamLeftBit, &player->field_D10, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamRight, GamepadCamRight, CamRightBit, &player->field_D10, false);
	// Face buttons
	isAny |= SetFlagsForAction(player, controllerNumber, NormalAttack, GamepadNormalAttack, LightAttackBit, &player->field_D04);
	isAny |= SetFlagsForAction(player, controllerNumber, StrongAttack, GamepadStrongAttack, HeavyAttackBit, &player->field_D04);
	isAny |= SetFlagsForAction(player, controllerNumber, Interact, GamepadInteract, InteractBit);
	isAny |= SetFlagsForAction(player, controllerNumber, Jump, GamepadJump, JumpBit, &player->field_D00, &player->field_D04);
	// Triggers and bumpers (lock-on takes camera control also)
	isAny |= SetFlagsForAction(player, controllerNumber, Lockon, GamepadLockon, LockOnBit);
	isAny |= SetFlagsForAction(player, controllerNumber, Run, GamepadRun, RunBit, &player->field_D00, &player->field_D04);
	if (!SetFlagsForAction(player, controllerNumber, BladeMode, GamepadBladeMode, BladeModeBit, &player->field_D00, &player->field_D04)) {
		GetCameraInput(controllerNumber); // No camera control in Blade Mode
	}
	else {
		isAny |= true;
	}
	isAny |= SetFlagsForAction(player, controllerNumber, Subweapon, GamepadSubweapon, SubWeaponBit);
	// D-pad
	if (player->m_nModelIndex == 0x11400) {
		isAny |= SetFlagsForAction(player, controllerNumber, Taunt, GamepadTaunt, TauntBit);
	}
	else {
		// Synchronize AR vision for Wolf and Raiden
		isARPressed |= CheckControlPressed(controllerNumber, Taunt, GamepadTaunt);
		isAny |= isARPressed;
		if (player == MainPlayer) {
			if (isARPressed) {
				if (!wasARPressed) {
					player->m_nKeyPressedFlag |= TauntBit;
				}
				else {
					player->m_nKeyPressedFlag &= ~TauntBit;
				}
				player->m_nKeyHoldingFlag |= TauntBit;
			}
			else {
				player->m_nKeyHoldingFlag &= ~TauntBit;
				player->m_nKeyPressedFlag &= ~TauntBit;
			}
			wasARPressed = isARPressed;
			isARPressed = false;
		}
	}

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
	isAny |= SetFlagsForAction(player, controllerNumber, Ability, GamepadAbility, AbilityBit);
	isAny |= SetFlagsForAction(player, controllerNumber, CamReset, GamepadCamReset, CamResetBit);
	//isAny |= SetFlagsForAction(player, controllerNumber, Pause, GamepadPause, PauseBit); // Does not work
	//isAny |= SetFlagsForAction(player, controllerNumber, Pause2, GamepadPause2, CodecBit); // Non-Raidens don't have codec

	if (!isAny) {
		player->m_nKeyPressedFlag = 0;
		player->field_D04 = 0;
	}

}

float angleOfFuckYou[5]; // idk why this is needed but apparently it keeps the DG from making everything AR vision

void FullHandleDGPlayer(Behavior* dg, int controllerNumber, bool EnableDamageToPlayers) {
	int i = controllerNumber + 1;
	//if (EnableDamageToPlayers)
	//	player->field_640 = 0;
	//else
	//	player->field_640 = 1;

	Pl0000* player = (Pl0000*)((char*)dg + 0xE90 + 0xA4 - 0xCF8); // Hack to align the fields

	player->m_nKeyHoldingFlag = 0;
	player->field_D0C = 0;
	player->field_D08 = 0;
	player->field_D10 = 0;
	player->field_D14 = 0;

	// Aight we're stealing the rotation code from bosses because that locked itself up somehow
	float field_X = 0; float field_Y = 0;
	//forward
	if (CheckControlPressed(controllerNumber, Forward, GamepadForward)) {
		field_Y = -1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadForward)) field_Y *= GetGamepadAnalog(controllerNumber, GamepadForward);
	}

	//back
	if (CheckControlPressed(controllerNumber, Back, GamepadBack)) {
		field_Y = 1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadBack)) field_Y *= GetGamepadAnalog(controllerNumber, GamepadBack);
	}

	//left
	if (CheckControlPressed(controllerNumber, Left, GamepadLeft)) {
		field_X = -1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadLeft)) field_X *= GetGamepadAnalog(controllerNumber, GamepadLeft);
	}

	//right
	if (CheckControlPressed(controllerNumber, Right, GamepadRight)) {
		field_X = 1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadRight)) field_X *= GetGamepadAnalog(controllerNumber, GamepadRight);
	}

	if (field_X != 0 || field_Y != 0) {
		angleOfFuckYou[controllerNumber] = atan2(field_X, field_Y);
		angleOfFuckYou[controllerNumber] += camYaw;
		dg->m_vecRotation.y = angleOfFuckYou[controllerNumber];
		((float*)dg)[573] = angleOfFuckYou[controllerNumber];

		//printf("%f\n", angle * 360 / 2 / PI);
	}


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

	if ((controllerNumber == 0 && GetKeyState(std::stoi(Pause2, nullptr, 16)) & 0x8000) || IsGamepadButtonPressed(controllerNumber, GamepadPause2))
		TeleportToMainPlayer(MainPlayer, controllerNumber);


	bool isAny = false;

	// Left stick
	isAny |= SetFlagsForAnalog(player, controllerNumber, Forward, GamepadForward, ForwardBit, &player->field_D0C, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, Back, GamepadBack, BackwardBit, &player->field_D0C, false);
	isAny |= SetFlagsForAnalog(player, controllerNumber, Left, GamepadLeft, LeftBit, &player->field_D08, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, Right, GamepadRight, RightBit, &player->field_D08, false);

	// PVP Homing - disabled
	// Right stick
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamUp, GamepadCamUp, CamUpBit, &player->field_D14, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamDown, GamepadCamDown, CamDownBit, &player->field_D14, false);
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamLeft, GamepadCamLeft, CamLeftBit, &player->field_D10, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, CamRight, GamepadCamRight, CamRightBit, &player->field_D10, false);
	// Face buttons
	isAny |= SetFlagsForAction(player, controllerNumber, NormalAttack, GamepadNormalAttack, LightAttackBit, &player->field_D04);
	isAny |= SetFlagsForAction(player, controllerNumber, StrongAttack, GamepadStrongAttack, HeavyAttackBit, &player->field_D04);
	isAny |= SetFlagsForAction(player, controllerNumber, Interact, GamepadInteract, InteractBit);
	isAny |= SetFlagsForAction(player, controllerNumber, Jump, GamepadJump, JumpBit, &player->field_D00, &player->field_D04);
	// Triggers and bumpers (lock-on takes camera control also)
	isAny |= SetFlagsForAction(player, controllerNumber, Lockon, GamepadLockon, LockOnBit);
	isAny |= SetFlagsForAction(player, controllerNumber, Run, GamepadRun, RunBit, &player->field_D00, &player->field_D04);
	if (!SetFlagsForAction(player, controllerNumber, BladeMode, GamepadBladeMode, BladeModeBit, &player->field_D00, &player->field_D04)) {
		GetCameraInput(controllerNumber); // No camera control in Blade Mode
	}
	else {
		isAny |= true;
	}
	isAny |= SetFlagsForAction(player, controllerNumber, Subweapon, GamepadSubweapon, SubWeaponBit);
	// D-pad
	// No AR

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
	isAny |= SetFlagsForAction(player, controllerNumber, Ability, GamepadAbility, AbilityBit);
	isAny |= SetFlagsForAction(player, controllerNumber, CamReset, GamepadCamReset, CamResetBit);
	//isAny |= SetFlagsForAction(player, controllerNumber, Pause, GamepadPause, PauseBit); // Does not work
	//isAny |= SetFlagsForAction(player, controllerNumber, Pause2, GamepadPause2, CodecBit); // Non-Raidens don't have codec


}