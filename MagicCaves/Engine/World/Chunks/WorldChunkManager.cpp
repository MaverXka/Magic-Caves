#include "WorldChunkManager.h"
#include "Chunk.h"
#include <vector>

WorldChunkManager::WorldChunkManager()
{
	WorldChunks.push_back(new Chunk());
}
