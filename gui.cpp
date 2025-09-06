#include "gui.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include <Hw.h>



void gui::OnReset::Before()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void gui::OnReset::After()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::OnEndScene()
{

}

void gui::LoadStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	
	// your style settings here
}