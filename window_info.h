#ifndef WINDOW_INFO_H
#define WINDOW_INFO_H

#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>
#include <wingdi.h>
#include <dwmapi.h>
#include <windef.h>
#include <psapi.h>
#include <vector>

class Window {

    public:
    HWND TopWindow;

    enum BUFFER_SIZE {

        WINDOW_CAPTION = 128,
        WINDOW_IMAGE = 256

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
        int width;
        int height;
        WindowInfo* WindowList;

    };

    bool OpenWindow(const char* path, char cmd[], PROCESS_INFORMATION pi, WindowInfo* w, Window::EnumProcess* ep);

    void WindowList(WindowInfo* w);

    bool SetWindowPosition(WindowInfo* w, int x, int y);

    Window(EnumProcess* ep);

};

#endif