#pragma once

#include "../ChunkyWorld/ChunkyWorld.h"

class Overworld : public ChunkyWorld
{
public:

	Overworld(class Engine* engine);

	virtual void BeginPlay() override;
};

