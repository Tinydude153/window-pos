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

    if (threadId == ((Window::EnumProcess*)lParam)->tid) {
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

    // Fill EnumProcess struct and the pid of WindowInfo struct
    ep->tid = pi.dwThreadId;
    ep->pid = pi.dwProcessId;
    w->pid = pi.dwProcessId;
    Sleep(500);

    // Find the window that is associated with the created process
    EnumWindows(enum_windows_proc, (LPARAM)ep);

    Sleep(500);

    // Set WindowInfo struct's window member to the found window
    w->window = ep->ReturnWindow;

    // Get parent and if it exists, set the parent member of WindowInfo to its window
    HWND pWindow = GetParent(w->window);
    if (pWindow != NULL) {

        w->parent = pWindow;

    }

    // Get window caption; appears to not be very useful if borderless
    GetWindowTextA(w->window, w->caption, Window::BUFFER_SIZE::WINDOW_CAPTION);

    // Get name of executable file for the specified process
    //   TODO: implement GetFileModuleName instead
    if (!GetProcessImageFileNameA(pi.hProcess, w->image, Window::BUFFER_SIZE::WINDOW_IMAGE)) {

        printf("\nWindow::OpenWindow: GetProcessImageFileNameA failed: %d\n", GetLastError());

    }

    std::cout << std::endl << w->image;
    std::cout << std::endl << w->pid;
    std::cout << std::endl << w->caption;

    return true;

}

void Window::WindowList(Window::WindowInfo* w) {

    // OpenWindow may be called successively, therefore it may be necessary to 
    // automatically populate new WindowInfo structs that will need enumerated
    w->WindowList = (Window::WindowInfo*)malloc(sizeof(Window::WindowInfo));
    w = w->WindowList;

}

bool Window::SetWindowPosition(Window::WindowInfo* w, int x, int y) {

    HDWP DeferWindow = BeginDeferWindowPos(1);

    DeferWindow = DeferWindowPos(
        DeferWindow,
        w->window,
        NULL,
        x,
        y,
        500,
        500,
        SWP_NOZORDER
    );

    if (!EndDeferWindowPos(DeferWindow)) {

        printf("\nWindow::SetWindowPosition: EndDeferWindowPos failed: \n", GetLastError());
        return false;

    }

    return true;

}

Window::Window(Window::EnumProcess* ep) {

    Window::TopWindow = GetTopWindow(GetDesktopWindow());
    ep->zOrderTopWindow = GetTopWindow(GetDesktopWindow());

}