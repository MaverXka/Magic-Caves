
#include "Engine.h"
#include "Renderer/RenderingManager.h"
#include "../Misc/Config/Config.h"
#include "ImGUI/imgui_impl_win32.h"
#include "World/World.h"
#include "World/Overworld/Overworld.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void EngineDestroy();
void WinAPIPaintWindow(HWND hwnd);

const wchar_t WINDOW_CLASS_NAME[] = L"Magic Caves Window";

Engine* Engine::MainEngineInstance = nullptr;

Engine::Engine(HINSTANCE hInstance, int nCmdShow) : HInstance(hInstance)
{
    LOG(ENGINELOG, "Engine initialization begin");
    if (!MainEngineInstance)
    {
        MainEngineInstance = this;
    }
    EngineConfig = new Config("settings.ini");

    CreateMainWindowClass();
    ShowWindow(EngineWindow, nCmdShow);

    EngineRenderer = new RenderingManager(EngineWindow, EngineConfig, this);

    Overworld* World = new Overworld(this);
    LoadWorld(World);
    EngineWorld->BeginPlay();

    EngineRenderer->ReadyRender = true;

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return true;

    switch (uMsg)
    {
    case WM_DESTROY:
        EngineDestroy();
        return 0;

    case WM_PAINT:
    {
        WinAPIPaintWindow(hwnd);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Engine::CreateMainWindowClass()
{

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = HInstance;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    EngineWindow = CreateWindowEx(
        0,
        WINDOW_CLASS_NAME,
        L"Magic Caves",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, EngineConfig->windowSizeX, EngineConfig->windowSizeY,
        NULL,       // Parent window    
        NULL,       // Menu
        HInstance,  // Instance handle
        NULL        // Additional application data
    );
    if (EngineWindow == NULL) { return; }

}

void WinAPIPaintWindow(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
    EndPaint(hwnd, &ps);
}

void EngineDestroy()
{
    PostQuitMessage(0);
}

Engine::~Engine()
{
    delete EngineWorld;
    if (EngineRenderer != nullptr)
    {
        if (EngineRenderer->RenderThread.joinable())
        {
            EngineRenderer->RenderThread.join();
        }
    }
}

void Engine::LoadWorld(World* world)
{
    delete EngineWorld;
    LOG(ENGINELOG, "Loaded world " << world);
    EngineWorld = world;
}

void Engine::EngineUpdate()
{
    if (EngineWorld == nullptr) return;
    EngineWorld->Tick(1);
}
