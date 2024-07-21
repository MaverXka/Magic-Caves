#include "RenderPass.h"
#include "Renderer/RenderProgram.h"

RenderPass::RenderPass()
{
	RenderProgram::Get()->M_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&RenderPassCommandAllocator));
	RenderProgram::Get()->M_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, RenderPassCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&RenderPassCommandList));
	RenderPassCommandList->Close();
}

void RenderPass::RenderThread_Prepare()
{
}
