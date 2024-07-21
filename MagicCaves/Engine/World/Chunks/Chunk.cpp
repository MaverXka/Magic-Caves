#include "Chunk.h"
#include <iostream>
#include "Renderer/RenderingManager.h"
#include "Renderer/RenderProgram.h"

Chunk::Chunk()
{
	ChunkBlocks = new BlockID[TotalChunkBlocksSize];
	bIsChunkBlocksAllocated = true;

	GenerateMesh();


	auto Device = RenderProgram::Get()->M_Device;
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(ChunkVertex)*ARRAYSIZE(triangleVertices));
	Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&M_ChunkVertexBuffer));

	void* data;
	M_ChunkVertexBuffer->Map(0, nullptr, &data);
	memcpy(data, triangleVertices, sizeof(ChunkVertex) * ARRAYSIZE(triangleVertices));
	M_ChunkVertexBuffer->Unmap(0, nullptr);

	ChunkVertexBufferView.BufferLocation = M_ChunkVertexBuffer->GetGPUVirtualAddress();
	ChunkVertexBufferView.StrideInBytes = sizeof(ChunkVertex);
	ChunkVertexBufferView.SizeInBytes = sizeof(ChunkVertex) * ARRAYSIZE(triangleVertices);

}

Chunk::~Chunk()
{
	M_ChunkVertexBuffer.Reset();
	M_ChunkIndexBuffer.Reset();
}

void Chunk::GenerateMesh()
{
}

// Render thread
void Chunk::RenderThread_BuildCMDList()
{
}

int Chunk::GetBlockIndexAt(int X, int Y, int Z)
{
	return Z* (CHUNK_WIDTH * CHUNK_WIDTH) + Y * CHUNK_WIDTH + X;
}
