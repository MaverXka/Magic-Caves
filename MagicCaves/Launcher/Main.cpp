#ifndef UNICODE
#define UNICODE
#endif 

#include "pch.h"
#include "Engine.h"
#include "../Misc/Config/Config.h"

void EnableSTDConsoleOutput()
{
    AllocConsole();

    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
}

Engine* MainEngine = nullptr;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
#ifdef _DEBUG
    EnableSTDConsoleOutput();
#endif

    MainEngine = new Engine(hInstance,nCmdShow);

    return 0;
}