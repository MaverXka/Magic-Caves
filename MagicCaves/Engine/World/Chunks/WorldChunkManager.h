#pragma once

#include "pch.h"

class WorldChunkManager
{

public:

	WorldChunkManager();

protected:

	std::vector<class Chunk*> WorldChunks;
};

