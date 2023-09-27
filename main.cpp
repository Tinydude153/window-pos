#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>
#include <wingdi.h>
#include <dwmapi.h>
#include <windef.h>

struct ProcHwnd {
    DWORD proc_id;
    HWND  hwnd;
};

RECT itRect;
bool getWindowRect;

char tBuffer[64];
int tSize = 0;
HWND window = GetTopWindow(GetDesktopWindow());

BOOL CALLBACK enum_windows_proc(HWND m_hwnd, LPARAM lParam) {
    DWORD processId;

    window = GetWindow(window, GW_HWNDNEXT);

    /*if (IsWindowVisible(m_hwnd)) {

        do {

            tSize = GetWindowTextLengthA(m_hwnd);
            GetWindowTextA(m_hwnd, tBuffer, tSize + 1);
            std::cout << std::endl << tBuffer;

        } while (m_hwnd = GetWindow(m_hwnd, GW_HWNDNEXT));

    }*/

    GetWindowThreadProcessId(m_hwnd, &processId);
    if (processId == ((ProcHwnd*)lParam)->proc_id) {
        ((ProcHwnd*)lParam)->hwnd = m_hwnd;
        std::cout << std::endl << "EQUAL";
        return FALSE;
    }
    return TRUE;
}

int main() {

    STARTUPINFOA si;
    memset(&si, 0, sizeof(STARTUPINFOA));
    si.cb = sizeof(STARTUPINFOA);
    si.dwX = 0;
    si.dwY = 0;
    si.lpTitle = (char*)"Window Testing";
    si.dwXSize = 1000;
    si.dwYSize = 1000;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USEPOSITION | STARTF_USESIZE;
    PROCESS_INFORMATION pi;

    const char* path = "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe";
    char cline[] = "\"-app\"www.twitch.tv\"\"";

    if (!CreateProcessA(path, cline, NULL, 
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

    SetProcessDPIAware();
    Sleep(5000);
    // Windows deals with screen borders to account for resizing, so this is for offsetting that to get
    // accurate screen sizes
    RECT rect, frame;
    GetWindowRect(ph.hwnd, &rect);
    DwmGetWindowAttribute(ph.hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(RECT));

    RECT border;
    border.left = frame.left - rect.left;
    border.top = frame.top - rect.top;
    border.right = frame.right - rect.right;
    border.bottom = frame.bottom - rect.bottom;

    rect.left -= border.left;
    rect.top -= border.top;
    rect.right -= border.right;
    rect.bottom -= border.bottom;

    std::cout << std::endl << rect.left;
    std::cout << std::endl << rect.top;
    std::cout << std::endl << rect.right;
    std::cout << std::endl << rect.bottom;

    HDWP deferWindow = BeginDeferWindowPos(1);

    deferWindow = DeferWindowPos(
        deferWindow,
        ph.hwnd,
        NULL,
        0,
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

    UpdateWindow(ph.hwnd);

    /*if (!SetWindowPos(ph.hwnd, HWND_BOTTOM, 0, 0, 1000, 500, SWP_HIDEWINDOW)) {

        printf("\nSetWindowPos failed: %d\n", GetLastError());

    }

    Sleep(5000);

    UpdateWindow(ph.hwnd);

    if (!ShowWindow(ph.hwnd, SW_SHOW)) {

        printf("\nShowWindow failed: %d\n", GetLastError());

    }

    UpdateWindow(ph.hwnd);
    WaitForSingleObject(ph.hwnd, INFINITE);

    /*if (!MoveWindow(ph.hwnd, 0, 0, 800, 200, TRUE)) {

        printf("\nMoveWindow failed: %d\n", GetLastError());

    } */

    PostMessageA(ph.hwnd, WM_CLOSE, 0, 0);  
    DestroyWindow(ph.hwnd);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    /*SHELLEXECUTEINFOA shellexec;
    shellexec.cbSize = sizeof(SHELLEXECUTEINFOA);
    shellexec.fMask = SEE_MASK_NOCLOSEPROCESS;
    shellexec.lpVerb = (const char*)"open";
    shellexec.lpFile = (char*)"C:\\Users\\mneal\\OneDrive - Holzer Health System\\Desktop\\Scoreboard.lnk";
    shellexec.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExA(&shellexec)) {

        printf("\nShellExecute failed: %d\n", GetLastError());

    }

    Sleep(500);

    DWORD sePid = GetProcessId(shellexec.hProcess);
    std::cout << std::endl << sePid;
    EnumWindows(EnumWindowsProc, 0);

    HWND hwndWindow = FindWindow(NULL, "Holzer Scoreboard");

    if (!MoveWindow(windowH, 0, 0, 500, 500, TRUE)) {

        printf("\nMoveWindow failed: %d\n", GetLastError());

    } 

    CloseHandle(shellexec.hProcess); */

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