#include "Camera.h"
#include "dllmain.h"
#include "MGRControls.h"
#include "MGRCustomAI.h"

#include <BehaviorEmBase.h>
#include <cCameraGame.h>
#include <Pl0000.h>
#include "MGRFunctions.h"
#include "MPPlayer.h"

extern Sub_18AE10_t sundownerPhase2Create;

bool isARPressed = false;
bool wasARPressed = false;

bool SetFlagsForAction(MPPlayer* player, std::string GamepadBind,
	InputBitflags bit, unsigned int* altField1, unsigned int* altField2, cInput::InputUnit* curInput) {
	Pl0000* playerObj = player->playerObj;
	if (!curInput) curInput = &playerObj->m_CurrentInput;
	if (CheckControlPressed(player->controlIndex, GamepadBind)) {
		curInput->m_nButtonsDown |= bit;
		if (!(player->prevPressed & bit)) {
			curInput->m_nButtonsPressed |= bit;
		}
		else {
			curInput->m_nButtonsPressed &= ~bit;
		}
		player->prevPressed |= bit;
		if (altField1) *altField1 |= bit;
		if (altField2) *altField2 |= bit;
		curInput->m_nButtonsReleased |= bit;
		curInput->m_nButtonsAlternated |= bit;
		return true;
	}
	else {
		curInput->m_nButtonsDown &= ~bit;
		curInput->m_nButtonsPressed &= ~bit;
		player->prevPressed &= ~bit;
		if (altField1) *altField1 &= ~bit;
		if (altField2) *altField2 &= ~bit;
		curInput->m_nButtonsReleased &= ~bit;
		curInput->m_nButtonsAlternated &= ~bit;
		return false;
	}
}

bool SetFlagsForAnalog(MPPlayer* player, std::string GamepadBind,
	InputBitflags bit, float* altField, bool invert, cInput::InputUnit *curInput) {
	Pl0000* playerObj = player->playerObj;
	if (!curInput) curInput = &playerObj->m_CurrentInput;
	if (CheckControlPressed(player->controlIndex, GamepadBind)) {
		curInput->m_nButtonsDown |= bit;
		curInput->m_nButtonsReleased |= bit;
		curInput->m_nButtonsAlternated |= bit;
		*altField = invert ? -1000.0f : 1000.0f;
		if (IsGamepadButtonPressed(player->controlIndex, GamepadBind))
			*altField *= GetGamepadAnalog(player->controlIndex, GamepadBind);
		return true;
	}
	curInput->m_nButtonsReleased &= ~bit;
	curInput->m_nButtonsAlternated &= ~bit;
	return false;
}

bool matchAction(MPPlayer* Enemy, setAction act) {
	if (Enemy->enemyObj->m_nCurrentAction != act.ActionId) return false;
	if (act.SubactionId != 0) {
		if (Enemy->enemyObj->m_nCurrentActionId != act.SubactionId) return false;
	}
	if (act.MaxSubaction != 0) {
		if ((unsigned)Enemy->enemyObj->m_nCurrentActionId > act.MaxSubaction) return false;
	}
	return true;
}

void useAction(MPPlayer* Enemy, setAction act) {
	Enemy->enemyObj->setState(act.ActionId, act.SubactionId, 0, 0);
	if (act.AnimationMapId) {
		((int(__thiscall*)(Behavior*, int, int, float, float, int, float, float))(shared::base + 0x6A4080))(Enemy->enemyObj, act.AnimationMapId, 0, act.AnimationLength, 1.0, 0x80000000, -1.0, 1.0);
	}
}

