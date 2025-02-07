#pragma once
#include "../lib/render.h"
#include "../lib/input.h"
#include "../lib/resource.h"
#include "../lib/object.h"
#include <functional>

namespace engine::preset {
    void BeginGUIContext(int argc, char **argv, int w, int h, const std::string &title);
    void EndGUIContext();
    void SetGUIProc(std::function<void(float dt)> f);

    enum class ButtonInterationType {
        Pressed = 0, Hovered, OnLeft, OnRelease, Click, Hover
    };

    struct MouseState {
        bool lmb, mmb, rmb;
        Vec2 pos;
        Vec2 relPos;
        Direction scrollDirection;
        float delta;
    };

    using WindowFlags = int;
    using WindowID = int;

    struct WindowState {
        int currentWindowID = 0;
        int focusedWindowID = 0;
        std::map<WindowID, GUIWindow *> windowStack;
        std::vector<WindowID> windowLayer;
    };
  
    struct GUIState {
        MouseState mouse;
        WindowState window;
    }; 

    struct GUIStyle {
        int windowCornerRadius = 16;
        int windowTitleFontSlot = -1;
        int windowTitleFontsize = 16;
        int windowTitleBarHeight = 24;
        int windowTitleTextVerticalMargin = 4;
        int windowTitleTextHorizonalMargin = 8;
        Color windowTitleForegroundColor = Colors::White;
        Color windowTitleBackgroundColor = { 54, 66, 95, 255 };
        Color windowUnfocusedTitleBackgroundColor = { 32, 32, 64, 255 };
        Color windowBackgroundColor = { 3, 22, 52, 255 };
        Vec2 minuiumWindowSize = { -1, -1 };
    };

    extern GUIState io;
    extern GUIStyle style;

    void Begin(const Vec2 &pos, const Vec2 &size, const std::string &title, WindowFlags flags);
    GUIState &GetIO();
    void End();
    void GUIUpdate();
    void GUIRender();
    void RenderWindowFrame(GUIWindow *window);
}
