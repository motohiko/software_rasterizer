#pragma once

#include "PreIncludeWindows.h"
#include <Windows.h>
#include "PostIncludeWindows.h"
#include "MainWindow.h" 
#include <memory.h>

class Application
{

public:

    Application();
    ~Application();

    void initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

    bool createMainWindows();

    void messageLoop();

private:

    HINSTANCE _hInstance;
    int _nShowCmd;

    std::unique_ptr<MainWindow> _mainWindow;

};

extern Application* application;
