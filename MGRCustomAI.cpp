#include "Camera.h"
#include "dllmain.h"
#include "MGRControls.h"

#include <BehaviorEmBase.h>
#include <cCameraGame.h>
#include <Pl0000.h>

int healTimers[5] = { -1, -1, -1, -1, -1 };
int prevPressed[5] = { 0 };
bool EveryHeal = false;
bool isARPressed = false;
bool wasARPressed = false;

bool SetFlagsForAction(Pl0000* player, int controllerNumber, std::string GamepadBind,
	InputBitflags bit, unsigned int* altField1 = nullptr, unsigned int* altField2 = nullptr) {
	int playerIndex = -1;
	for (int i = 0; i < 5; i++) {
		if (players[i] == player)
			playerIndex = i;
	}

	if (CheckControlPressed(controllerNumber, GamepadBind)) {
		player->m_CurrentInput.m_nButtonsDown |= bit;
		if (!(prevPressed[playerIndex] & bit)) {
			player->m_CurrentInput.m_nButtonsPressed |= bit;
		}
		else {
			player->m_CurrentInput.m_nButtonsPressed &= ~bit;
		}
		prevPressed[playerIndex] |= bit;
		if (altField1) *altField1 |= bit;
		if (altField2) *altField2 |= bit;
		player->m_CurrentInput.m_nButtonsReleased |= bit;
		player->m_CurrentInput.m_nButtonsAlternated |= bit;
		return true;
	}
	else {
		player->m_CurrentInput.m_nButtonsDown &= ~bit;
		player->m_CurrentInput.m_nButtonsPressed &= ~bit;
		prevPressed[playerIndex] &= ~bit;
		if (altField1) *altField1 &= ~bit;
		if (altField2) *altField2 &= ~bit;
		player->m_CurrentInput.m_nButtonsReleased &= ~bit;
		player->m_CurrentInput.m_nButtonsAlternated &= ~bit;
		return false;
	}
}

bool SetFlagsForAnalog(Pl0000* player, int controllerNumber, std::string GamepadBind,
	InputBitflags bit, float* altField, bool invert) {
	if (CheckControlPressed(controllerNumber, GamepadBind)) {
		player->m_CurrentInput.m_nButtonsDown |= bit;
		player->m_CurrentInput.m_nButtonsReleased |= bit;
		player->m_CurrentInput.m_nButtonsAlternated |= bit;
		*altField = invert ? -1000.0f : 1000.0f;
		if (IsGamepadButtonPressed(controllerNumber, GamepadBind))
			*altField *= GetGamepadAnalog(controllerNumber, GamepadBind);
		return true;
	}
	player->m_CurrentInput.m_nButtonsReleased &= ~bit;
	player->m_CurrentInput.m_nButtonsAlternated &= ~bit;
	return false;
}

struct setAction {
	unsigned int ActionId;
	unsigned int SubactionId;
	unsigned int MaxSubaction; // For checking states that contain a range of subactions 0-x
	unsigned int AnimationMapId;
	float AnimationLength;
};


typedef struct actionList {
	setAction Idle;
	setAction Walking;
	setAction LightAttack;
	setAction HeavyAttack;
	setAction StartRun;
	setAction Interaction;
	setAction Jumping;
	setAction Taunting;
	setAction Special;
	setAction EndSpecial;
	setAction EndRun;
	setAction MidRun;
} ActionList;

bool matchAction(BehaviorEmBase* Enemy, setAction act) {
	if (Enemy->m_nCurrentAction != act.ActionId) return false;
	if (act.SubactionId != 0) {
		if (Enemy->m_nCurrentActionId != act.SubactionId) return false;
	}
	if (act.MaxSubaction != 0) {
		if (Enemy->m_nCurrentActionId > act.MaxSubaction) return false;
	}
	return true;
}

void useAction(BehaviorEmBase* Enemy, setAction act) {
	Enemy->setState(act.ActionId, act.SubactionId, 0, 0);
	if (act.AnimationMapId) {
		((int(__thiscall*)(Behavior*, int, int, float, float, int, float, float))(shared::base + 0x6A4080))(Enemy, act.AnimationMapId, 0, act.AnimationLength, 1.0, 0x80000000, -1.0, 1.0);
	}
}

