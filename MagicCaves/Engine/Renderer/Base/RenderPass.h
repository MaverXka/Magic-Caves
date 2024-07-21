#pragma once

#include "pch.h"

#include "Renderer/RenderProgram.h"

#define RENDERPASSLOG "Render Pass"


struct ID3D12GraphicsCommandList;

enum RenderPassType
{
	Dynamic = 0,
	Static = 1
};

class RenderPass
{
public:

	RenderPass();

	/// <summary>
	/// Prepearing render pass to draw. Filling command ist etc.
	/// </summary>
	virtual void RenderThread_Prepare();

	virtual RenderPassType GetRenderPassType() { return RenderPassType::Dynamic; };

	ComPtr<ID3D12CommandAllocator> GetExecutableCommandAllocator() { return RenderPassCommandAllocator; }
	ComPtr<ID3D12GraphicsCommandList> GetExecutableCommandList() { return RenderPassCommandList; };

protected:

	HRESULT hr;

	ComPtr<ID3D12CommandAllocator> RenderPassCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> RenderPassCommandList;


};

