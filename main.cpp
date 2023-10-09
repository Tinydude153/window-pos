#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>
#include <wingdi.h>
#include <dwmapi.h>
#include <windef.h>
#include <vector>

#include "window_info.h"
#include "screen.h"

bool GetWindowRectNoInvisibleBorders(HWND hWnd, RECT* rect) {

    // Get the physical coordinates of the window (this is without the additional offsets)
    RECT dwmRect;
    HRESULT hresult = DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &dwmRect, sizeof(RECT));
    // Return false in such case. error handling was done only here because after this check, the rest of the code
    // use the API correctly so error is unlikely 
    if (hresult != S_OK)
        return false;

    // Get information from the monitor where the window located.
    // We need it for getting its RECT in logical coordinates (rcMonitor.*)
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEX monInfo;
    monInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(monitor, &monInfo);

    // Get additional information from this monitor. we need it for getting the physical
    // coordinates of its position (dmPosition.x and dmPosition.y) and its physical coordinates
    // of its size (dmPelsWidth)
    DEVMODE monDeviceConfig;
    monDeviceConfig.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(monInfo.szDevice, ENUM_CURRENT_SETTINGS, &monDeviceConfig);

    // Calculate the ratio between the logical size and the physical size of the monitor (part of math to handle DPI changes)
    auto scalingRatio = (monInfo.rcMonitor.right - monInfo.rcMonitor.left) / (double)monDeviceConfig.dmPelsWidth;

    // Calculate the final answer in logical coordinates
    rect->left = (dwmRect.left - monDeviceConfig.dmPosition.x) * scalingRatio + monInfo.rcMonitor.left;
    rect->right = (dwmRect.right - monDeviceConfig.dmPosition.x) * scalingRatio + monInfo.rcMonitor.left;
    rect->top = (dwmRect.top - monDeviceConfig.dmPosition.y) * scalingRatio + monInfo.rcMonitor.top;
    rect->bottom = (dwmRect.bottom - monDeviceConfig.dmPosition.y) * scalingRatio + monInfo.rcMonitor.top;

    return true; // success

}


struct sMonitors {

    std::vector<int> iMonitors;
    std::vector<HMONITOR> hMonitor;
    std::vector<HDC> mHdc;
    std::vector<RECT> mRect;

};

/*static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) {

    sMonitors* smonitors = (sMonitors*)pData;

    smonitors->hMonitor.push_back(hMon);
    smonitors->mHdc.push_back(hdc);
    smonitors->mRect.push_back(*lprcMonitor);
    smonitors->iMonitors.push_back(smonitors->mHdc.size());

    return TRUE;

}*/