void UpdateBossActions(MPPlayer* Enemy, ActionList* BossActions) {

	// Armstrong move (Sam move)
	if (CheckControlPressed(Enemy->controlIndex, GamepadNormalAttack)
		&& !matchAction(Enemy, BossActions->LightAttack)) { // Two punches (four strikes)
		useAction(Enemy, BossActions->LightAttack);
	}

	if (CheckControlPressed(Enemy->controlIndex, GamepadStrongAttack)
		&& !matchAction(Enemy, BossActions->HeavyAttack)) { // Two punches, kick, punch (four strike w/ sheath)
		useAction(Enemy, BossActions->HeavyAttack);
	}

	if (CheckControlPressed(Enemy->controlIndex, GamepadRun)
		&& (
			CheckControlPressed(Enemy->controlIndex, GamepadForward)
		    || CheckControlPressed(Enemy->controlIndex, GamepadBack)
		    || CheckControlPressed(Enemy->controlIndex, GamepadLeft)
		    || CheckControlPressed(Enemy->controlIndex, GamepadRight))) { // Run QTE (Assault Rush)
		if (!matchAction(Enemy, BossActions->StartRun) && !matchAction(Enemy, BossActions->MidRun))
			useAction(Enemy, BossActions->StartRun);
	}
	else if (matchAction(Enemy, BossActions->MidRun)) { // Running should end
		useAction(Enemy, BossActions->EndRun);
	}

	if (CheckControlPressed(Enemy->controlIndex, GamepadInteract)
		&& !matchAction(Enemy, BossActions->Interaction)) { // Overhead with AOE (unblockable QTE)
		useAction(Enemy, BossActions->Interaction);
	}

	if (CheckControlPressed(Enemy->controlIndex, GamepadJump)
		&& !matchAction(Enemy, BossActions->Jumping)) { // Uppercut (perfect parry QTE fail)
		useAction(Enemy, BossActions->Jumping);
	}

	if (CheckControlPressed(Enemy->controlIndex, GamepadTaunt)
		&& !matchAction(Enemy, BossActions->Taunting)) { // Explode (taunt)
		useAction(Enemy, BossActions->Taunting);
	}

	if (CheckControlPressed(Enemy->controlIndex, GamepadBladeMode)) { // Heal (un-perfect-parryable four strike)
		if (!matchAction(Enemy, BossActions->Special))
			useAction(Enemy, BossActions->Special);
	}
	else if (matchAction(Enemy, BossActions->Special)) { // Blade Mode should end 
		useAction(Enemy, BossActions->EndSpecial);
	}
}

