#pragma once

#include <iostream>
#include <thread>
#include <chrono>

#include <d3d12.h>
#include <dxgi.h>
#include <d3dx12.h>
#include <d3dx12_default.h>
#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>

#include "Logging/Logger.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#define RENDERLOG "Renderer"

class RenderingManager
{
public:

	RenderingManager(HWND hwnd);
	~RenderingManager();


	std::thread RenderThread;

private:

	void RenderThread_Render();

	//Debugging
	void EnableDebugRendering();
	void RenderThread_RenderDebugIMGUI();

	std::shared_ptr<ID3D12DescriptorHeap> MD_ImGUISrvDescriptorHeap;
	std::shared_ptr<ID3D12CommandAllocator> MD_ImGUICommandAllocator;
	std::shared_ptr<ID3D12GraphicsCommandList> MD_ImGUICommandList;

	//~End Debugging


	const static unsigned int FrameCount = 2;
	int RTVDescriptorSize = 0;

	std::shared_ptr<IDXGIFactory7> M_Factory;
	std::shared_ptr<ID3D12Device10> M_Device;
	std::shared_ptr<ID3D12CommandQueue> M_CommandQueue;
	std::shared_ptr<IDXGISwapChain4> M_SwapChain;
	std::shared_ptr<ID3D12DescriptorHeap> M_RenderTargetViewHeap;
	std::shared_ptr<ID3D12Resource2> M_RenderTargets[FrameCount];


	std::shared_ptr<ID3D12Debug> M_DebugController;

	//Sync
	void WaitForPreviousFrame();

	unsigned int M_FrameIDX;
	HANDLE M_FenceEvent;
	std::shared_ptr<ID3D12Fence> M_Fence;
	unsigned int M_FenceValue;




	HRESULT hr;
	HWND window;
};

