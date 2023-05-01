#include "natsu_runtime/function/render/window_system/window_system.h"

#include "natsu_gfx/pch.h"

namespace Natsu::Runtime {
    LRESULT CALLBACK static MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        // if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        //     return true;

        return WindowSystem::MsgProc(hwnd, msg, wParam, lParam);
    }

    LRESULT CALLBACK
    WindowSystem::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
            case WM_SIZE: {
                s_data.width  = LOWORD(lParam);
                s_data.height = HIWORD(lParam);
                return 0;
            }

            case WM_CLOSE: {
                g_runtime_global_context.m_window_system->SetClose();
                return 0;
            }

            case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    // ***********************************************************************************

    WindowSystem::WindowData WindowSystem::s_data;

    WindowSystem::~WindowSystem() {
        Shutdown();
    }

    void WindowSystem::Init(const WindowSystemInitInfo& init_info) {
        s_data.title  = init_info.title;
        s_data.width  = init_info.width;
        s_data.height = init_info.height;

        LOG_INFO("creating window {0} ({1}, {2})", init_info.title, init_info.width, init_info.height);

        WNDCLASSEXW wc = {
            sizeof(wc),
            CS_CLASSDC,
            MainWndProc,
            0L,
            0L,
            GetModuleHandle(NULL), NULL, NULL, (HBRUSH)GetStockObject(NULL_BRUSH), NULL, L"MainWnd", NULL};

        if (!::RegisterClassExW(&wc))
            LOG_CRITICAL("register class failed!!!");

        RECT rc;
        SetRect(&rc, 0, 0, s_data.width, s_data.height);
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

        auto window_flag = WS_POPUPWINDOW | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;
        if (init_info.is_fullscreen)
            window_flag |= WS_MAXIMIZE;

        m_window = CreateWindow(
            wc.lpszClassName,
            GFX::AnsiToWString(s_data.title).c_str(),
            window_flag,
            300, 200,
            s_data.width, s_data.height,
            NULL, NULL, wc.hInstance, NULL);

        ASSERT(m_window, "create window failed!!!");

        ShowWindow(m_window, (init_info.is_fullscreen) ? SW_SHOWMAXIMIZED : SW_SHOWDEFAULT);
        UpdateWindow(m_window);
    }
    void WindowSystem::Shutdown() {
        ::DestroyWindow(m_window);
        ::UnregisterClassW(L"MainWnd", GetModuleHandle(NULL));
    }

    void WindowSystem::OnUpdate() {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    void WindowSystem::SetWindowTitle(const std::string& title) {
        SetWindowText(m_window, GFX::AnsiToWString(title).c_str());
    }

} // namespace Natsu::Runtime