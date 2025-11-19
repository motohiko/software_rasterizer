#pragma once

#include "PreIncludeWindows.h"
#include <Windows.h>
#include "PostIncludeWindows.h"
#include "MainWindow.h" 

class Application
{

public:

    Application();
    ~Application();

    void initialize(HINSTANCE hInstance);

    bool createMainWindows(int nShowCmd);

    void messageLoop();

private:

    HINSTANCE _hInstance;

    MainWindow* _mainWindow;

};

extern Application* application;
