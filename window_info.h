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
        int width;  // width of window; size
        int height; // height of window; size 
        int x;      // x position of window; coordinates
        int y;      // y position of window; coordinates
        unsigned long pid;
        WindowInfo* WindowList;

    };

    bool OpenWindow(const char* path, char cmd[], PROCESS_INFORMATION pi, WindowInfo* w, Window::EnumProcess* ep);

    // Window position functions
        // Positions window; sets WindowInfo.x and WindowInfo.y to specified value or, if filled
        // prior, it uses those values to position the window.
    bool SetWindowPosition(WindowInfo* w, int x = 100000, int y = 100000);
        // Gets window position and sets WindowInfo.x and WindowInfo.y to the size received.
    bool GetWindowPosition(WindowInfo* w);
    
    // Window size functions
        // Sized window; sets WindowInfo.width and WindowInfo.height to specified value or, if filled
        // prior, it sizes the window.
    bool SetWindowSize(WindowInfo* w, int width = 100000, int height = 100000);
        // Gets window size and sets WindowInfo.width and WindowInfo.height to the size received.
    bool GetWindowSize(WindowInfo* w);
    // Sets window position and size with SetWindowSize() and GetWindowSize()
    bool SetWindowPositionSize(WindowInfo* w);

    void WindowList(WindowInfo* w);

    Window(EnumProcess* ep);

};

#endif