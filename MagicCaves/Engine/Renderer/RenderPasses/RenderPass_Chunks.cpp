#include "RenderPass_Chunks.h"
#include "Engine.h"
#include "../World/ChunkyWorld/ChunkyWorld.h"
#include "../Renderer/View/Camera.h"
#include "../World/Chunks/ChunkRendering.h"

RenderPass_Chunks::RenderPass_Chunks(ChunkRendering* NewChunkRenderer) : ChunkRenderer(NewChunkRenderer)
{
	RenderPass::RenderPass();
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

	Camera* renderViewCamera = Camera::GetMainCamera();
	renderViewCamera->SetCameraPosition(-4, 0, 2);
	auto ViewMatrix = renderViewCamera->GetProjectionViewMatrix();

	ChunkConstantBuffer cb = {};
	cb.ProjectionViewMatrix = ViewMatrix;
	cb.c = 1;
	//cb.ProjectionView = ViewMatrix;
	//cb.X = 46;
	//cb.Y = 43;
	//cb.R = 130;
	//cb.G = 120;

	CD3DX12_RANGE readRange(0, 0);
	ChunkRenderer->M_ChunkConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&Data));
	memcpy(Data, &cb, sizeof(ChunkConstantBuffer));
	ChunkRenderer->M_ChunkConstantBuffer->Unmap(0, nullptr);
	//ChunkRenderer->M_ChunkConstantBuffer->Unmap(0, nullptr);

	ID3D12DescriptorHeap* heaps[] = { ChunkRenderer->M_CBDescriptorHeap.Get() };
	RenderPassCommandList->SetGraphicsRootSignature(ChunkRenderer->M_ChunkRootSignature.Get());
	//RenderPassCommandList->SetDescriptorHeaps(ARRAYSIZE(heaps), heaps);
	//RenderPassCommandList->SetGraphicsRootDescriptorTable(0, ChunkRenderer->M_CBDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	D3D12_VIEWPORT VP = D3D12_VIEWPORT(0, 0, 1280, 720);
	D3D12_RECT RCT = D3D12_RECT(0, 0, 1280, 720);
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
			RenderPassCommandList->SetGraphicsRootConstantBufferView(0, ChunkRenderer->M_ChunkConstantBuffer->GetGPUVirtualAddress());
			RenderPassCommandList->IASetVertexBuffers(0, 1, &CWorld->WorldChunks[i]->ChunkVertexBufferView);
			RenderPassCommandList->DrawInstanced(3, 1, 0, 0);
		}
	}

	auto td2 = CD3DX12_RESOURCE_BARRIER::Transition(RenderProgram::Get()->MainRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	RenderPassCommandList->ResourceBarrier(1, &td2);

	RenderPassCommandList->Close();
}
