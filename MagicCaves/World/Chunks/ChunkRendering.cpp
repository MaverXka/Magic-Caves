#include "ChunkRendering.h"
#include "../Renderer/RenderingManager.h"
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

	CD3DX12_ROOT_SIGNATURE_DESC chunkrootsigndesc;
	chunkrootsigndesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	hr = D3D12SerializeRootSignature(&chunkrootsigndesc, D3D_ROOT_SIGNATURE_VERSION_1, &ChunkSignature, &ChunkSignatureErrorBlob);
	LOGS(hr, CHUNKRENDERLOG, "Chunk root signature serialized");
	hr = M_Device->CreateRootSignature(0, ChunkSignature->GetBufferPointer(), ChunkSignature->GetBufferSize(), __uuidof(ID3D12RootSignature), &M_ChunkRootSignature);
	LOGS(hr, CHUNKRENDERLOG, "Chunk root signature created");

	ReadChunkVertexShader();
	ReadChunkOpaqueShader();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout.NumElements = ARRAYSIZE(chunkVertexElementDesc);
	psoDesc.InputLayout.pInputElementDescs = chunkVertexElementDesc;
	psoDesc.pRootSignature = M_ChunkRootSignature.Get();
	psoDesc.VS.pShaderBytecode = ChunkVertexShader.data();
	psoDesc.VS.BytecodeLength = ChunkVertexShader.size() * sizeof(uint8_t);
	psoDesc.PS.pShaderBytecode = ChunkOpaquePixelShader.data();
	psoDesc.PS.BytecodeLength = ChunkOpaquePixelShader.size() * sizeof(uint8_t);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT());
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
