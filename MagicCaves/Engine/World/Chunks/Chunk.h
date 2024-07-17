#pragma once

#include "pch.h"
#include "ChunkData.h"

class Chunk
{
public:

	Chunk();


	/// <returns>Returns true if chunk blocks array are allocated in memory</returns>
	bool GetIsChunkBlocksAllocated() { return bIsChunkBlocksAllocated; };

	void GenerateMesh();

	void RenderThread_BuildCMDList();

	/// <summary>
	/// Functions converts X, Y, Z coordinates to single index.
	/// </summary>
	/// <param name="X">Width</param>
	/// <param name="Y">Length</param>
	/// <param name="Z">Height</param>
	/// <returns>Returns one dimensional index</returns>
	inline int GetBlockIndexAt(int X, int Y, int Z);

protected:

	ChunkVertex triangleVertices[3] = {
		ChunkVertex(0.0f, 1.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f),  // Вершина 1
		ChunkVertex(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f), // Вершина 2
		ChunkVertex(1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f)   // Вершина 3
	};

	ComPtr<ID3D12Resource> M_ChunkVertexBuffer;
	ComPtr<ID3D12Resource> M_ChunkIndexBuffer;
	D3D12_VERTEX_BUFFER_VIEW ChunkVertexBufferView;

	friend class ChunkIterator;

	BlockID* ChunkBlocks;
	bool bIsChunkBlocksAllocated = false;

};

