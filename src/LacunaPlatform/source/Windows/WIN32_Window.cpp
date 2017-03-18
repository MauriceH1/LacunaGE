#include "WIN32_Window.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace lcn::platform;
using namespace lcn::platform::specifics;

namespace lcn
{
    namespace platform
    {
        namespace specifics
        {
            MSG msg;

            LRESULT CALLBACK WindowProc(HWND a_HWnd, UINT a_Message, WPARAM a_WParam, LPARAM a_LParam)
            {
                switch(a_Message)
                {
                    case WM_ENTERSIZEMOVE:
                        break;
                    case WM_EXITSIZEMOVE:
                        RECT winRect;
                        GetWindowRect(a_HWnd, &winRect);
                        break;
                    case WM_NCDESTROY:
                        PostQuitMessage(0);
                        break;
                }

                // Handle Input

                return DefWindowProc(a_HWnd, a_Message, a_WParam, a_LParam);
            }

            struct PlatformHandles
            {
                HINSTANCE hInstance;
                HWND hWnd;
            };
        }; // namespace specifics
    }; // namespace platform
}; // namespace lcn

WIN32_Window::WIN32_Window(const LacunaWindowOptions& a_Options)
    : LacunaWindow(a_Options)
{
    m_PlatformHandles = new PlatformHandles;
    Initialize();
}

const int32_t WIN32_Window::HandleMessages()
{
    // Process any messages in the queue.
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if(msg.message == WM_QUIT)
        {
            return -1;
        }
    }
    return 0;
}

const int32_t WIN32_Window::Initialize()
{
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.cbClsExtra = 0;                           // No extra class data
    wc.cbWndExtra = sizeof(void*) + sizeof(int); // Make room for one pointer
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_Name.c_str();
    
    // Load user-provided icon if available
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), "ICON");
    if(!wc.hIcon)
    {
        // No user-provided icon found, load default icon
        wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    }
    
    if(!RegisterClass(&wc))
    {
        return -1;
    }

    DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    DWORD dwExStyle = WS_EX_APPWINDOW;
    
    RECT rect = {0, 0, m_Size.x, m_Size.y};
    AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);
    int32_t fullWidth = rect.right - rect.left;
    int32_t fullHeight = rect.bottom - rect.top;
    
    m_PlatformHandles->hWnd = CreateWindowEx(dwExStyle,
                                        m_Name.c_str(),
                                        m_Name.c_str(),
                                        dwStyle,
                                        CW_USEDEFAULT, CW_USEDEFAULT,
                                        fullWidth, fullHeight,
                                        NULL,
                                        NULL,
                                        GetModuleHandle(NULL),
                                        NULL);
    
    ShowWindow(m_PlatformHandles->hWnd, SW_SHOWNORMAL);
    
    return 0;
}