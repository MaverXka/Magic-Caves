#pragma once

#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#define RENDERPASSLOG "Render Pass"

class RenderPass
{
public:

	RenderPass();

	virtual void RenderThread_Activate();

	ComPtr<ID3D12GraphicsCommandList> GetExecutableCommandList() { return RenderPassCommandList; };

protected:

	ComPtr<ID3D12GraphicsCommandList> RenderPassCommandList;


};

