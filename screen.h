#ifndef SCREEN_H
#define SCREEN_H

#include "window_info.h"

class Screen {

    public:
    struct ScreenInfo {

        int xBorder;
        int yBorder;
        std::vector<int> ScreenIndex;
        std::vector<HMONITOR> ScreenHandle;
        std::vector<HDC> ScreenDC;
        std::vector<RECT> ScreenRect;
        RECT ScreenWorkArea;
        
    };

    // Gets the primary screen (screen with taskbar) resolution; true resolution, 
    // adding borders and removing the taskbar.
    bool MainScreenWorkArea(Window::WindowInfo* w, ScreenInfo* sinfo);
    // Calls EnumDisplayMonitors and fills ScreenInfo with information.
    bool GetScreenRectangles(ScreenInfo* sinfo);

    bool ConvertCoordinates(Window::WindowInfo* w, ScreenInfo* sinfo, int index);

};

#endif