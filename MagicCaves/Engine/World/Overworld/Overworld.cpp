#include "Overworld.h"

Overworld::Overworld(class Engine* engine) : ChunkyWorld(engine)
{
}

void Overworld::BeginPlay()
{
	Chunk* MyTestChunk = new Chunk();
	WorldChunks[0] = MyTestChunk;
	LOG("Test", WorldChunks[0]);
}