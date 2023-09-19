#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <processthreadsapi.h>

int main() {

    /*STARTUPINFOA si { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi;

    const char* path = "C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe";
    //char cline[] = "-app=https://stackoverflow.com/questions/67038352/createprocess-launching-edge-browser";

    if (!CreateProcessA(path, cline, NULL, 
    NULL, FALSE, 0, NULL, NULL, &si, &pi)) {

        printf("\nCreateProcess failed: %d\n", GetLastError());

    };

    std::cout << std::endl << pi.dwProcessId;

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);*/

    SHELLEXECUTEINFOA shellexec;
    shellexec.cbSize = sizeof(SHELLEXECUTEINFOA);
    shellexec.fMask = 0x00000000;
    shellexec.lpVerb = (const char*)"open";
    shellexec.lpFile = (char*)"C:\\Users\\mneal\\OneDrive - Holzer Health System\\Desktop\\Scoreboard.lnk";
    shellexec.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExA(&shellexec)) {

        printf("\nShellExecute failed: %d\n", GetLastError());

    }

    CloseHandle(shellexec.hProcess);

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

