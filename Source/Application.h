#pragma once

#include "PreIncludeWindows.h"
#include <Windows.h>
#include "PostIncludeWindows.h"

class MainWindow;

class Application
{

public:

    Application();
    ~Application();

    bool initialize(HINSTANCE hInstance);

    void showMainWindows(int nShowCmd);

    void messageLoop();

private:

    MainWindow* _mainWindow;

};

