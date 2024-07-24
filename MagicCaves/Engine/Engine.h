#pragma once

#include "pch.h"

#include "Renderer/View/Camera.h"

class RenderingManager;
class Config;
class World;
class Camera;
class InputSystem;

#define ENGINELOG "Engine"
#define WINDOWLOG "Window"

class Engine
{
public:

	Engine(HINSTANCE hInstance, int nCmdShow);
	~Engine();

	void LoadWorld(World* world);
	World* GetEngineWorld() { return EngineWorld; }

	void EngineUpdate();

	static Engine* GetEngine() { return MainEngineInstance; };

	void GetWindowSize(int& X, int& Y);
	bool IsFullscreen() { return false; }

	void SetCursorLocked(bool Locked);

private:

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool IsCursorLocked;
	bool HasWindowFocus = false;

	int* WindowSizeX;
	int* WindowSizeY;

	bool bCanRender();

	void CreateMainWindowClass();

	static Engine* MainEngineInstance;

	InputSystem* InputSystemPtr;
	HINSTANCE HInstance;
	HWND EngineWindow;
	Config* EngineConfig;
	class World* EngineWorld;
	Camera* EngineCamera;
	RenderingManager* EngineRenderer;

};

