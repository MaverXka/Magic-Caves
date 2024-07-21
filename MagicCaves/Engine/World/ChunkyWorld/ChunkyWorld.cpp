#include "ChunkyWorld.h"
#include "World/Chunks/WorldChunkManager.h"


ChunkyWorld::ChunkyWorld(class Engine* engine)
{
	World::World(engine);
	WorldChunks = new Chunk * [5];
	ChunkWorldPoolSize = 1;
	WorldChunkManagerPtr = new WorldChunkManager();
}
