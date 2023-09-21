#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>

struct ProcHwnd {
    DWORD proc_id;
    HWND  hwnd;
};

char tBuffer[64];
int tSize = 0;
HWND window = GetTopWindow(GetDesktopWindow());

BOOL CALLBACK enum_windows_proc(HWND m_hwnd, LPARAM lParam) {
    DWORD processId;

    window = GetWindow(window, GW_HWNDNEXT);

    if (IsWindowVisible(m_hwnd)) {

        do {

            tSize = GetWindowTextLengthA(m_hwnd);
            GetWindowTextA(m_hwnd, tBuffer, tSize + 1);
            std::cout << std::endl << tBuffer;

        } while (m_hwnd = GetWindow(m_hwnd, GW_HWNDNEXT));

    }

    GetWindowThreadProcessId(m_hwnd, &processId);
    if (processId == ((ProcHwnd*)lParam)->proc_id) {
        ((ProcHwnd*)lParam)->hwnd = m_hwnd;
        return FALSE;
    }
    return TRUE;
}

int main() {

    STARTUPINFOA si { sizeof(STARTUPINFOA) };
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
    EnumWindows(enum_windows_proc, (LPARAM)&ph);

    if (!MoveWindow(ph.hwnd, 0, 0, 500, 500, TRUE)) {

        printf("\nMoveWindow failed: %d\n", GetLastError());

    } 

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
    dDevices.cb = sizeof(DISPLAY_DEVICEA);

    EnumDisplayDevices(NULL, 1, &dDevices, EDD_GET_DEVICE_INTERFACE_NAME);
    std::cout << std::endl << dDevices.DeviceString;

    DEVMODE devmode;
    devmode.dmSize = sizeof(DEVMODE);
    devmode.dmDriverExtra = 512;
    char* dName = dDevices.DeviceName;
    EnumDisplaySettingsA(dName, ENUM_CURRENT_SETTINGS, &devmode);
    std::cout << std::endl << devmode.dmPelsWidth;
    std::cout << std::endl << devmode.dmPelsHeight;
    std::cout << std::endl << dName;

}

