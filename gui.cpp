#include "gui.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include <Hw.h>

static WNDPROC oWndProc = NULL;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK hkWindowProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam) > 0)
		return 1L;
	return ::CallWindowProcA(oWndProc, hwnd, uMsg, wParam, lParam);
}

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
	static bool init = false;

	if (!init)
	{
		oWndProc = (WNDPROC)::SetWindowLongPtr(Hw::OSWindow, GWLP_WNDPROC, (LONG)hkWindowProc);

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(Hw::OSWindow);
		ImGui_ImplDX9_Init(Hw::GraphicDevice);

		gui::LoadStyle();

		init = true;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	gui::RenderWindow();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void gui::LoadStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	
	// your style settings here
}