#pragma once
#include "ChunkData.h"

class Chunk
{
public:

	Chunk();


	/// <returns>Returns true if chunk blocks array are allocated in memory</returns>
	bool GetIsChunkBlocksAllocated() { return bIsChunkBlocksAllocated; };

	void GenerateMesh();

	/// <summary>
	/// Called when need to be render (Render Thread)
	/// </summary>
	void RenderThread_Render();

	/// <summary>
	/// Functions converts X, Y, Z coordinates to single index.
	/// </summary>
	/// <param name="X">Width</param>
	/// <param name="Y">Length</param>
	/// <param name="Z">Height</param>
	/// <returns>Returns one dimensional index</returns>
	inline int GetBlockIndexAt(int X, int Y, int Z);

protected:

	friend class ChunkIterator;

	BlockID* ChunkBlocks;
	bool bIsChunkBlocksAllocated = false;

};

