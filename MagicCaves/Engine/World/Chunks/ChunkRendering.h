#pragma once

#include "pch.h"

#define CHUNKRENDERLOG "Chunk Renderer"

class RenderingManager;

class ChunkRendering
{
public:

	ChunkRendering(RenderingManager* renderer);
	~ChunkRendering();

private:

	friend class RenderPass_Chunks;

	void ReadChunkVertexShader();
	void ReadChunkOpaqueShader();


	//D3D12
	ComPtr<ID3DBlob> ChunkSignature;
	ComPtr<ID3DBlob> ChunkSignatureErrorBlob;
	ComPtr<ID3D12RootSignature> M_ChunkRootSignature;
	ComPtr<ID3D12Device10> M_Device;
	ComPtr<ID3D12PipelineState> M_ChunkPSO;
	std::vector<uint8_t> ChunkVertexShader;

	std::vector<uint8_t> ChunkOpaquePixelShader;

	//~	D3D12 End

	RenderingManager* MainRenderer;



	//Debugging
	HRESULT hr;

};

