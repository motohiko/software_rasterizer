#include "Application.h" 
#include "MainWindow.h" 

Application* application;

Application::Application() :
    _hInstance(NULL),
    _mainWindow(nullptr)
{
    application = this;
}

Application::~Application()
{
    delete _mainWindow;
    application = nullptr;
}

void Application::initialize(HINSTANCE hInstance)
{
    _hInstance = hInstance;
}

bool Application::createMainWindows(int nShowCmd)
{
    _mainWindow = new MainWindow();

    bool created = _mainWindow->create(_hInstance);
    if (!created)
    {
        return false;
    }

    _mainWindow->show(nShowCmd);

    return true;
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
