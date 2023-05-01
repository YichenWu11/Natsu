#pragma once

#include "natsu_runtime/pch.h"

namespace Natsu::Runtime {
    struct WindowSystemInitInfo {
        int         width{1280};
        int         height{720};
        std::string title{"Natsu"};
        bool        is_fullscreen{false};
    };

    class WindowSystem {
    public:
        WindowSystem() = default;
        ~WindowSystem();

        void Init(const WindowSystemInitInfo& init_info = {});
        void Shutdown();

        void OnUpdate();

        int  GetWidth() const { return s_data.width; }
        int  GetHeight() const { return s_data.height; }
        HWND GetNativeWindowHandle() { return m_window; }

        void SetWindowTitle(const std::string& title);

        bool ShouldClose() { return m_should_close; }
        void SetClose() { m_should_close = true; }

    private:
        HWND m_window;
        bool m_should_close{false};

        struct WindowData {
            int         width;
            int         height;
            std::string title;
        };

        static WindowData s_data;

    public:
        static LRESULT CALLBACK
        MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };
} // namespace Natsu::Runtime