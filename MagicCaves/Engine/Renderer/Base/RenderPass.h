#pragma once

#include "pch.h"

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

	ComPtr<ID3D12GraphicsCommandList> GetExecutableCommandList() { return RenderPassCommandList; };

protected:

	ComPtr<ID3D12GraphicsCommandList> RenderPassCommandList;


};