void UpdateBossActions(BehaviorEmBase* Enemy, ActionList* BossActions, int controllerNumber = -1) {

	// Armstrong move (Sam move)
	if (CheckControlPressed(controllerNumber, GamepadNormalAttack)
		&& !matchAction(Enemy, BossActions->LightAttack)) { // Two punches (four strikes)
		useAction(Enemy, BossActions->LightAttack);
	}

	if (CheckControlPressed(controllerNumber, GamepadStrongAttack)
		&& !matchAction(Enemy, BossActions->HeavyAttack)) { // Two punches, kick, punch (four strike w/ sheath)
		useAction(Enemy, BossActions->HeavyAttack);
	}

	if (CheckControlPressed(controllerNumber, GamepadRun)
		&& (
			CheckControlPressed(controllerNumber, GamepadForward)
		    || CheckControlPressed(controllerNumber, GamepadBack)
		    || CheckControlPressed(controllerNumber, GamepadLeft)
		    || CheckControlPressed(controllerNumber, GamepadRight))) { // Run QTE (Assault Rush)
		if (!matchAction(Enemy, BossActions->StartRun) && !matchAction(Enemy, BossActions->MidRun))
			useAction(Enemy, BossActions->StartRun);
	}
	else if (matchAction(Enemy, BossActions->MidRun)) { // Running should end
		useAction(Enemy, BossActions->EndRun);
	}

	if (CheckControlPressed(controllerNumber, GamepadInteract)
		&& !matchAction(Enemy, BossActions->Interaction)) { // Overhead with AOE (unblockable QTE)
		useAction(Enemy, BossActions->Interaction);
	}

	if (CheckControlPressed(controllerNumber, GamepadJump)
		&& !matchAction(Enemy, BossActions->Jumping)) { // Uppercut (perfect parry QTE fail)
		useAction(Enemy, BossActions->Jumping);
	}

	if (CheckControlPressed(controllerNumber, GamepadTaunt)
		&& !matchAction(Enemy, BossActions->Taunting)) { // Explode (taunt)
		useAction(Enemy, BossActions->Taunting);
	}

	if (CheckControlPressed(controllerNumber, GamepadBladeMode)) { // Heal (un-perfect-parryable four strike)
		if (!matchAction(Enemy, BossActions->Special))
			useAction(Enemy, BossActions->Special);
	}
	else if (matchAction(Enemy, BossActions->Special)) { // Blade Mode should end 
		useAction(Enemy, BossActions->EndSpecial);
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

	if (CheckControlPressed(controllerNumber, GamepadPause2))
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


	static ActionList ArmstrongBossActions = {
		{0x10000}, // Idle
		{0x10001}, // Walk
		{0x20000}, // (X) Two punch combo
		{0x20003}, // (Y) Two punches, kick, punch
		{0x20007}, // (RT) Start run
		{0x20006}, // (B) AOE overhead
		{0x20001}, // (A) Uppercut
		{0x20009}, // (D-pad up) Explode
		{0x2000F, 0, 3}, // (LT) Heal (cycles)
		{0x2000F, 4}, // (end LT) End heal
		{0x20007, 4}, // (end RT) End run
		{0x20007, 3}  // (mid RT) Still run
	};
	static ActionList SamBossActions       = {
		{0x20000}, // Idle
		{0x10002}, // Walk (unused)
		{0x30004}, // (X) Four strike combo
		{0x30006}, // (Y) Different four strike combo, with sheath
		{0x30007}, // (RT) Start Assault Rush
		{0x30014}, // (B) Yellow QTE attack
		{0x3001C}, // (A) QTE fail, TODO: replace because it's scripted to damage P1 and not who you aim at
		{0x10006}, // (D-pad up) Taunt
		{0x30005}, // (LT) Un-parryable four strike combo (you know, that one)
		{0x20000}, // (end LT) Idle
		{0x30009}, // (end RT) End Assault Rush
		{0x30008}  // (mid RT) Continued Assault Rush
	};
	static ActionList SundownerBossActions = {
		{0x10000}, // Idle
		{0x10001}, // Walk
		{0x20001}, // (X) Uppercut
		{0x20000}, // (Y) Overhead
		{0x20008}, // (RT) Charge attack, TODO: ends early if too close to a player
		{0x20002}, // (B) Combo attack
		{0x20007}, // (A) Shields behind + turn around
		{0x10006}, // (D-pad up) Taunt
		{0x20009, 0, 2}, // (LT) Shield bash, TODO: ends early if not facing a player
		{0x20009, 3, 0, 0x43, 2.0/15.0}, // (end LT) Shield bash end
		{0x20008, 3, 0, 0x3F, 2.0/15.0}, // (end RT) Charge attack end
		{0x20008, 2}  // (mid RT) Still charge attack
	};
	// Default here for Armstrong (em0700)
	ActionList* BossActions = &ArmstrongBossActions;

	if (Enemy->m_pEntity->m_EntityIndex == 0x20020) {
		BossActions = &SamBossActions;
	}
	if (Enemy->m_pEntity->m_EntityIndex == 0x20310) {
		BossActions = &SundownerBossActions;
	}

	UpdateBossActions(Enemy, BossActions, controllerNumber);

	if (CanDamagePlayer)
		Enemy->field_640 = 2;
	else
		Enemy->field_640 = 1;


	//forward
	if (CheckControlPressed(controllerNumber, GamepadForward)) {
		field_Y = -1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadForward))
			field_Y *= GetGamepadAnalog(controllerNumber, GamepadForward);
	}

	//back
	if (CheckControlPressed(controllerNumber, GamepadBack)) {
		field_Y = 1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadBack))
			field_Y *= GetGamepadAnalog(controllerNumber, GamepadBack);
	}

	//left
	if (CheckControlPressed(controllerNumber, GamepadLeft)) {
		field_X = -1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadLeft))
			field_X *= GetGamepadAnalog(controllerNumber, GamepadLeft);
	}

	//right
	if (CheckControlPressed(controllerNumber, GamepadRight)) {
		field_X = 1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadRight))
			field_X *= GetGamepadAnalog(controllerNumber, GamepadRight);
	}

	if (field_X != 0 || field_Y != 0) {
		if (matchAction(Enemy, BossActions->Idle)) useAction(Enemy, BossActions->Walking);
		float angle = atan2(field_X, field_Y);
		angle += camYaw;
		Enemy->m_vecRotation.y = angle;
	}
	else {
		if (matchAction(Enemy, BossActions->Walking)) useAction(Enemy, BossActions->Idle);
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

	player->m_CurrentInput.m_nButtonsDown = 0;
	player->m_CurrentInput.m_fLeftStick.x = 0;
	player->m_CurrentInput.m_fLeftStick.y = 0;
	player->m_CurrentInput.m_fRightStick.x = 0;
	player->m_CurrentInput.m_fRightStick.y = 0;


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

	if (CheckControlPressed(controllerNumber, GamepadPause2))
		TeleportToMainPlayer(MainPlayer, controllerNumber);


	bool isAny = false;

	// Left stick
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadForward, ForwardBit, &player->m_CurrentInput.m_fLeftStick.y, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadBack, BackwardBit, &player->m_CurrentInput.m_fLeftStick.y, false);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadLeft, LeftBit, &player->m_CurrentInput.m_fLeftStick.x, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadRight, RightBit, &player->m_CurrentInput.m_fLeftStick.x, false);

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
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadCamUp, CamUpBit, &player->m_CurrentInput.m_fRightStick.y, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadCamDown, CamDownBit, &player->m_CurrentInput.m_fRightStick.y, false);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadCamLeft, CamLeftBit, &player->m_CurrentInput.m_fRightStick.x, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadCamRight, CamRightBit, &player->m_CurrentInput.m_fRightStick.x, false);
	// Face buttons
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadNormalAttack, LightAttackBit, &player->m_CurrentInput.m_nButtonsAlternated);
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadStrongAttack, HeavyAttackBit, &player->m_CurrentInput.m_nButtonsAlternated);
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadInteract, InteractBit);
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadJump, JumpBit, &player->m_CurrentInput.m_nButtonsReleased, &player->m_CurrentInput.m_nButtonsAlternated);
	// Triggers and bumpers (lock-on takes camera control also)
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadLockon, LockOnBit);
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadRun, RunBit, &player->m_CurrentInput.m_nButtonsReleased, &player->m_CurrentInput.m_nButtonsAlternated);
	if (!SetFlagsForAction(player, controllerNumber, GamepadBladeMode, BladeModeBit, &player->m_CurrentInput.m_nButtonsReleased, &player->m_CurrentInput.m_nButtonsAlternated)) {
		GetCameraInput(controllerNumber); // No camera control in Blade Mode
	}
	else {
		isAny |= true;
	}
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadSubweapon, SubWeaponBit);
	// D-pad
	if (player->m_ModelIndex == 0x11400) {
		isAny |= SetFlagsForAction(player, controllerNumber, GamepadTaunt, TauntBit);
	}
	else {
		// Synchronize AR vision for Wolf and Raiden
		isARPressed |= CheckControlPressed(controllerNumber, GamepadTaunt);
		isAny |= isARPressed;
		if (player == MainPlayer) {
			if (isARPressed) {
				if (!wasARPressed) {
					player->m_CurrentInput.m_nButtonsPressed |= TauntBit;
				}
				else {
					player->m_CurrentInput.m_nButtonsPressed &= ~TauntBit;
				}
				player->m_CurrentInput.m_nButtonsDown |= TauntBit;
			}
			else {
				player->m_CurrentInput.m_nButtonsDown &= ~TauntBit;
				player->m_CurrentInput.m_nButtonsPressed &= ~TauntBit;
			}
			wasARPressed = isARPressed;
			isARPressed = false;
		}
	}

	if (SetFlagsForAction(player, controllerNumber, GamepadHeal, HealBit)) { // Plays effect, does not heal
		isAny |= true;
		if (EveryHeal && healTimers[i] <= 0) {
			healTimers[i] = 30 * 60;
			player->setHealth(player->getMaxHealth());
		}
	}
	if (healTimers[i] > 0)
		healTimers[i]--;
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadWeaponMenu, WeaponMenuBit);
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadWeaponMenu2, WeaponMenu2Bit);
	// Other
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadAbility, AbilityBit);
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadCamReset, CamResetBit);
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadPause, PauseBit); // Does not work
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadPause2, CodecBit); // Non-Raidens don't have codec

	if (!isAny) {
		player->m_CurrentInput.m_nButtonsPressed = 0;
		player->m_CurrentInput.m_nButtonsAlternated = 0;
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

	player->m_CurrentInput.m_nButtonsDown = 0;
	player->m_CurrentInput.m_fLeftStick.x = 0;
	player->m_CurrentInput.m_fLeftStick.y = 0;
	player->m_CurrentInput.m_fRightStick.x = 0;
	player->m_CurrentInput.m_fRightStick.y = 0;

	// Aight we're stealing the rotation code from bosses because that locked itself up somehow
	float field_X = 0; float field_Y = 0;
	//forward
	if (CheckControlPressed(controllerNumber, GamepadForward)) {
		field_Y = -1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadForward)) field_Y *= GetGamepadAnalog(controllerNumber, GamepadForward);
	}

	//back
	if (CheckControlPressed(controllerNumber, GamepadBack)) {
		field_Y = 1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadBack)) field_Y *= GetGamepadAnalog(controllerNumber, GamepadBack);
	}

	//left
	if (CheckControlPressed(controllerNumber, GamepadLeft)) {
		field_X = -1000;
		if (IsGamepadButtonPressed(controllerNumber, GamepadLeft)) field_X *= GetGamepadAnalog(controllerNumber, GamepadLeft);
	}

	//right
	if (CheckControlPressed(controllerNumber, GamepadRight)) {
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

	if (CheckControlPressed(controllerNumber, GamepadPause2))
		TeleportToMainPlayer(MainPlayer, controllerNumber);


	bool isAny = false;

	// Left stick
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadForward, ForwardBit, &player->m_CurrentInput.m_fLeftStick.y, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadBack, BackwardBit, &player->m_CurrentInput.m_fLeftStick.y, false);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadLeft, LeftBit, &player->m_CurrentInput.m_fLeftStick.x, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadRight, RightBit, &player->m_CurrentInput.m_fLeftStick.x, false);

	// PVP Homing - disabled
	// Right stick
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadCamUp, CamUpBit, &player->m_CurrentInput.m_fRightStick.y, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadCamDown, CamDownBit, &player->m_CurrentInput.m_fRightStick.y, false);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadCamLeft, CamLeftBit, &player->m_CurrentInput.m_fRightStick.x, true);
	isAny |= SetFlagsForAnalog(player, controllerNumber, GamepadCamRight, CamRightBit, &player->m_CurrentInput.m_fRightStick.x, false);
	// Face buttons
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadNormalAttack, LightAttackBit, &player->m_CurrentInput.m_nButtonsAlternated);
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadStrongAttack, HeavyAttackBit, &player->m_CurrentInput.m_nButtonsAlternated);
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadInteract, InteractBit);
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadJump, JumpBit, &player->m_CurrentInput.m_nButtonsReleased, &player->m_CurrentInput.m_nButtonsAlternated);
	// Triggers and bumpers (lock-on takes camera control also)
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadLockon, LockOnBit);
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadRun, RunBit, &player->m_CurrentInput.m_nButtonsReleased, &player->m_CurrentInput.m_nButtonsAlternated);
	if (!SetFlagsForAction(player, controllerNumber, GamepadBladeMode, BladeModeBit, &player->m_CurrentInput.m_nButtonsReleased, &player->m_CurrentInput.m_nButtonsAlternated)) {
		GetCameraInput(controllerNumber); // No camera control in Blade Mode
	}
	else {
		isAny |= true;
	}
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadSubweapon, SubWeaponBit);
	// D-pad
	// No AR

	if (SetFlagsForAction(player, controllerNumber, GamepadHeal, HealBit)) { // Plays effect, does not heal
		isAny |= true;
		if (EveryHeal && healTimers[i] <= 0) {
			healTimers[i] = 30 * 60;
			player->setHealth(player->getMaxHealth());
		}
	}
	if (healTimers[i] > 0)
		healTimers[i]--;
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadWeaponMenu, WeaponMenuBit);
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadWeaponMenu2, WeaponMenu2Bit);
	// Other
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadAbility, AbilityBit);
	isAny |= SetFlagsForAction(player, controllerNumber, GamepadCamReset, CamResetBit);
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadPause, PauseBit); // Does not work
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadPause2, CodecBit); // Non-Raidens don't have codec


}