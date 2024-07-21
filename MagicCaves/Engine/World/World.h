#pragma once

class Camera;
class WorldChunkManager;
class Engine;

class World
{
public:

	World();
	World(class Engine* engine);


	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void EndPlay();


protected:

	Engine* EnginePtr = nullptr;

	Camera* WorldCamera;

	WorldChunkManager* worldChunkManager;



};

