#pragma once
int __fastcall CameraHacked(void* ecx);
void LoadCameraConfig() noexcept;
void OverrideCameraPos();
void GetCameraInput(int controllerNumber);

__declspec(selectany) bool overrideCamera = false;
__declspec(selectany) bool customCamera = true;
__declspec(selectany) double camLateralScale = 1.0;
__declspec(selectany) double camHeightScale = 5.0;
__declspec(selectany) double camLateralMin = 1.0;
__declspec(selectany) double camLateralMax = 6.0;
__declspec(selectany) double camHeightMin = -2.5;
__declspec(selectany) double camHeightMax = 10.0;
__declspec(selectany) double camYaw = 0.0;
__declspec(selectany) double camSensitivity = 1.0;
__declspec(selectany) double zoomInFOV = 50.0;
__declspec(selectany) double zoomOutFOV = 75.0;
__declspec(selectany) bool invertCameraY = false;
__declspec(selectany) bool enableCameraY = true;
__declspec(selectany) bool qteCamera = true;
__declspec(selectany) bool zoomOut = false;
