#pragma once

#include "pch.h"

class RenderingManager;
class Config;


class Engine
{
public:

	Engine(HINSTANCE hInstance, int nCmdShow);
	~Engine();

private:

	void CreateMainWindowClass();

	HINSTANCE HInstance;
	HWND EngineWindow;

	Config* EngineConfig;

	class World* EngineWorld;


	RenderingManager* EngineRenderer;

};