void FullHandleAIBoss(MPPlayer* Enemy) {

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
	bool CanDamagePlayer = EnableFriendlyFire;
	BehaviorEmBase* enemyObj = Enemy->enemyObj;

	if (CheckControlPressed(Enemy->controlIndex, GamepadPause2))
		TeleportToMainPlayer(MainPlayer, Enemy->controlIndex);



	//float rotation = 0;
	//float v9;
	float field_X = 0;
	float field_Y = 0;
	//float v10;
	//float v11;
	//cVec4 v54;
	//long double tann = 0;
	//forward

	cCameraGame camera = cCameraGame::Instance;
	//UpdateMovement(Enemy, &camera);


	static ActionList ArmstrongBossActions = {
		{0x10000}, // Idle
		{0x10001}, // Walk
		{0x20000}, // (X) Two punch combo
		{0x20003}, // (Y) Two punches, kick, punch
		{0x20006}, // (B) AOE overhead
		{0x20001}, // (A) Uppercut
		{0x20009}, // (D-pad up) Explode
		{0x2000F, 0, 3}, // (LT) Heal (cycles)
		{0x2000F, 4}, // (end LT) End heal
		{0x20007},    // (RT) Start run
		{0x20007, 3}, // (mid RT) Still run
		{0x20007, 4}  // (end RT) End run
	};
	static ActionList SamBossActions       = {
		{0x20000}, // Idle
		{0x10002}, // Walk (unused)
		{0x30004}, // (X) Four strike combo
		{0x30006}, // (Y) Different four strike combo, with sheath
		{0x30014}, // (B) Yellow QTE attack
		{0x3001C}, // (A) QTE fail, TODO: replace because it's scripted to damage P1 and not who you aim at
		{0x10006}, // (D-pad up) Taunt
		{0x30005}, // (LT) Un-parryable four strike combo (you know, that one)
		{0x20000}, // (end LT) Idle
		{0x30007}, // (RT) Start Assault Rush
		{0x30008}, // (mid RT) Continued Assault Rush
		{0x30009}  // (end RT) End Assault Rush
	};
	static ActionList SundownerBossActions = {
		{0x10000}, // Idle
		{0x10001}, // Walk
		{0x20001}, // (X) Uppercut
		{0x20000}, // (Y) Overhead
		{0x20002}, // (B) Combo attack
		{0x20007}, // (A) Shields behind + turn around
		{0x10006}, // (D-pad up) Taunt
		{0x20009, 0, 2}, // (LT) Shield bash, TODO: ends early if not facing a player
		{0x20009, 3, 0, 0x43, 2.0f/15.0f}, // (end LT) Shield bash end
		{0x20008},    // (RT) Charge attack, TODO: ends early if too close to a player
		{0x20008, 2}, // (mid RT) Still charge attack
		{0x20008, 3, 0, 0x3F, 2.0f/15.0f} // (end RT) Charge attack end
	};

	static ActionList Sundowner2BossActions = {
		{0x10000}, // Idle
		{0x10001}, // Walk
		{0x20011, 1}, // (X) Uppercut
		{0x20010}, // (Y) Combo A
		{0x2000E}, // (B) Combo Yellow
		{0x50000}, // (A) Jump Back
		{0x10006}, // (D-pad up) Taunt
		{0x40007, 1, 2}, // (LT) Block
		{0, 3, 0, 0x43, 2.0f / 15.0f}, // (end LT) Shield bash end
		{0},    // (RT) Charge attack, TODO: ends early if too close to a player
		{0, 2}, // (mid RT) Still charge attack
		{0, 3, 0, 0x3F, 2.0f / 15.0f} // (end RT) Charge attack end
	};

	// Default here for Armstrong (em0700)
	ActionList* BossActions = &ArmstrongBossActions;

	if (Enemy->playerType == 0x20020) {
		BossActions = &SamBossActions;
	}
	if (Enemy->playerType == 0x20310) {
		if (Enemy->isSundownerPhase2) {
			
			BossActions = &Sundowner2BossActions;
			if (!Enemy->sundownerInitialized) {
				sundownerPhase2Create(enemyObj, 0);
				Enemy->sundownerInitialized = true;
			}
			
		}
		else {
			BossActions = &SundownerBossActions;
		}
		
	}

	UpdateBossActions(Enemy, BossActions);

	if (CanDamagePlayer)
		enemyObj->field_640 = 2;
	else
		enemyObj->field_640 = 1;


	//forward
	if (CheckControlPressed(Enemy->controlIndex, GamepadForward)) {
		field_Y = -1000;
		if (IsGamepadButtonPressed(Enemy->controlIndex, GamepadForward))
			field_Y *= GetGamepadAnalog(Enemy->controlIndex, GamepadForward);
	}

	//back
	if (CheckControlPressed(Enemy->controlIndex, GamepadBack)) {
		field_Y = 1000;
		if (IsGamepadButtonPressed(Enemy->controlIndex, GamepadBack))
			field_Y *= GetGamepadAnalog(Enemy->controlIndex, GamepadBack);
	}

	//left
	if (CheckControlPressed(Enemy->controlIndex, GamepadLeft)) {
		field_X = -1000;
		if (IsGamepadButtonPressed(Enemy->controlIndex, GamepadLeft))
			field_X *= GetGamepadAnalog(Enemy->controlIndex, GamepadLeft);
	}

	//right
	if (CheckControlPressed(Enemy->controlIndex, GamepadRight)) {
		field_X = 1000;
		if (IsGamepadButtonPressed(Enemy->controlIndex, GamepadRight))
			field_X *= GetGamepadAnalog(Enemy->controlIndex, GamepadRight);
	}

	if (field_X != 0 || field_Y != 0) {
		if (matchAction(Enemy, BossActions->Idle)) useAction(Enemy, BossActions->Walking);
		float angle = atan2(field_X, field_Y);
		angle += (float)camYaw;
		enemyObj->m_vecRotation.y = angle;
	}
	else {
		if (matchAction(Enemy, BossActions->Walking)) useAction(Enemy, BossActions->Idle);
	}

	GetCameraInput(Enemy->controlIndex);

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

float getDistance(Behavior* player1, Behavior* player2) {
	cVec4 p1pos = player1->getTransPos();
	cVec4 p2pos = player2->getTransPos();
	float xDist = p1pos.x - p2pos.x;
	float yDist = p1pos.y - p2pos.y;
	float zDist = p1pos.z - p2pos.z;
	return sqrt(xDist*xDist + yDist*yDist + zDist*zDist);
}

float getAngle(Behavior* player1, Behavior* player2) {
	cVec4 p1pos = player1->getTransPos();
	cVec4 p2pos = player2->getTransPos();
	float xDist = p1pos.x - p2pos.x;
	float zDist = p1pos.z - p2pos.z;
	return atan2(xDist, zDist) + PI;
}

#define RADIANS(x) (x * PI / 180)

void FullHandleAIPlayer(MPPlayer* player) {
	Pl0000* playerObj = player->playerObj;
	if (EnableFriendlyFire)
		playerObj->field_640 = 0;
	else
		playerObj->field_640 = 1;

	playerObj->m_CurrentInput.m_nButtonsDown = 0;
	playerObj->m_CurrentInput.m_fLeftStick.x = 0;
	playerObj->m_CurrentInput.m_fLeftStick.y = 0;
	playerObj->m_CurrentInput.m_fRightStick.x = 0;
	playerObj->m_CurrentInput.m_fRightStick.y = 0;


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

	if ((GetKeyState('8') & 0x8000) || IsGamepadButtonPressed(player->controlIndex, GamepadLockon))
		giveVanillaCameraControl(playerObj);

	if (CheckControlPressed(player->controlIndex, GamepadPause2))
		TeleportToMainPlayer(MainPlayer, player->controlIndex);


	bool isAny = false;

	// Left stick
	isAny |= SetFlagsForAnalog(player, GamepadForward, ForwardBit, &playerObj->m_CurrentInput.m_fLeftStick.y, true);
	isAny |= SetFlagsForAnalog(player, GamepadBack, BackwardBit, &playerObj->m_CurrentInput.m_fLeftStick.y, false);
	isAny |= SetFlagsForAnalog(player, GamepadLeft, LeftBit, &playerObj->m_CurrentInput.m_fLeftStick.x, true);
	isAny |= SetFlagsForAnalog(player, GamepadRight, RightBit, &playerObj->m_CurrentInput.m_fLeftStick.x, false);

	// PVP Homing
	if (!isAny && EnableFriendlyFire && !playerObj->isIdle()) {
		Pl0000* nearestPlayer = nullptr;
		float nearestPlayerDist = INFINITY;
		for (MPPlayer* player2 : players) {
			if (player2 == player || !player2)
				continue;
			float dist = getDistance(playerObj, player2->playerObj);
			if (dist < nearestPlayerDist) {
				nearestPlayer = player2->playerObj;
				nearestPlayerDist = dist;
			}
		}

		if (nearestPlayer != nullptr) {
			static float turnSpeed = RADIANS(5);
			float curAngle = playerObj->m_vecRotation.y;
			float targetAngle = getAngle(playerObj, nearestPlayer);
			float diffAngle = targetAngle - curAngle;
			if (diffAngle < 0) diffAngle += 2 * PI;
			if (diffAngle >= 2 * PI) diffAngle -= 2 * PI;

			// Very close-- lock perfectly
			if (diffAngle < turnSpeed || diffAngle > 2*PI - turnSpeed)
				playerObj->m_vecRotation.y = targetAngle;

			// "< PI" means it's faster to turn positively
			else if (diffAngle < PI) {
				playerObj->m_vecRotation.y += turnSpeed;
				if (playerObj->m_vecRotation.y >= 2 * PI) playerObj->m_vecRotation.y -= 2 * PI;
			}
			else { // and vice versa
				playerObj->m_vecRotation.y -= turnSpeed;
				if (playerObj->m_vecRotation.y < 0) playerObj->m_vecRotation.y += 2 * PI;
			}
		}
	}
	// Right stick
	isAny |= SetFlagsForAnalog(player, GamepadCamUp, CamUpBit, &playerObj->m_CurrentInput.m_fRightStick.y, true);
	isAny |= SetFlagsForAnalog(player, GamepadCamDown, CamDownBit, &playerObj->m_CurrentInput.m_fRightStick.y, false);
	isAny |= SetFlagsForAnalog(player, GamepadCamLeft, CamLeftBit, &playerObj->m_CurrentInput.m_fRightStick.x, true);
	isAny |= SetFlagsForAnalog(player, GamepadCamRight, CamRightBit, &playerObj->m_CurrentInput.m_fRightStick.x, false);
	// Face buttons
	isAny |= SetFlagsForAction(player, GamepadNormalAttack, LightAttackBit, &playerObj->m_CurrentInput.m_nButtonsAlternated);
	isAny |= SetFlagsForAction(player, GamepadStrongAttack, HeavyAttackBit, &playerObj->m_CurrentInput.m_nButtonsAlternated);
	isAny |= SetFlagsForAction(player, GamepadInteract, InteractBit);
	isAny |= SetFlagsForAction(player, GamepadJump, JumpBit, &playerObj->m_CurrentInput.m_nButtonsReleased, &playerObj->m_CurrentInput.m_nButtonsAlternated);
	// Triggers and bumpers (lock-on takes camera control also)
	isAny |= SetFlagsForAction(player, GamepadLockon, LockOnBit);
	isAny |= SetFlagsForAction(player, GamepadRun, RunBit, &playerObj->m_CurrentInput.m_nButtonsReleased, &playerObj->m_CurrentInput.m_nButtonsAlternated);
	if (!SetFlagsForAction(player, GamepadBladeMode, BladeModeBit, &playerObj->m_CurrentInput.m_nButtonsReleased, &playerObj->m_CurrentInput.m_nButtonsAlternated)) {
		GetCameraInput(player->controlIndex); // No camera control in Blade Mode
	}
	else {
		isAny |= true;
	}
	isAny |= SetFlagsForAction(player, GamepadSubweapon, SubWeaponBit);
	// D-pad
	if (player->playerType == 0x11400) {
		isAny |= SetFlagsForAction(player, GamepadTaunt, TauntBit);
	}
	else {
		// Synchronize AR vision for Wolf and Raiden
		isARPressed |= CheckControlPressed(player->controlIndex, GamepadTaunt);
		isAny |= isARPressed;
		Pl0000* firstARPlayer = playerObj;
		for (int i = 0; i < 5; i++) {
			if (players[i]->playerType == 0x10010 || players[i]->playerType == 0x11500) {
				firstARPlayer = players[i]->playerObj;
				break;
			}
		}
		if (playerObj == firstARPlayer) {
			if (isARPressed) {
				if (!wasARPressed) {
					playerObj->m_CurrentInput.m_nButtonsPressed |= TauntBit;
				}
				else {
					playerObj->m_CurrentInput.m_nButtonsPressed &= ~TauntBit;
				}
				playerObj->m_CurrentInput.m_nButtonsDown |= TauntBit;
			}
			else {
				playerObj->m_CurrentInput.m_nButtonsDown &= ~TauntBit;
				playerObj->m_CurrentInput.m_nButtonsPressed &= ~TauntBit;
			}
			wasARPressed = isARPressed;
			isARPressed = false;
		}
	}

	if (SetFlagsForAction(player, GamepadHeal, HealBit)) {
		isAny |= true;
		// Infinite nanopaste helper function, kept in but disabled by default
		if (EveryHeal && player->healTimer < 0) {
			player->healTimer = 30 * 60;
			playerObj->setHealth(playerObj->getMaxHealth());
		}
	}
	if (player->healTimer > 0)
		player->healTimer--;
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadWeaponMenu, WeaponMenuBit);
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadWeaponMenu2, WeaponMenu2Bit);
	// Other
	isAny |= SetFlagsForAction(player, GamepadAbility, AbilityBit);
	isAny |= SetFlagsForAction(player, GamepadCamReset, CamResetBit);
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadPause, PauseBit); // Does not work
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadPause2, CodecBit); // Non-Raidens don't have codec

	if (!isAny) {
		playerObj->m_CurrentInput.m_nButtonsPressed = 0;
		playerObj->m_CurrentInput.m_nButtonsAlternated = 0;
	}

}

