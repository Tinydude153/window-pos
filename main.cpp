#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>
#include <wingdi.h>
#include <dwmapi.h>
#include <windef.h>
#include <vector>

#include "window_info.h"

struct ProcHwnd {
    DWORD proc_id;
    HWND  hwnd;
};

RECT itRect;
bool getWindowRect;

char tBuffer[64];
int tSize = 0;
HWND window = GetTopWindow(GetDesktopWindow());

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

/*BOOL CALLBACK enum_windows_proc(HWND m_hwnd, LPARAM lParam) {
    DWORD processId;

    window = GetWindow(window, GW_HWNDNEXT);

    /*if (IsWindowVisible(m_hwnd)) {

        do {

            tSize = GetWindowTextLengthA(m_hwnd);
            GetWindowTextA(m_hwnd, tBuffer, tSize + 1);
            std::cout << std::endl << tBuffer;

        } while (m_hwnd = GetWindow(m_hwnd, GW_HWNDNEXT));

    }

    GetWindowThreadProcessId(m_hwnd, &processId);
    if (processId == ((ProcHwnd*)lParam)->proc_id) {
        ((ProcHwnd*)lParam)->hwnd = m_hwnd;
        std::cout << std::endl << "EQUAL";
        return FALSE;
    }
    return TRUE;
} */

typedef struct {

    char windowName[128];
    unsigned long pid;
    HWND windowHwnd;

} windowsProcess;

/*bool createProcess(const char* name, char cmd[], PROCESS_INFORMATION pi, windowsProcess* proc) {

    const int windowNameBufferSize = 128;
    char windowNameText[windowNameBufferSize];

    // useless required garbage; most programs don't even check this so it doesn't even work
    STARTUPINFOA si;
    memset(&si, 0, sizeof(STARTUPINFOA));

    if (!CreateProcessA(name, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {

        printf("\nCreateProcess failed: %d\n", GetLastError());
        return false;

    }

    WaitForInputIdle(pi.hProcess, INFINITE);
    HWND window;
    ProcHwnd ph { pi.dwProcessId, window };
    Sleep(5000);
    EnumWindows(enum_windows_proc, (LPARAM)&ph);

    Sleep(5000);

    HWND pWindow = GetParent(ph.hwnd);
    if (pWindow != NULL) {

        proc->windowHwnd = pWindow;

    }

    GetWindowTextA(proc->windowHwnd, proc->windowName, windowNameBufferSize);

    proc->pid = pi.dwProcessId;
    //proc.windowHwnd = ph.hwnd;

    std::cout << std::endl << proc->pid;
    std::cout << std::endl << proc->windowName;

    return true;

} */

struct sMonitors {

    std::vector<int> iMonitors;
    std::vector<HMONITOR> hMonitor;
    std::vector<HDC> mHdc;
    std::vector<RECT> mRect;

};

static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) {

    sMonitors* smonitors = (sMonitors*)pData;

    smonitors->hMonitor.push_back(hMon);
    smonitors->mHdc.push_back(hdc);
    smonitors->mRect.push_back(*lprcMonitor);
    smonitors->iMonitors.push_back(smonitors->mHdc.size());

    return TRUE;

}

int main() {

    STARTUPINFOA si;
    memset(&si, 0, sizeof(STARTUPINFOA));
    si.cb = sizeof(STARTUPINFOA);
    PROCESS_INFORMATION pi;

    const char* path = "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe";
    char cline[] = "\"-app\"www.twitch.tv\"\"";

    /*if (!CreateProcessA(path, cline, NULL, 
    NULL, FALSE, 0, NULL, NULL, &si, &pi)) {

        printf("\nCreateProcess failed: %d\n", GetLastError());

    };

    std::cout << std::endl << pi.dwProcessId;

    WaitForInputIdle(pi.hProcess, INFINITE);
    HWND windowH;
    ProcHwnd ph { pi.dwProcessId, windowH };
    Sleep(5000);
    EnumWindows(enum_windows_proc, (LPARAM)&ph);

    Sleep(500);

    HWND pWindow = GetParent(ph.hwnd);
    if (pWindow != NULL) {

        ph.hwnd = pWindow;

    }

    char captionBuffer[512];
    GetWindowTextA(ph.hwnd, captionBuffer, 128);
    std::cout << std::endl << captionBuffer; */

    /*windowsProcess* proc_struct;
    proc_struct = (windowsProcess*)malloc(sizeof(windowsProcess));
    memset(proc_struct, 0, sizeof(windowsProcess));

    if (!createProcess(path, cline, pi, proc_struct)) {

        printf("\nCreateProcess failed: %d\n", GetLastError());

    };*/

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

    HDWP deferWindow = BeginDeferWindowPos(1);

    deferWindow = DeferWindowPos(
        deferWindow,
        w->window,
        NULL,
        -7,
        0,
        2000,
        400,
        SWP_NOZORDER
    );

    if (deferWindow != NULL) {

        if (!EndDeferWindowPos(deferWindow)) {

            printf("\nEndDeferWindowPos failed: %d\n", GetLastError());

        }

    }

    Sleep(5000);

    UpdateWindow(w->window);

    PostMessageA(w->window, WM_CLOSE, 0, 0);  
    DestroyWindow(w->window);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    sMonitors monitors;
    EnumDisplayMonitors(NULL, NULL, MonitorEnum, (LPARAM)&monitors);

    for (int i = 0; i < monitors.iMonitors.size(); i++) {

        std::cout << std::endl << "Screen ID: " << i;
        std::cout << std::endl << "Left top corner: " << monitors.mRect[i].left << ", " << monitors.mRect[i].top;
        //std::cout << std::endl <<

    }

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