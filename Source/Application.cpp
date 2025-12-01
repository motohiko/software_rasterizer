#include "Application.h" 
#include "MainWindow.h" 

Application::Application() :
    _mainWindow(nullptr)
{
}

Application::~Application()
{
    delete _mainWindow;
}

bool Application::initialize(HINSTANCE hInstance)
{
    _mainWindow = new MainWindow();

    bool created = _mainWindow->create(hInstance);
    if (!created)
    {
        return false;
    }

    return true;
}

void Application::showMainWindows(int nShowCmd)
{
    _mainWindow->show(nShowCmd);
}

void Application::messageLoop()
{
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}
