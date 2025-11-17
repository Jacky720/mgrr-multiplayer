#include "dllmain.h"
#include "MGRCustomAI.h"

#include <cCameraGame.h>
#include <Trigger.h>

bool overrideCamera = false;
bool customCamera = true;
bool qteCamera = true;
bool invertCameraY = false;
bool enableCameraY = true;
bool zoomOut = false;
double camLateralScale = 1.0;
double camHeightScale = 5.0;
double camLateralMin = 1.0;
double camLateralMax = 6.0;
double camHeightMin = -2.5;
double camHeightMax = 10.0;
double camSensitivity = 1.0;
double camYaw = 0.0;
double zoomInFOV = 50.0;
double zoomOutFOV = 75.0;

int __fastcall CameraHacked(void* ecx) {
	if (overrideCamera) {
		//return ((INT(__thiscall*)(void*))(shared::base + 0x9d03e0))(ecx);
		return ((INT(__thiscall*)())(shared::base + 0x9d03e0))();
	}
	else {
		//return ((INT(__thiscall*)(void*, float))(shared::base + 0x9d1a30))(ecx, a2);
		return ((INT(__thiscall*)())(shared::base + 0x9d1a30))();
	}
}

void GetCameraInput(int controllerNumber) {
	// Camera handling
	float deltaYaw = 0.0;
	//left
	if (CheckControlPressed(controllerNumber, GamepadCamLeft)) {
		deltaYaw = -1;
		if (IsGamepadButtonPressed(controllerNumber, GamepadCamLeft))
			deltaYaw *= GetGamepadAnalog(controllerNumber, GamepadCamLeft);
	}

	//right
	if (CheckControlPressed(controllerNumber, GamepadCamRight)) {
		deltaYaw = 1;
		if (IsGamepadButtonPressed(controllerNumber, GamepadCamRight))
			deltaYaw *= GetGamepadAnalog(controllerNumber, GamepadCamRight);
	}

	camYaw -= deltaYaw / (2 * PI) / 3 * camSensitivity;
	if (camYaw < 0) camYaw += 2 * PI;
	if (camYaw > 2 * PI) camYaw -= 2 * PI;

	float deltaPitch = 0.0;
	//up
	if (CheckControlPressed(controllerNumber, GamepadCamUp)) {
		deltaPitch = -1;
		if (IsGamepadButtonPressed(controllerNumber, GamepadCamUp))
			deltaPitch *= GetGamepadAnalog(controllerNumber, GamepadCamUp);
	}

	//down
	if (CheckControlPressed(controllerNumber, GamepadCamDown)) {
		deltaPitch = 1;
		if (IsGamepadButtonPressed(controllerNumber, GamepadCamDown))
			deltaPitch *= GetGamepadAnalog(controllerNumber, GamepadCamDown);
	}

	if (invertCameraY) deltaPitch *= -1;
	if (!enableCameraY) return; // Note: also disables the limits on height/dist scale, so you can override in GUI

	camHeightScale += deltaPitch * (camHeightMax - camHeightMin) * 0.015 * camSensitivity;
	if (camHeightScale < camHeightMin) camHeightScale = camHeightMin;
	if (camHeightScale > camHeightMax) camHeightScale = camHeightMax;

	camLateralScale -= deltaPitch * (camLateralMax - camLateralMin) * 0.02 * camSensitivity;
	if (camLateralScale < camLateralMin) camLateralScale = camLateralMin;
	if (camLateralScale > camLateralMax) camLateralScale = camLateralMax;

	static bool zooming[5] = { false };
	if (CheckControlPressed(controllerNumber, GamepadCamReset) && !CheckControlPressed(controllerNumber, GamepadAbility)) {
		if (!zooming[controllerNumber])
			zoomOut = !zoomOut;
		zooming[controllerNumber] = true;
	}
	else
		zooming[controllerNumber] = false;
}