int main() {

    STARTUPINFOA si;
    memset(&si, 0, sizeof(STARTUPINFOA));
    si.cb = sizeof(STARTUPINFOA);
    PROCESS_INFORMATION pi;

    const char* path = "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe";
    char cline[] = "\"-app\"www.twitch.tv\"\"";

    // Instantiate instance of EnumProcess struct and initialize memory for it
    Window::EnumProcess* ep;
    ep = (Window::EnumProcess*)malloc(sizeof(Window::EnumProcess));
    memset(ep, 0, sizeof(ep));

    Window Window(ep);

    // Instantiate instance of WindowInfo struct and initialize memory for it
    Window::WindowInfo* w;
    w = (Window::WindowInfo*)malloc(sizeof(Window::WindowInfo));
    memset(w, 0, sizeof(w));

    if (!Window.OpenWindow(path, cline, pi, w, ep)) {

        printf("\nCreateProcess failed: %d\n", GetLastError());

    }

    Sleep(500);

    SetProcessDPIAware();

    WINDOWINFO winfo;
    winfo.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(w->window, &winfo);

    RECT cArea;
    if (!SystemParametersInfoA(SPI_GETWORKAREA, 0, &cArea, 0)) {

        printf("\nSystemParametersInfoA failed: %d\n", GetLastError());

    }

    Screen Screen;
    Screen::ScreenInfo sinfo;
    //sinfo = (Screen::ScreenInfo*)malloc(sizeof(Screen::ScreenInfo));
    //memset(sinfo, 0, sizeof(sinfo));

    Screen.MainScreenWorkArea(w, &sinfo);
    Screen.GetScreenRectangles(&sinfo);

    printf(

        "\nwaLeft: %d\nwaRight: %d\nwaBottom: %d\n",
        sinfo.ScreenRect[1].left,
        sinfo.ScreenRect[1].right,
        sinfo.ScreenRect[1].bottom

    );

    printf(

        "\n--cLeft: %d\n--cTop: %d\n--cRight: %d\n--cBottom: %d\n", 
        sinfo.ScreenWorkArea.left, 
        sinfo.ScreenWorkArea.top, 
        sinfo.ScreenWorkArea.right, 
        sinfo.ScreenWorkArea.bottom

    );

    printf("\nTOP: %d\n", sinfo.ScreenRect[1].top);

    printf("\ncyWindowBorder: %d\n", winfo.cyWindowBorders);



    w->x = 0;
    w->y = 0;
    w->width = 1280;
    w->height = 720;

    Screen.ConvertCoordinates(w, &sinfo, 0);

    Window.SetWindowPosition(w);
    Window.SetWindowSize(w);
    Window.GetWindowSize(w);

    UpdateWindow(w->window);

    PostMessageA(w->window, WM_CLOSE, 0, 0);  
    DestroyWindow(w->window);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    /*sMonitors monitors;
    EnumDisplayMonitors(NULL, NULL, MonitorEnum, (LPARAM)&monitors);

    for (int i = 0; i < monitors.iMonitors.size(); i++) {

        printf("\nScreen ID: %d\n", i);
        printf(

            "\n--Left: %d\n--Top: %d\n--Right: %d\n--Bottom: %d\n", 
            monitors.mRect[i].left , 
            monitors.mRect[i].top, 
            monitors.mRect[i].left, 
            monitors.mRect[i].bottom

        );

        //std::cout << std::endl <<

    } */

    DISPLAY_DEVICEA dDevices;
    memset(&dDevices, 0, sizeof(DISPLAY_DEVICEA));
    dDevices.cb = sizeof(DISPLAY_DEVICEA);

    EnumDisplayDevices(NULL, 0, &dDevices, EDD_GET_DEVICE_INTERFACE_NAME);
    std::cout << std::endl << dDevices.DeviceString;

    DISPLAY_DEVICEA dDevices_;
    memset(&dDevices_, 0, sizeof(DISPLAY_DEVICEA));
    dDevices_.cb = sizeof(DISPLAY_DEVICEA);

    EnumDisplayDevices((const char*)dDevices.DeviceName, 0, &dDevices_, 0);
    std::cout << std::endl << dDevices_.DeviceName;

    DEVMODE devmode;
    devmode.dmSize = sizeof(DEVMODE);
    devmode.dmDriverExtra = 512;
    char* dName = dDevices.DeviceName;

    BITMAP monitor;
    memset(&monitor, 1, (sizeof(BITMAP)));
    HDC deviceDC = CreateDC((const char*)dDevices_.DeviceName, (const char*)dDevices_.DeviceName, NULL, NULL);
    HGDIOBJ bmp = GetCurrentObject(deviceDC, OBJ_BITMAP);
    GetObject(bmp, sizeof(BITMAP), &monitor);
    std::cout << std::endl << monitor.bmWidth;
    std::cout << std::endl << monitor.bmHeight;
    std::cout << std::endl << GetDeviceCaps(deviceDC, HORZRES) << std::endl << GetDeviceCaps(deviceDC, VERTRES);

    EnumDisplaySettingsA(dName, ENUM_CURRENT_SETTINGS, &devmode);
    std::cout << std::endl << devmode.dmPelsWidth;
    std::cout << std::endl << devmode.dmPelsHeight;
    std::cout << std::endl << dName;

}