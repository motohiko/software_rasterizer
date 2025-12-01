#include "PreIncludeWindows.h"
#include <Windows.h>
#include "PostIncludeWindows.h"
#include "Application.h" 

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    Application* application = new Application();

    if (!application->initialize(hInstance))
    {
        delete application;
        return 0;
    }

    application->showMainWindows(nShowCmd);

    application->messageLoop();

    delete application;
    return 0;
}