void OverrideCameraPos() {

	bool inQte = Trigger::StaFlags.STA_QTE || MainPlayer->m_nBladeModeType == 8;
	overrideCamera = !(inQte && qteCamera) && customCamera;
	if (!overrideCamera)
		return;

	cCameraGame* camera = &cCameraGame::Instance;
	cVec4* oldPos = &camera->m_TranslationMatrix.m_vecPosition;
	cVec4* oldTarget = &camera->m_TranslationMatrix.m_vecLookAtPosition;

	float maxDist = 0.0;
	cVec4 targetCenter = MainPlayer->m_vecTransPos;
	targetCenter.y += 1.0;
	cVec4 cameraPos = { 0.0, 0.0, 0.0, 1.0 };

#define getYaw(x, z) (((z) != 0) ? atan((x)/(z)) : DegreeToRadian(90))

	for (Pl0000* player : players) {
		if (!player) continue;
		cVec4 p1Pos = player->m_vecTransPos;
		for (Pl0000* player2 : players) {
			if (!player2) continue;
			cVec4 p2Pos = player2->m_vecTransPos;
			float dist = sqrt((p2Pos.x - p1Pos.x) * (p2Pos.x - p1Pos.x)
				+ (p2Pos.z - p1Pos.z) * (p2Pos.z - p1Pos.z));
			if (dist >= 15.0) {
				// Move players closer
				float distMoveBack = (dist - 15.0f) / 2;
				float xVecNrm = (p2Pos.x - p1Pos.x) / dist;
				float zVecNrm = (p2Pos.z - p1Pos.z) / dist;
				player->m_vecTransPos.x += distMoveBack * xVecNrm;
				player->m_vecTransPos.z += distMoveBack * zVecNrm;

				player2->m_vecTransPos.x -= distMoveBack * xVecNrm;
				player2->m_vecTransPos.z -= distMoveBack * zVecNrm;
			}
			if (dist >= maxDist) {
				maxDist = dist;
				targetCenter.x = p1Pos.x / 2 + p2Pos.x / 2;
				targetCenter.z = p1Pos.z / 2 + p2Pos.z / 2;
			}
		}
		targetCenter.y = (float)min(targetCenter.y, p1Pos.y + 1.0);
	}
	if (maxDist < 5.0) maxDist = 5.0;
	maxDist /= 5.0;
	cameraPos.x = targetCenter.x + (float)(maxDist * camLateralScale * sin(camYaw));
	cameraPos.y = targetCenter.y + (float)(maxDist * camHeightScale);
	cameraPos.z = targetCenter.z + (float)(maxDist * camLateralScale * cos(camYaw));

	/* // Old implementation, more horizontal
	float curYaw = getYaw(oldTarget->x - oldPos->x, oldTarget->z - oldPos->z);
	float newDirection[2];
	for (int i = 0; i < playerCount; i++) {
		for (int j = i; j < playerCount; j++) {
			float dist = sqrt((playerPos[j].x - playerPos[i].x) * (playerPos[j].x - playerPos[i].x)
				+ (playerPos[j].z - playerPos[i].z) * (playerPos[j].z - playerPos[i].z));
			if (dist >= maxDist) {
				maxDist = dist;
				targetCenter.x = playerPos[j].x / 2 + playerPos[i].x / 2;
				targetCenter.z = playerPos[j].z / 2 + playerPos[i].z / 2;
				newDirection[0] = -(playerPos[j].z - playerPos[i].z);
				newDirection[1] = (playerPos[j].x - playerPos[i].x);
				float newYaw = getYaw(newDirection[0], newDirection[1]);
				float yawDiff = newYaw - curYaw;
				if (yawDiff < 0.0) {
					yawDiff += 2 * PI;
				}
				else if (yawDiff >= 2 * PI) {
					yawDiff -= 2 * PI;
				}
				if (yawDiff > PI / 2 && yawDiff < 3 * PI / 2) {
					newDirection[0] *= -1.0;
					newDirection[1] *= -1.0;
				}
			}
		}
		targetCenter.y = min(targetCenter.y, playerPos[i].y + 1.0);
	}
	cameraPos.y = targetCenter.y + 2.0 * camHeightScale;


	//cameraPos.x -= 10.0 * playerCount;
	if (maxDist <= 0.2) { // Don't die
		newDirection[0] = 0.0;
		newDirection[1] = -3.0;
	} else if (maxDist <= 3.0) { // Limit zoom
		newDirection[0] *= 3.0 / maxDist;
		newDirection[1] *= 3.0 / maxDist;
	}
	// Screw you, top down only
	newDirection[0] = 0.0;
	newDirection[1] = -3.0;
	cameraPos.y += maxDist * camHeightScale;
	newDirection[0] *= camLateralScale;
	newDirection[1] *= camLateralScale;

	cameraPos.x = targetCenter.x + newDirection[0];
	cameraPos.z = targetCenter.z + newDirection[1];
	//cameraPos.z -= 1.0;
	//cameraPos.y += 15.0;
	*/

#define posUpdateSpeed 0.2f
	oldPos->x = oldPos->x * (1 - posUpdateSpeed) + cameraPos.x * posUpdateSpeed;
	oldPos->y = oldPos->y * (1 - posUpdateSpeed) + cameraPos.y * posUpdateSpeed;
	oldPos->z = oldPos->z * (1 - posUpdateSpeed) + cameraPos.z * posUpdateSpeed;

	oldTarget->x = oldTarget->x * (1 - posUpdateSpeed) + targetCenter.x * posUpdateSpeed;
	oldTarget->y = oldTarget->y * (1 - posUpdateSpeed) + targetCenter.y * posUpdateSpeed;
	oldTarget->z = oldTarget->z * (1 - posUpdateSpeed) + targetCenter.z * posUpdateSpeed;

	Hw::cVec4* cameraOffset = &camera->m_TranslationMatrix.m_vecCameraOffset;
	// Reset roll
	cameraOffset->x = 0.0;
	cameraOffset->y = 0.0;
	cameraOffset->z = 0.0;
	cameraOffset->w = 0.0;
	camera->m_TranslationMatrix.m_fFOV = (zoomOut ? zoomOutFOV : zoomInFOV) * PI / 180;
}