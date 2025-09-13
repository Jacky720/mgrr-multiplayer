#pragma once
int __fastcall CameraHacked(void* ecx);
void OverrideCameraPos();
void GetCameraInput(int controllerNumber);

extern bool overrideCamera;
extern double camLateralScale;
extern double camHeightScale;
extern double camYaw;