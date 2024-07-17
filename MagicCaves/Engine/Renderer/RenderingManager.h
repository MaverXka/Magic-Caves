#pragma once

#include <iostream>
#include <thread>
#include <chrono>

#include <d3d12.h>
#include <dxgi.h>
#include <cd3dx12/d3dx12.h>
#include <cd3dx12/d3dx12_default.h>
#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>

#include "Logging/Logger.h"
#include <wrl/client.h>

#include "../World/Chunks/ChunkRendering.h"

using namespace Microsoft::WRL;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class RenderPass;
class Config;

#define RENDERLOG "Renderer"

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
	static RenderProgram* Get();

	ComPtr<ID3D12Device10> M_Device;

};

class RenderingManager
{
public:

	RenderingManager(HWND hwnd, Config* config);
	~RenderingManager();


	/// <returns>Rendering Device</returns>
	ComPtr<ID3D12Device10> GetRenderingDevice() { return M_Device; };

	void RegisterRenderPass(RenderPass renderPass);

	ChunkRendering* ChunkRenderer;

	std::thread RenderThread;

private:

	void RenderThread_Render();

	//Debugging
	ComPtr<ID3D12Debug> M_DebugController;
	//~End Debugging


	const static unsigned int FrameCount = 2;
	int RTVDescriptorSize = 0;

	ComPtr<IDXGIFactory7> M_Factory;
	ComPtr<ID3D12Device10> M_Device;
	ComPtr<ID3D12CommandQueue> M_CommandQueue;
	ComPtr<IDXGISwapChain4> M_SwapChain;
	ComPtr<ID3D12DescriptorHeap> M_RenderTargetViewHeap;
	ComPtr<ID3D12Resource2> M_RenderTargets[FrameCount];


	//Sync
	void WaitForPreviousFrame();

	unsigned int M_FrameIDX;
	HANDLE M_FenceEvent;
	ComPtr<ID3D12Fence> M_Fence;
	unsigned int M_FenceValue;

	std::vector<RenderPass> RegisteredRenderPasses;


	Config* GameConfig;
	HRESULT hr;
	HWND window;
};

