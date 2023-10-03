#ifndef WINDOW_INFO_H
#define WINDOW_INFO_H

#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>
#include <wingdi.h>
#include <dwmapi.h>
#include <windef.h>
#include <vector>

class Window {

    public:
    HWND TopWindow;

    enum BUFFER_SIZE {

        WINDOW_CAPTION = 128,
        WINDOW_IMAGE = 512

    };

    struct EnumProcess {

        DWORD tid;
        DWORD pid;
        HWND zOrderTopWindow;
        HWND ReturnWindow;

    };

    struct WindowInfo {

        HWND window;
        HWND parent;
        HWND child;
        char caption[BUFFER_SIZE::WINDOW_CAPTION];
        char image[BUFFER_SIZE::WINDOW_IMAGE];
        unsigned long pid;

    };

    bool OpenWindow(const char* path, char cmd[], PROCESS_INFORMATION pi, WindowInfo* w, Window::EnumProcess* ep);

    Window(EnumProcess* ep);

};

#endif