void FullHandleDGPlayer(MPPlayer* dg) {
	//if (EnableDamageToPlayers)
	//	player->field_640 = 0;
	//else
	//	player->field_640 = 1;

	Pl0000* player = (Pl0000*)((char*)(dg->dgObj) + 0xE90 + 0xA4 - 0xCF8); // Hack to align the fields
	
	player->m_CurrentInput.m_nButtonsDown = 0;
	player->m_CurrentInput.m_fLeftStick.x = 0;
	player->m_CurrentInput.m_fLeftStick.y = 0;
	player->m_CurrentInput.m_fRightStick.x = 0;
	player->m_CurrentInput.m_fRightStick.y = 0;

	// Aight we're stealing the rotation code from bosses because that locked itself up somehow
	float field_X = 0; float field_Y = 0;
	//forward
	if (CheckControlPressed(dg->controlIndex, GamepadForward)) {
		field_Y = -1000;
		if (IsGamepadButtonPressed(dg->controlIndex, GamepadForward)) field_Y *= GetGamepadAnalog(dg->controlIndex, GamepadForward);
	}

	//back
	if (CheckControlPressed(dg->controlIndex, GamepadBack)) {
		field_Y = 1000;
		if (IsGamepadButtonPressed(dg->controlIndex, GamepadBack)) field_Y *= GetGamepadAnalog(dg->controlIndex, GamepadBack);
	}

	//left
	if (CheckControlPressed(dg->controlIndex, GamepadLeft)) {
		field_X = -1000;
		if (IsGamepadButtonPressed(dg->controlIndex, GamepadLeft)) field_X *= GetGamepadAnalog(dg->controlIndex, GamepadLeft);
	}

	//right
	if (CheckControlPressed(dg->controlIndex, GamepadRight)) {
		field_X = 1000;
		if (IsGamepadButtonPressed(dg->controlIndex, GamepadRight)) field_X *= GetGamepadAnalog(dg->controlIndex, GamepadRight);
	}

	if (field_X != 0 || field_Y != 0) {
		dg->dwarfGekkoAngle = atan2(field_X, field_Y);
		dg->dwarfGekkoAngle += (float)camYaw;
		dg->dgObj->m_vecRotation.y = dg->dwarfGekkoAngle;
		((float*)(dg->dgObj))[573] = dg->dwarfGekkoAngle;

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

	if (CheckControlPressed(dg->controlIndex, GamepadPause2))
		TeleportToMainPlayer(MainPlayer, dg->controlIndex);


	bool isAny = false;

	// Left stick
	isAny |= SetFlagsForAnalog(dg, GamepadForward, ForwardBit, &player->m_CurrentInput.m_fLeftStick.y, true, &player->m_CurrentInput);
	isAny |= SetFlagsForAnalog(dg, GamepadBack, BackwardBit, &player->m_CurrentInput.m_fLeftStick.y, false, &player->m_CurrentInput);
	isAny |= SetFlagsForAnalog(dg, GamepadLeft, LeftBit, &player->m_CurrentInput.m_fLeftStick.x, true, &player->m_CurrentInput);
	isAny |= SetFlagsForAnalog(dg, GamepadRight, RightBit, &player->m_CurrentInput.m_fLeftStick.x, false, &player->m_CurrentInput);

	// PVP Homing - disabled
	// Right stick
	isAny |= SetFlagsForAnalog(dg, GamepadCamUp, CamUpBit, &player->m_CurrentInput.m_fRightStick.y, true, &player->m_CurrentInput);
	isAny |= SetFlagsForAnalog(dg, GamepadCamDown, CamDownBit, &player->m_CurrentInput.m_fRightStick.y, false, &player->m_CurrentInput);
	isAny |= SetFlagsForAnalog(dg, GamepadCamLeft, CamLeftBit, &player->m_CurrentInput.m_fRightStick.x, true, &player->m_CurrentInput);
	isAny |= SetFlagsForAnalog(dg, GamepadCamRight, CamRightBit, &player->m_CurrentInput.m_fRightStick.x, false, &player->m_CurrentInput);
	// Face buttons
	isAny |= SetFlagsForAction(dg, GamepadNormalAttack, LightAttackBit, &player->m_CurrentInput.m_nButtonsAlternated, nullptr, &player->m_CurrentInput);
	isAny |= SetFlagsForAction(dg, GamepadStrongAttack, HeavyAttackBit, &player->m_CurrentInput.m_nButtonsAlternated, nullptr, &player->m_CurrentInput);
	isAny |= SetFlagsForAction(dg, GamepadInteract, InteractBit, nullptr, nullptr, &player->m_CurrentInput);
	isAny |= SetFlagsForAction(dg, GamepadJump, JumpBit, &player->m_CurrentInput.m_nButtonsReleased, &player->m_CurrentInput.m_nButtonsAlternated, &player->m_CurrentInput);
	// Triggers and bumpers (lock-on takes camera control also)
	isAny |= SetFlagsForAction(dg, GamepadLockon, LockOnBit, nullptr, nullptr, &player->m_CurrentInput);
	isAny |= SetFlagsForAction(dg, GamepadRun, RunBit, &player->m_CurrentInput.m_nButtonsReleased, &player->m_CurrentInput.m_nButtonsAlternated, &player->m_CurrentInput);
	if (!SetFlagsForAction(dg, GamepadBladeMode, BladeModeBit, &player->m_CurrentInput.m_nButtonsReleased, &player->m_CurrentInput.m_nButtonsAlternated, &player->m_CurrentInput)) {
		GetCameraInput(dg->controlIndex); // No camera control in Blade Mode
	}
	else {
		isAny |= true;
	}
	isAny |= SetFlagsForAction(dg, GamepadSubweapon, SubWeaponBit, nullptr, nullptr, &player->m_CurrentInput);
	// D-pad
	// No AR

	if (SetFlagsForAction(dg, GamepadHeal, HealBit, nullptr, nullptr, &player->m_CurrentInput)) {
		isAny |= true;
		// Infinite nanopaste helper function, kept in but disabled by default
		if (EveryHeal && dg->healTimer < 0) {
			dg->healTimer = 30 * 60;
			player->setHealth(player->getMaxHealth());
		}
	}
	if (dg->healTimer > 0)
		dg->healTimer--;
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadWeaponMenu, WeaponMenuBit);
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadWeaponMenu2, WeaponMenu2Bit);
	// Other
	isAny |= SetFlagsForAction(dg, GamepadAbility, AbilityBit, nullptr, nullptr, &player->m_CurrentInput);
	isAny |= SetFlagsForAction(dg, GamepadCamReset, CamResetBit, nullptr, nullptr, &player->m_CurrentInput);
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadPause, PauseBit); // Does not work
	//isAny |= SetFlagsForAction(player, controllerNumber, GamepadPause2, CodecBit); // Non-Raidens don't have codec


}