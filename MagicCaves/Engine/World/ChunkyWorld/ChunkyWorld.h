#pragma once

#include "../World.h"
#include "World/Chunks/Chunk.h"

class WorldChunkManager;

class ChunkyWorld : public World
{
public:

	ChunkyWorld(class Engine* engine);

protected:

	friend class RenderPass_Chunks;
	
	Chunk** WorldChunks;
	int ChunkWorldPoolSize = 0;

	WorldChunkManager* WorldChunkManagerPtr;

};

