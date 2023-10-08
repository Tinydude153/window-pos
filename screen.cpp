#include "screen.h"
#include "window_info.h"

static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) {

    ((Screen::ScreenInfo*)pData)->ScreenHandle.push_back(hMon);
    ((Screen::ScreenInfo*)pData)->ScreenDC.push_back(hdc);
    ((Screen::ScreenInfo*)pData)->ScreenRect.push_back(*lprcMonitor);
    ((Screen::ScreenInfo*)pData)->ScreenIndex.push_back(((Screen::ScreenInfo*)pData)->ScreenDC.size());

    return TRUE;

}

bool Screen::MainScreenWorkArea(Window::WindowInfo* w, Screen::ScreenInfo* sinfo) {

    RECT swa = sinfo->ScreenWorkArea;
    if (!SystemParametersInfoA(SPI_GETWORKAREA, 0, &swa, 0)) {

        printf("\nScreen::MainScreenWorkArea(): SystemParametersInfoA failed: %d\n", GetLastError());
        return false;

    }

    /* // This may come in use at some point //
    //
    DISPLAY_DEVICEA d;
    DEVMODE dm;
    memset(&d, 0, sizeof(DISPLAY_DEVICEA));
    d.cb = sizeof(DISPLAY_DEVICEA);
    EnumDisplayDevices(NULL, 0, &d, EDD_GET_DEVICE_INTERFACE_NAME);
    dm.dmSize = sizeof(DEVMODE);
    dm.dmDriverExtra = 512;
    EnumDisplaySettingsA((char*)d.DeviceName, ENUM_CURRENT_SETTINGS, &dm); 
    //
    */

    WINDOWINFO winfo;
    winfo.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(w->window, &winfo);
    sinfo->xBorder = winfo.cxWindowBorders;
    sinfo->yBorder = winfo.cyWindowBorders;

    swa.left = swa.left - winfo.cxWindowBorders;
    swa.right = swa.right + (winfo.cxWindowBorders * 2);
    swa.bottom = swa.bottom + winfo.cyWindowBorders;
    sinfo->ScreenWorkArea = swa;

    return true;

}

bool Screen::GetScreenRectangles(Screen::ScreenInfo* sinfo) {
    
    if (!EnumDisplayMonitors(NULL, NULL, MonitorEnum, (LPARAM)sinfo)) {

        printf("\nScreen::GetScreenRectangles(): EnumDisplayMonitors failed: %d\n", GetLastError());
        return false;

    }

    sinfo->ScreenRect[0].left = sinfo->ScreenWorkArea.left;
    sinfo->ScreenRect[0].right = sinfo->ScreenWorkArea.right;
    sinfo->ScreenRect[0].bottom = sinfo->ScreenWorkArea.bottom;

    for (int i = 1; i < sinfo->ScreenIndex.size(); i++) {

        sinfo->ScreenRect[i].left = sinfo->ScreenRect[i].left - sinfo->xBorder;
        sinfo->ScreenRect[i].right = sinfo->ScreenRect[i].right - (sinfo->xBorder * 2);
        printf("\nLEFT: %d | RIGHT: %d\n", sinfo->ScreenRect[i].left, sinfo->ScreenRect[i].right);

    }

    return true;

}

bool Screen::ConvertCoordinates(Window::WindowInfo* w, Screen::ScreenInfo* sinfo, int index) {

    w->x = w->x + sinfo->ScreenRect[index].left;
    if (index > 0) {
        w->y = sinfo->ScreenRect[index].bottom - w->y - sinfo->ScreenRect[index].top;
    }
    printf("\nX: %d, Y: %d\n", w->x, w->y);

    return true;

}