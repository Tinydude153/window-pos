#include "window_info.h"

BOOL CALLBACK enum_windows_proc(HWND m_hwnd, LPARAM lParam) {

    DWORD processId;
    DWORD threadId;
    static HWND tWindow = ((Window::EnumProcess*)lParam)->zOrderTopWindow;

    tWindow = GetWindow (
        tWindow, 
        GW_HWNDNEXT
    );

    threadId = GetWindowThreadProcessId(m_hwnd, &processId);

    if (processId == ((Window::EnumProcess*)lParam)->pid) {
        ((Window::EnumProcess*)lParam)->ReturnWindow = m_hwnd;
        std::cout << std::endl << "EQUAL";
        return FALSE;
    }

    return TRUE;
}

bool Window::OpenWindow(const char* path, char cmd[], PROCESS_INFORMATION pi, Window::WindowInfo* w, Window::EnumProcess* ep) {

    STARTUPINFOA si;
    memset(&si, 0, sizeof(STARTUPINFOA));

    if (!CreateProcessA(path, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {

        printf("\nWindow::OpenWindow: CreateProcess failed: %d\n", GetLastError());
        return false;

    }

    WaitForInputIdle(pi.hProcess, INFINITE);
    HWND hWindow;
    ep->tid = pi.dwThreadId;
    ep->pid = pi.dwProcessId;
    printf("\nep->pid: %d\n", ep->pid);
    w->pid = pi.dwProcessId;
    EnumWindows(enum_windows_proc, (LPARAM)ep);

    Sleep(500);

    w->window = ep->ReturnWindow;
    HWND pWindow = GetParent(w->window);
    if (pWindow != NULL) {

        w->parent = pWindow;

    }

    GetWindowTextA(w->window, w->caption, Window::BUFFER_SIZE::WINDOW_CAPTION);

    std::cout << std::endl << w->pid;
    std::cout << std::endl << w->caption;

    return true;

}

Window::Window(Window::EnumProcess* ep) {

    Window::TopWindow = GetTopWindow(GetDesktopWindow());
    ep->zOrderTopWindow = GetTopWindow(GetDesktopWindow());

}