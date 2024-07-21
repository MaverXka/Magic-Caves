#pragma once

#include "pch.h"

class RenderingManager;
class Config;
class World;

#define ENGINELOG "Engine"

class Engine
{
public:

	Engine(HINSTANCE hInstance, int nCmdShow);
	~Engine();

	void LoadWorld(World* world);
	World* GetEngineWorld() { return EngineWorld; }

	void EngineUpdate();

	static Engine* GetEngine() { return MainEngineInstance; };

private:

	void CreateMainWindowClass();

	static Engine* MainEngineInstance;

	HINSTANCE HInstance;
	HWND EngineWindow;

	Config* EngineConfig;

	class World* EngineWorld;


	RenderingManager* EngineRenderer;

};

