#pragma once

const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 256;

constexpr int CHUNK_VOLUME_SIZE = (CHUNK_WIDTH * CHUNK_WIDTH) * CHUNK_HEIGHT;

/// <summary> The number of blocks will be limited from 0 to 255. Using char as block type id. </summary>
typedef unsigned char BlockID;

/// <summary> Returns block type size </summary>
constexpr int BlockTypeIDSize = sizeof(BlockID);

constexpr int TotalChunkBlocksSize = BlockTypeIDSize * CHUNK_VOLUME_SIZE;

/// <summary>
/// Chunk iterator created for easy way to iterate for all blocks in it.
/// </summary>
class ChunkLocalVoxelIterator
{

public:

	/// <summary>
	/// Default constructor, iterated from 0 to max chunk size. Function GetBlock will not work;
	/// </summary>
	ChunkLocalVoxelIterator() {}

	/// <summary>
	/// Use this constructor if you want to link iterator with specified chunk. You can use for example GetBlock function.
	/// </summary>
	/// <param name="IterateChunk"></param>
	ChunkLocalVoxelIterator(class Chunk* IterateChunk) : IteratedChunk(IterateChunk)
	{

	}

	bool operator==(const ChunkLocalVoxelIterator& other)
	{
		return (X == other.X) && (Y == other.Y) && (Z == other.Z);
	}

	ChunkLocalVoxelIterator& operator++()
	{
		IteratedIndex++;
		return *this;
	}

	operator bool() const
	{
		return IteratedIndex < CHUNK_VOLUME_SIZE;
	}

	int GetX() { return X; };
	int GetY() { return Y; };
	int GetZ() { return Z; };

	/// <returns>Returns one dimensional index.</returns>
	int GetIndex() { return IteratedIndex; };

	//BlockID GetBlock() { return IteratedChunk->ChunkBlocks[IteratedIndex]; }

private:

	class Chunk* IteratedChunk = nullptr;
	int IteratedIndex = 0;

	int X = 0;
	int Y = 0;
	int Z = 0;

};

struct ChunkVertex
{
	float X, Y, Z;
	float U, V;
	float R, G, B;
};

/// <summary>
/// Chunk specified constant buffer 
/// </summary>
struct ChunkConstantBuffer
{
	XMFLOAT4X4 ProjectionViewMatrix;
	int ChunkOffsetX, ChunkOffsetY;
	char padding[184];
};
static_assert((sizeof(ChunkConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");
