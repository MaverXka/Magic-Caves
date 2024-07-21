#pragma once

#include "pch.h"

struct RenderProgram
{
protected:
	RenderProgram()
	{

	}

	static RenderProgram* Singleton;

public:

	RenderProgram(RenderProgram& other) = delete;
	void operator=(const RenderProgram&) = delete;
	static RenderProgram* Get()
	{
		if (Singleton == nullptr)
		{
			Singleton = new RenderProgram();
		}
		return Singleton;
	}

	ComPtr<ID3D12Device10> M_Device;
	int FrameIndex = 0;
	int RTVDescriptorSize = 0;
	ID3D12DescriptorHeap* RenderTargetViewHeap;
	ID3D12Resource2* MainRenderTarget;

};
