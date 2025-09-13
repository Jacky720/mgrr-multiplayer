#pragma once
int __fastcall CameraHacked(void* ecx);
void OverrideCameraPos();
void GetCameraInput(int controllerNumber);

extern bool overrideCamera;
extern float camLateralScale;
extern float camHeightScale;
extern float camYaw;