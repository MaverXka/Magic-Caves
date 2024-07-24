#pragma once

#include "pch.h"
#include "ChunkData.h"

class Chunk
{
public:

	Chunk();
	~Chunk();


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

	ChunkVertex triangleVertices[36] = {
    {-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f},
    {1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
    {-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
    {-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},

    // Передняя грань
    {-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f},

    // Левая грань
    {-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f},

    // Правая грань
    {1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f},

    // Нижняя грань
    {-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f},

    // Верхняя грань
    {-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f},
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f},
    {-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f}
	};

	ComPtr<ID3D12Resource> M_ChunkVertexBuffer;
	ComPtr<ID3D12Resource> M_ChunkIndexBuffer;
	ComPtr<ID3D12Resource2> M_ChunkConstantBuffer;
	D3D12_VERTEX_BUFFER_VIEW ChunkVertexBufferView;

	friend class RenderPass_Chunks;
	friend class ChunkLocalVoxelIterator;

	BlockID* ChunkBlocks;
	bool bIsChunkBlocksAllocated = false;

};

