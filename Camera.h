#pragma once
int __fastcall CameraHacked(void* ecx);
void LoadCameraConfig();
void OverrideCameraPos();
void GetCameraInput(int controllerNumber);

extern bool overrideCamera;
extern bool customCamera;
extern double camLateralScale;
extern double camHeightScale;
extern double camLateralMin;
extern double camLateralMax;
extern double camHeightMin;
extern double camHeightMax;
extern double camYaw;
extern double camSensitivity;
extern double zoomInFOV;
extern double zoomOutFOV;
extern bool invertCameraY;
extern bool enableCameraY;
extern bool qteCamera;
