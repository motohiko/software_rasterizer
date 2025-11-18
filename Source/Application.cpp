#include "Application.h" 

Application* application;

Application::Application()
{
    application = this;
}

Application::~Application()
{
    application = nullptr;
}

void Application::initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    _hInstance = hInstance;
    _nShowCmd = nShowCmd;
}

bool Application::createMainWindows()
{
    _mainWindow = std::make_unique<MainWindow>();

    bool created = _mainWindow->create(_hInstance);
    if (!created)
    {
        return false;
    }

    _mainWindow->show(_nShowCmd);

    return true;
}

void Application::messageLoop()
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
