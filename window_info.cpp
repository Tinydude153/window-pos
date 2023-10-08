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

    // Get position of window
    Window::GetWindowPosition(w);

    std::cout << std::endl << w->image;
    std::cout << std::endl << w->pid;
    std::cout << std::endl << w->caption;

    return true;

}

bool Window::GetWindowSize(Window::WindowInfo* w) {

    RECT r;

    if (!GetWindowRect(w->window, &r)) {

        printf("\nWindow::GetWindowSize(): GetWindowRect failed: %d\n", GetLastError());
        return false;

    }

    w->width = r.right - r.left;
    w->height = r.bottom - r.top;
    printf("\nwidth: %d\n", w->width);
    printf("\nheight: %d\n", w->height);

    return true;

}

// Either WindowInfo->x and WindowInfo->y needs to be set before calling this function,
// or SetWindowPosition() must be called before this function is called.
bool Window::SetWindowSize(Window::WindowInfo* w, int width, int height) {

    if (width == 100000) {
        width = w->width; 
    }
    if (height == 100000) {
        height = w->height;
    }
    printf("\nw %d, h %d\n", width, height);

    if(!w->window) {
        printf("\nWindow::SetWindowSize(): Window::WindowInfo*->window is NULL\n");
        return false;
    }

    // Set the size of the window
    HDWP deferWindow = BeginDeferWindowPos(1);
    deferWindow = DeferWindowPos(
        deferWindow,
        w->window,
        NULL,
        w->x,
        w->y,
        width,
        height,
        SWP_NOZORDER
    );

    if (deferWindow != NULL) {

        if (!EndDeferWindowPos(deferWindow)) {

            printf("\nEndDeferWindowPos failed: %d\n", GetLastError());
            return false;

        }

        return true;

    } else {

        printf("\nWindow::SetWindowSize: BeginDeferWindowPos is NULL\n");
        return false;

    }

    return true;

}

bool Window::GetWindowPosition(Window::WindowInfo* w) {

    WINDOWINFO wi;
    wi.cbSize = sizeof(WINDOWINFO);

    if (!GetWindowInfo(w->window, &wi)) {

        printf("\nWindow::GetWindowPosition: GetWindowInfo failed: %d\n", GetLastError());
        return false;

    }

    w->x = wi.rcWindow.left;
    w->y = wi.rcWindow.top;

    return true;

}

bool Window::SetWindowPosition(Window::WindowInfo* w, int x, int y) {

    // Check whether x and y were specified or if w->x, w->y are to be used 
    //  // 100000 was chosen just because it is unlikely that a virtual screen of size
    //  // 100000 will exist, so this is easiest to do
    if (x == 100000) {
        x = w->x;
    }
    if (y == 100000) {
        y = w->y;
    }

    // Check if window handle is NULL
    if (!w->window) {
        printf("\nWindow::SetWindowPosition(): Window::WindowInfo*->window is NULL\n");
        return false;
    }

    // Set the position of the window
    HDWP deferWindow = BeginDeferWindowPos(1);
    deferWindow = DeferWindowPos(
        deferWindow,
        w->window,
        NULL,
        x,
        y,
        480, // needs value, so this size is fine
        360,
        SWP_NOZORDER
    );

    if (deferWindow != NULL) {

        if (!EndDeferWindowPos(deferWindow)) {

            printf("\nEndDeferWindowPos failed: %d\n", GetLastError());
            return false;

        }

        return true;

    } else {

        printf("\nWindow::SetWindowPosition: BeginDeferWindowPos is NULL\n");
        return false;

    }

    return true;

}

bool Window::SetWindowPositionSize(Window::WindowInfo* w) {

    if (!SetWindowSize(w)) {

        printf("\nWindow::SetWindowPositionSize: Window::SetWindowSize failed: %d\n", GetLastError());
        return false;

    }
    if (!SetWindowPosition(w)) {

        printf("\nWindow::SetWindowPositionSize: Window::SetWindowPosition failed: %d\n", GetLastError());
        return false;

    }

    return true;

}

void Window::WindowList(Window::WindowInfo* w) {

    // OpenWindow may be called successively, therefore it may be necessary to 
    // automatically populate new WindowInfo structs that will need enumerated
    w->WindowList = (Window::WindowInfo*)malloc(sizeof(Window::WindowInfo));
    w = w->WindowList;

}

Window::Window(Window::EnumProcess* ep) {

    Window::TopWindow = GetTopWindow(GetDesktopWindow());
    ep->zOrderTopWindow = GetTopWindow(GetDesktopWindow());

}