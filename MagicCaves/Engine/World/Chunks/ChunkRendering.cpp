#include "ChunkRendering.h"
#include "Renderer/RenderingManager.h"
#include "ChunkData.h"
#include "../Misc/Platform/PlatformBinaryReader.h"


D3D12_INPUT_ELEMENT_DESC chunkVertexElementDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
};

ChunkRendering::ChunkRendering(RenderingManager* renderer) : MainRenderer(renderer)
{

	if (renderer == nullptr)
		LOG(CHUNKRENDERLOG, "Renderer reference was null");

	M_Device = renderer->GetRenderingDevice();

	D3D12_DESCRIPTOR_HEAP_DESC CBVHeapDesc = {};
	CBVHeapDesc.NumDescriptors = 1;
	CBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	CBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	hr = M_Device->CreateDescriptorHeap(&CBVHeapDesc, IID_PPV_ARGS(&M_CBDescriptorHeap));
	M_CBDescriptorHeap->SetName(L"Chunk Constant Buffer Heap");
	LOGS(hr, CHUNKRENDERLOG, "Constant buffer view heap created");


	CD3DX12_DESCRIPTOR_RANGE ranges = {};
	CD3DX12_ROOT_PARAMETER rootParameters[1] = {};

	ranges.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	rootParameters[0].InitAsConstantBufferView(0,0, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_ROOT_SIGNATURE_DESC chunkrootsigndesc;
	chunkrootsigndesc.Init(ARRAYSIZE(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	hr = D3D12SerializeRootSignature(&chunkrootsigndesc, D3D_ROOT_SIGNATURE_VERSION_1, &ChunkSignature, &ChunkSignatureErrorBlob);
	LOGS(hr, CHUNKRENDERLOG, "Chunk root signature serialized");
	hr = M_Device->CreateRootSignature(0, ChunkSignature->GetBufferPointer(), ChunkSignature->GetBufferSize(), __uuidof(ID3D12RootSignature), &M_ChunkRootSignature);
	LOGS(hr, CHUNKRENDERLOG, "Chunk root signature created");

	ReadChunkVertexShader();
	ReadChunkOpaqueShader();

	const UINT constantBufferSize = (sizeof(ChunkConstantBuffer) + 255) & ~255;

	auto cbh = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto cbb = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);
	hr = M_Device->CreateCommittedResource(&cbh,
		D3D12_HEAP_FLAG_NONE,
		&cbb,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&M_ChunkConstantBuffer));
	M_ChunkConstantBuffer->SetName(L"Chunk View Constant Buffer");

	LOGS(hr, CHUNKRENDERLOG, "Constant buffer created");

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = M_ChunkConstantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = constantBufferSize;
	M_Device->CreateConstantBufferView(&cbvDesc, M_CBDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout.NumElements = ARRAYSIZE(chunkVertexElementDesc);
	psoDesc.InputLayout.pInputElementDescs = chunkVertexElementDesc;
	psoDesc.pRootSignature = M_ChunkRootSignature.Get();
	psoDesc.VS.pShaderBytecode = ChunkVertexShader.data();
	psoDesc.VS.BytecodeLength = ChunkVertexShader.size() * sizeof(uint8_t);
	psoDesc.PS.pShaderBytecode = ChunkOpaquePixelShader.data();
	psoDesc.PS.BytecodeLength = ChunkOpaquePixelShader.size() * sizeof(uint8_t);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE, FALSE, 0, 1, 0, FALSE, FALSE, FALSE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	hr = M_Device->CreateGraphicsPipelineState(&psoDesc, __uuidof(ID3D12PipelineState), &M_ChunkPSO);
	M_ChunkPSO->SetName(L"Chunk Graphics Pipeline State");
	LOGF(hr, CHUNKRENDERLOG, "Chunk PSO creating error");
	LOGS(hr, CHUNKRENDERLOG, "Chunk PSO creating success");

}

void ChunkRendering::ReadChunkVertexShader()
{
	ChunkVertexShader = PlatformReadData(L"ChunkVertexShader.cso");
}

void ChunkRendering::ReadChunkOpaqueShader()
{
	ChunkOpaquePixelShader = PlatformReadData(L"ChunkPixelShader.cso");
}

ChunkRendering::~ChunkRendering()
{
}
