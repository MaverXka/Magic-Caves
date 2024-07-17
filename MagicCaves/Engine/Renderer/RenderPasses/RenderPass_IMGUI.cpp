#include "RenderPass_IMGUI.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx12.h"
#include "ImGUI/imgui_impl_win32.h"

RenderPass_IMGUI::RenderPass_IMGUI()
{
	RenderPass();


}

void RenderPass_IMGUI::RenderThread_Prepare()
{
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), RenderPassCommandList.Get());
}
