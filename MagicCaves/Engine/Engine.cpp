
#include "Engine.h"
#include "Renderer/RenderingManager.h"
#include "../Misc/Config/Config.h"
#include "ImGUI/imgui_impl_win32.h"
#include "World/World.h"
#include "Input/InputSystem.h"
#include "World/Overworld/Overworld.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void EngineDestroy();
void WinAPIPaintWindow(HWND hwnd);

const wchar_t WINDOW_CLASS_NAME[] = L"Magic Caves Window";

Engine* Engine::MainEngineInstance = nullptr;

int* gWindowSizeX;
int* gWindowSizeY;
InputSystem* gInput;

RenderingManager* gEngineRenderer;

Engine::Engine(HINSTANCE hInstance, int nCmdShow) : HInstance(hInstance)
{
    LOG(ENGINELOG, "Engine initialization begin");
    if (!MainEngineInstance)
    {
        MainEngineInstance = this;
    }
    EngineConfig = new Config("settings.ini");

    InputSystemPtr = new InputSystem();
    gInput = InputSystemPtr;

    gWindowSizeX = new int(EngineConfig->windowSizeX);
    gWindowSizeY = new int(EngineConfig->windowSizeY);
    WindowSizeX = gWindowSizeX;
    WindowSizeY = gWindowSizeY;

    CreateMainWindowClass();
    ShowWindow(EngineWindow, nCmdShow);

    EngineRenderer = new RenderingManager(EngineWindow, EngineConfig, this);
    gEngineRenderer = EngineRenderer;

    Overworld* World = new Overworld(this);
    LoadWorld(World);
    EngineWorld->BeginPlay();
    EngineCamera = new Camera();
    Camera::SetMainCamera(EngineCamera);

    Camera* renderViewCamera = Camera::GetMainCamera();
    renderViewCamera->SetCameraPosition(-4, 0, 2);

    EngineRenderer->SetCanRender(true);

    MSG msg = { };
    while (true)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) {
                return;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        //while (ShowCursor(FALSE) >= 0);
        EngineUpdate();



    }

}



LRESULT CALLBACK Engine::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return true;

    switch (uMsg)
    {
    case WM_DESTROY:
        EngineDestroy();
        return 0;
    case WM_KEYDOWN:
    {
        if(gInput)
        gInput->AddWinKeyDown((KeyboardKeys)wParam);
        break;
    }
    case WM_KEYUP:
    {
        if(gInput)
        gInput->AddWinKeyUp((KeyboardKeys)wParam);
        break;
    }
    case WM_SETFOCUS:
    {
        LOG(WINDOWLOG, "focus set");
        Engine::GetEngine()->HasWindowFocus = true;
        break;
    }
    case WM_KILLFOCUS:
    {
        LOG(WINDOWLOG, "focus killed");
        Engine::GetEngine()->HasWindowFocus = false;
        break;
    }
    case WM_SIZE:
    {
        if (!gEngineRenderer) return 0;
        gEngineRenderer->SetCanRender(false);
        *gWindowSizeX = LOWORD(lParam);
        *gWindowSizeY = HIWORD(lParam);
        LOG(WINDOWLOG, "resized to x: " << *gWindowSizeX << " y: " << *gWindowSizeY);
        if(gEngineRenderer)
        gEngineRenderer->ResizeWindow();
        gEngineRenderer->SetCanRender(true);
    }
    case WM_PAINT:
    {
        WinAPIPaintWindow(hwnd);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool Engine::bCanRender()
{
    return true;
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

    if (IsCursorLocked && HasWindowFocus)
    {
      //  LOG("C", "cursor hidden");
    }
    else
    {
    }

    if (InputSystemPtr->IsKeyPressedDown(KeyboardKeys::KEY_F1))
    {
        SetCursorLocked(true);
        return;
    }

    if (InputSystemPtr->IsKeyPressedDown(KeyboardKeys::KEY_F2))
    {
        SetCursorLocked(false);
        return;
    }

    if (InputSystemPtr->IsKeyPressedDown(KeyboardKeys::KEY_W))
    {
        EngineCamera->OffsetCamera(0.000001f, 0, 0);
    }

    if (InputSystemPtr->IsKeyPressedDown(KeyboardKeys::KEY_S))
    {
        EngineCamera->OffsetCamera(-0.000001f, 0, 0);
    }

    if (InputSystemPtr->IsKeyPressedDown(KeyboardKeys::KEY_A))
    {
        EngineCamera->OffsetCamera(0, 0.000001f, 0);
    }

    if (InputSystemPtr->IsKeyPressedDown(KeyboardKeys::KEY_D))
    {
        EngineCamera->OffsetCamera(0, -0.000001f, 0);
    }

    if (InputSystemPtr->IsKeyPressedDown(KeyboardKeys::KEY_Q))
    {
        EngineCamera->OffsetCamera(0, 0, -0.000001f);
    }

    if (InputSystemPtr->IsKeyPressedDown(KeyboardKeys::KEY_Space))
    {
        EngineCamera->OffsetCamera(0, 0, 0.000001f);
    }
}

void Engine::GetWindowSize(int& X, int& Y)
{
    X = *WindowSizeX;
    Y = *WindowSizeY;
}

void Engine::SetCursorLocked(bool Locked)
{
    if (IsCursorLocked == Locked) return;
    IsCursorLocked = Locked;
    ShowCursor(!Locked);
    //LOG(ENGINELOG, "Cursor int: " << ShowCursor(Locked));
}
