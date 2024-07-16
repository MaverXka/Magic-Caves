#include "RenderingManager.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx12.h"
#include "ImGUI/imgui_impl_win32.h"


RenderingManager::RenderingManager(HWND hwnd) : window(hwnd)
{

	//if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS((ID3D12Debug**)&M_DebugController))))
	//{
	//	M_DebugController->EnableDebugLayer();
	//	LOG(RENDERLOG, "Debug layer enabled");
	//}

	IDXGIFactory1* Factory = nullptr;
    hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,__uuidof(IDXGIFactory1*), (void**)&Factory);
	LOGS(hr, RENDERLOG, "DXGI Factory created");
	hr = Factory->QueryInterface(__uuidof(IDXGIFactory7), (void**)&M_Factory);
	LOGS(hr, RENDERLOG, "DXGI Factory transformed to 7");
	

	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, __uuidof(ID3D12Device10), (void**)&M_Device);
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
	swapdesc.Width = 1920;
	swapdesc.Height = 1080;

	IDXGISwapChain1* swapchain = nullptr;
	hr = M_Factory->CreateSwapChainForHwnd(M_CommandQueue.get(), hwnd, &swapdesc, nullptr, nullptr, &swapchain);
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
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(M_RenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());
		for (int n = 0; n < FrameCount; n++)
		{
			hr = M_SwapChain->GetBuffer(n, __uuidof(ID3D12Resource2), (void**)&M_RenderTargets[n]);
			LOGS(hr, RENDERLOG, "Render Buffer found");
			M_Device->CreateRenderTargetView(M_RenderTargets[n].get(), nullptr, rtvHandle);
			rtvHandle.Offset(1,RTVDescriptorSize);
		}
	}

	EnableDebugRendering();

	RenderThread = std::thread(&RenderingManager::RenderThread_Render, this);
}

int rendered = 0;


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

		M_FrameIDX = M_SwapChain->GetCurrentBackBufferIndex();
		RenderThread_RenderDebugIMGUI();

		ID3D12CommandList* CMDS[] = { MD_ImGUICommandList.get() };
		M_CommandQueue->ExecuteCommandLists(ARRAYSIZE(CMDS), CMDS);

		hr = M_SwapChain->Present(0, 0);
		LOGF(hr, RENDERLOG, "Present failed");

		WaitForPreviousFrame();
;
		rendered++;

	}

}

void RenderingManager::RenderThread_RenderDebugIMGUI()
{
	hr = MD_ImGUICommandAllocator->Reset();
	LOGF(hr, RENDERLOG, "IMGUI Command allocator reset failed");
	hr = MD_ImGUICommandList->Reset(MD_ImGUICommandAllocator.get(), nullptr);
	LOGF(hr, RENDERLOG, "IMGUI Command list reset failed");



	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition((ID3D12Resource2*)M_RenderTargets[M_FrameIDX].get(), D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	MD_ImGUICommandList->ResourceBarrier(1, &barrier);


	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	bool win = true;
	ImGui::ShowDemoWindow(&win);

	ImGui::Render();

	float color[4] = { 1,0,0,1 };
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(M_RenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());
	rtvHandle.ptr += M_FrameIDX * RTVDescriptorSize;
	MD_ImGUICommandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
	MD_ImGUICommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	MD_ImGUICommandList->SetDescriptorHeaps(1, (ID3D12DescriptorHeap* const*)&MD_ImGUISrvDescriptorHeap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), MD_ImGUICommandList.get());


	auto barrier2 = CD3DX12_RESOURCE_BARRIER::Transition((ID3D12Resource2*)M_RenderTargets[M_FrameIDX].get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	MD_ImGUICommandList->ResourceBarrier(1, &barrier2);


	hr = MD_ImGUICommandList->Close();
	LOGF(hr, RENDERLOG, "IMGUI command list close failed");
	//LOGS(hr, RENDERLOG, "IMGUI command list close success");

}

void RenderingManager::WaitForPreviousFrame()
{
	const UINT64 fence = M_FenceValue;
	M_CommandQueue->Signal(M_Fence.get(), fence);
	M_FenceValue++;
	if (M_Fence->GetCompletedValue() < fence)
	{
		hr = M_Fence->SetEventOnCompletion(fence, M_FenceEvent);
		LOGF(hr, RENDERLOG, "Fence event failed");
		WaitForSingleObject(M_FenceEvent, INFINITE);
	}

}

void RenderingManager::EnableDebugRendering()
{
	D3D12_DESCRIPTOR_HEAP_DESC imguiHeapDesc = {};
	imguiHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	imguiHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	imguiHeapDesc.NumDescriptors = FrameCount;
	hr = M_Device->CreateDescriptorHeap(&imguiHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&MD_ImGUISrvDescriptorHeap);
	LOGS(hr, RENDERLOG, "IMGUI srv descriptor heap created");

	hr = M_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**) & MD_ImGUICommandAllocator);
	LOGS(hr, RENDERLOG, "IMGUI command allocator created");

	hr = M_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, MD_ImGUICommandAllocator.get(), nullptr, __uuidof(ID3D12GraphicsCommandList), (void**) & MD_ImGUICommandList);
	LOGS(hr, RENDERLOG, "IMGUI command list created");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	if (ImGui_ImplWin32_Init(window)) { LOG(RENDERLOG, "IMGUI Win32 Interface initialized"); }
	if (ImGui_ImplDX12_Init(M_Device.get(), FrameCount, DXGI_FORMAT_R8G8B8A8_UNORM, MD_ImGUISrvDescriptorHeap.get(),
		MD_ImGUISrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		MD_ImGUISrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()))
	{
		LOG(RENDERLOG, "IMGUI DX12 Interface initialized");
	}


	MD_ImGUICommandList->Close();

}


//Знаю что в данном контексте не хорошо использывать shared ptr, а comptr потом изменю
RenderingManager::~RenderingManager()
{
	MD_ImGUICommandAllocator->Release();
	MD_ImGUICommandList->Release();
	M_Factory->Release();
	M_Device->Release();
	M_CommandQueue->Release();
	M_SwapChain->Release();
	M_RenderTargetViewHeap->Release();
	M_RenderTargets[0]->Release();
	M_RenderTargets[1]->Release();
	M_DebugController->Release();
	M_Fence->Release();
}

