#include "RenderPass_Chunks.h"
#include "Engine.h"
#include "../World/Chunks/ChunkRendering.h"
#include "../World/ChunkyWorld/ChunkyWorld.h"
#include "../Renderer/View/Camera.h"



RenderPass_Chunks::RenderPass_Chunks(ChunkRendering* NewChunkRenderer) : ChunkRenderer(NewChunkRenderer)
{
	RenderPass::RenderPass();

	CD3DX12_RANGE readRange(0, 0);
	ChunkRenderer->M_ChunkConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&Data));
}

RenderPass_Chunks::~RenderPass_Chunks()
{
	ChunkRenderer->M_ChunkConstantBuffer->Unmap(0, nullptr);
}

void RenderPass_Chunks::RenderThread_Prepare()
{
	if (ChunkRenderer == nullptr)
	{
		LOG(RENDERPASSLOG, "Command list wirting failed chunk renderer was null");
		return;
	}
	hr = RenderPassCommandAllocator->Reset();
	LOGF(hr, RENDERPASSLOG, "Render pass command allocator reset failed");

	RenderPassCommandList->Reset(RenderPassCommandAllocator.Get(), ChunkRenderer->M_ChunkPSO.Get());

	ID3D12DescriptorHeap* heaps[] = { ChunkRenderer->M_CBDescriptorHeap.Get() };
	RenderPassCommandList->SetGraphicsRootSignature(ChunkRenderer->M_ChunkRootSignature.Get());

	int WinX = 0;
	int WinY = 0;
	Engine::GetEngine()->GetWindowSize(WinX, WinY);
	D3D12_VIEWPORT VP = D3D12_VIEWPORT(0, 0, WinX, WinY);
	D3D12_RECT RCT = D3D12_RECT(0, 0, WinX, WinY);
	RenderPassCommandList->RSSetViewports(1,&VP);
	RenderPassCommandList->RSSetScissorRects(1,&RCT);

	auto td = CD3DX12_RESOURCE_BARRIER::Transition(RenderProgram::Get()->MainRenderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	RenderPassCommandList->ResourceBarrier(1, &td);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(RenderProgram::Get()->RenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), RenderProgram::Get()->FrameIndex, RenderProgram::Get()->RTVDescriptorSize);
	RenderPassCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	if (Engine::GetEngine()->GetEngineWorld() == nullptr)
	{
		LOG(RENDERPASSLOG, "Engine world was null");
	}
	auto CWorld = reinterpret_cast<ChunkyWorld*>(Engine::GetEngine()->GetEngineWorld());
	if (CWorld == nullptr)
	{
		LOG(RENDERPASSLOG, "Engine chunky world was null");
	}

	float clearcolor[] = { 0.0, 0.0, 0.0, 0.0 };
	RenderPassCommandList->ClearRenderTargetView(rtvHandle, clearcolor, 0, nullptr);
	RenderPassCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = 0; i < CWorld->ChunkWorldPoolSize; i++)
	{
		if (CWorld->WorldChunks[i] != nullptr)
		{
			UpdateConstantBuffer(0, 0);

			RenderPassCommandList->SetGraphicsRootConstantBufferView(0, ChunkRenderer->M_ChunkConstantBuffer->GetGPUVirtualAddress());
			RenderPassCommandList->IASetVertexBuffers(0, 1, &CWorld->WorldChunks[i]->ChunkVertexBufferView);
			RenderPassCommandList->DrawInstanced(36, 1, 0, 0);
		}
	}

	auto td2 = CD3DX12_RESOURCE_BARRIER::Transition(RenderProgram::Get()->MainRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	RenderPassCommandList->ResourceBarrier(1, &td2);

	RenderPassCommandList->Close();
}

void RenderPass_Chunks::UpdateConstantBuffer(int OffsetX, int OffsetY)
{
	Camera* renderViewCamera = Camera::GetMainCamera();
	auto ViewMatrix = renderViewCamera->GetProjectionViewMatrix();
	constantBuffer.ProjectionViewMatrix = ViewMatrix;
	memcpy(Data, &constantBuffer, sizeof(ChunkConstantBuffer));
}
