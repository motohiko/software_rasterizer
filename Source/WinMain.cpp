#include "PreIncludeWindows.h"
#include <Windows.h>
#include "PostIncludeWindows.h"
#include "Application.h" 

static Application s_application;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    application->initialize(hInstance);

    if (!application->createMainWindows(nShowCmd))
    {
        return 0;
    }

    application->messageLoop();

    return 0;
}
