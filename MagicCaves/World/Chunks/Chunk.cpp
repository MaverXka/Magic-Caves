#include "Chunk.h"
#include <iostream>

Chunk::Chunk()
{
	ChunkBlocks = new BlockID[TotalChunkBlocksSize];
	bIsChunkBlocksAllocated = true;

	GenerateMesh();
}

void Chunk::GenerateMesh()
{
}

// Render thread
void Chunk::RenderThread_Render()
{
}

int Chunk::GetBlockIndexAt(int X, int Y, int Z)
{
	return Z* (CHUNK_WIDTH * CHUNK_WIDTH) + Y * CHUNK_WIDTH + X;
}
