#include "RenderingManager.h"

#include "Base/RenderPass.h"
#include "RenderPasses/RenderPass_IMGUI.h"
#include "RenderPasses/RenderPass_Chunks.h"
#include "RenderProgram.h"



#include "../Misc/Config/Config.h"


RenderProgram* RenderProgram::Singleton = nullptr;

RenderingManager::RenderingManager(HWND hwnd,Config* config, Engine* engine) : window(hwnd), GameConfig(config), EnginePtr(engine)
{

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS((ID3D12Debug**)&M_DebugController))))
	{
		M_DebugController->EnableDebugLayer();
		LOG(RENDERLOG, "Debug layer enabled");
	}

	IDXGIFactory2* Factory = nullptr;
    hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,__uuidof(IDXGIFactory2*), (void**)&Factory);
	LOGS(hr, RENDERLOG, "DXGI Factory created");
	M_Factory = Factory;
	//hr = Factory->QueryInterface(__uuidof(IDXGIFactory2), (void**)&M_Factory);
	//LOGS(hr, RENDERLOG, "DXGI Factory transformed to 2");
	
	M_Factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, __uuidof(ID3D12Device10), &M_Device);
	LOGS(hr, RENDERLOG, "D3D12 Device created");

	D3D12_COMMAND_QUEUE_DESC queuedesc = {};
	queuedesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queuedesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hr = M_Device->CreateCommandQueue(&queuedesc, __uuidof(ID3D12CommandQueue),(void**)&M_CommandQueue);
	LOGS(hr, RENDERLOG, "Command queue created");

	DXGI_SWAP_CHAIN_DESC1 swapdesc = {};
	swapdesc.Stereo = FALSE;
	swapdesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapdesc.BufferCount = FrameCount;
	swapdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapdesc.SampleDesc.Count = 1;
	swapdesc.SampleDesc.Quality = 0;
	swapdesc.Scaling = DXGI_SCALING_NONE;
	swapdesc.Flags = 0;
	swapdesc.Width = GameConfig->windowSizeX;
	swapdesc.Height = GameConfig->windowSizeY;

	IDXGISwapChain1* swapchain = nullptr;
	hr = M_Factory->CreateSwapChainForHwnd(M_CommandQueue.Get(), hwnd, &swapdesc, nullptr, nullptr, &swapchain);
	LOGS(hr, RENDERLOG, "DXGI Swap chain created");

	hr = swapchain->QueryInterface(__uuidof(IDXGISwapChain4), (void**)&M_SwapChain);
	LOGS(hr, RENDERLOG, "DXGI Swapchain transformed to 4");

	M_FrameIDX = M_SwapChain->GetCurrentBackBufferIndex();

	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NumDescriptors = FrameCount;
		hr = M_Device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&M_RenderTargetViewHeap);
		LOGS(hr, RENDERLOG, "Render target view heap created");

		RTVDescriptorSize = M_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	{
		CreateSwapChainRenderTargets();
	}
	RenderProgram::Get()->M_Device = M_Device;
	RenderProgram::Get()->RenderTargetViewHeap = M_RenderTargetViewHeap.Get();
	RenderProgram::Get()->RTVDescriptorSize = RTVDescriptorSize;

	

	ChunkRenderer = new ChunkRendering(this);
	RegisterRenderPass(new RenderPass_Chunks(ChunkRenderer));

	RenderThread = std::thread(&RenderingManager::RenderThread_Render, this);
	LOG(RENDERLOG, "Render thread started");
}

void RenderingManager::RenderThread_Render()
{
	M_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&M_Fence);
	M_FenceValue = 1;
	M_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (M_FenceEvent == nullptr)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		LOGF(hr, RENDERLOG, "Fence was null");
	}

	WaitForPreviousFrame();

	while (true)
	{
		if (!CanRender()) continue;
		std::vector<ID3D12GraphicsCommandList*> Commands;
		M_FrameIDX = M_SwapChain->GetCurrentBackBufferIndex();
		RenderProgram::Get()->MainRenderTarget = M_RenderTargets[M_FrameIDX].Get();
		RenderProgram::Get()->FrameIndex = M_FrameIDX;

		for (int i = 0; i < RegisteredRenderPasses.size(); i++)
		{
			RegisteredRenderPasses[i]->RenderThread_Prepare();
			auto cmdlist = RegisteredRenderPasses[i]->GetExecutableCommandList().Get();
			if (cmdlist == nullptr) continue;
			Commands.push_back(cmdlist);
		}

		M_CommandQueue->ExecuteCommandLists(Commands.size(), (ID3D12CommandList*const*)Commands.data());
		//LOG(RENDERLOG, Commands.size());
		

		hr = M_SwapChain->Present(1, 0);
		LOGF(hr, RENDERLOG, "Present failed");

		WaitForPreviousFrame();

	}

}

void RenderingManager::RegisterRenderPass(RenderPass* renderPass)
{
	RegisteredRenderPasses.push_back(renderPass);
}

void RenderingManager::ResizeWindow()
{
	if (M_SwapChain == nullptr) return;
	WaitForPreviousFrame();
	int X, Y;
	Engine::GetEngine()->GetWindowSize(X, Y);

	for (int n = 0; n < FrameCount; n++)
	{
		M_RenderTargets->Reset();
		M_RenderTargets[n] = nullptr;
	}

	hr = M_SwapChain->ResizeBuffers(FrameCount, X, Y, DXGI_FORMAT_UNKNOWN, 0);
	CreateSwapChainRenderTargets();
	LOGF(hr,WINDOWLOG, "Swap chain resize failed");

}

void RenderingManager::CreateSwapChainRenderTargets()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(M_RenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());
	for (int n = 0; n < FrameCount; n++)
	{
		hr = M_SwapChain->GetBuffer(n, __uuidof(ID3D12Resource2), (void**)&M_RenderTargets[n]);
		//LOGS(hr, RENDERLOG, "Render Buffer found");
		M_Device->CreateRenderTargetView(M_RenderTargets[n].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, RTVDescriptorSize);
	}
}

void RenderingManager::WaitForPreviousFrame()
{
	const UINT64 fence = M_FenceValue;
	M_CommandQueue->Signal(M_Fence.Get(), fence);
	M_FenceValue++;
	if (M_Fence->GetCompletedValue() < fence)
	{
		hr = M_Fence->SetEventOnCompletion(fence, M_FenceEvent);
		LOGF(hr, RENDERLOG, "Fence event failed");
		WaitForSingleObject(M_FenceEvent, INFINITE);
	}
}

//Знаю что в данном контексте не хорошо использывать shared ptr, а comptr потом изменю
RenderingManager::~RenderingManager()
{
	WaitForPreviousFrame();
	for (int n = 0; n < FrameCount; n++)
	{
		M_RenderTargets->Reset();
		M_RenderTargets[n] = nullptr;
	}
	M_Factory.Reset();
	M_Device.Reset();
	M_CommandQueue.Reset();
	M_SwapChain.Reset();
	M_RenderTargetViewHeap.Reset();
	M_RenderTargets[0].Reset();
	M_RenderTargets[1].Reset();
	M_DebugController.Reset();
	M_Fence.Reset();
}
