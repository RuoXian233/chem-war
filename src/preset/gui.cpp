#include "gui.h"

using namespace engine;

preset::GUIState preset::io;
preset::GUIStyle preset::style;

preset::GUIState &engine::preset::GetIO() {
    return io;
}

static Logger logger("WindowSystem");


void engine::preset::BeginGUIContext(int argc, char **argv, int w, int h, const std::string &title) {
    engine::Renderer::Initialize();
    engine::InputManager::Initialize();
    engine::ResourceManager::Initialize(argc, argv);
    logger.SetDisplayLevel(Logger::Level::Debug);
    // ParticleManager::Initialize();

    engine::Renderer::CreateWindow(w, h, title.c_str());

    engine::InputManager::RegisterHandler(SDL_MOUSEMOTION, nullptr, [](void *, const SDL_Event &e) {
        auto &io = GetIO();
        io.mouse.pos.x = e.motion.x;
        io.mouse.pos.y = e.motion.y;
        io.mouse.relPos.x = e.motion.xrel;
        io.mouse.relPos.y = e.motion.yrel;
        return false;
    });

    engine::InputManager::RegisterHandler(SDL_MOUSEBUTTONDOWN, nullptr, [](void *, const SDL_Event &e) {
        auto &io = GetIO();
        if (e.button.button == SDL_BUTTON_LEFT) {
            io.mouse.lmb = true;
        }
        if (e.button.button == SDL_BUTTON_MIDDLE) {
            io.mouse.mmb = true;
        }
        if (e.button.button == SDL_BUTTON_RIGHT) {
            io.mouse.rmb = true;
        }
        return false;
    });

    engine::InputManager::RegisterHandler(SDL_MOUSEBUTTONUP, nullptr, [](void *, const SDL_Event &e) {
        auto &io = GetIO();
        if (e.button.button == SDL_BUTTON_LEFT) {
            io.mouse.lmb = false;
        }
        if (e.button.button == SDL_BUTTON_MIDDLE) {
            io.mouse.mmb = false;
        }
        if (e.button.button == SDL_BUTTON_RIGHT) {
            io.mouse.rmb = false;
        }
        return false;
    });

    Renderer::DebugAddHUD("io.windowLayer", [] { return ToString(GetIO().window.windowLayer); });
    Renderer::DebugAddHUD("io.currentWindow", [] { return std::format("{}", GetIO().window.currentWindowID); });
}

void engine::preset::Begin(const Vec2 &pos, const Vec2 &size, const std::string &title, WindowFlags flags) {
    auto &io = GetIO();
    if (io.window.windowStack.size() < ++io.window.currentWindowID) {
        GUIWindow *window = new GUIWindow;
        window->flags = flags;
        window->pos = pos;
        window->shown = true;
        window->size = size;
        window->widgets = {};
        window->title = title;
    
        io.window.windowStack.insert(std::make_pair(io.window.currentWindowID, window));
        io.window.windowLayer.push_back(io.window.currentWindowID);
    }
}

void engine::preset::End() {
    auto &io = GetIO();
    if (io.window.currentWindowID > 0) {
        --io.window.currentWindowID;
    }
}

void engine::preset::EndGUIContext() {
    // world.Shutdown();
    // engine::ParticleManager::Finalize();
    engine::ResourceManager::Finalize();
    engine::InputManager::Finalize();
    engine::Renderer::Finalize();
}

void engine::preset::GUIUpdate() {
    auto &io = GetIO();
    static Vec2 lastFrameMousePos;
    static bool lastFrameLMB;

    for (auto window : io.window.windowLayer) {
        auto w = io.window.windowStack[window];
        if (io.mouse.lmb && !lastFrameLMB && PointInRect(io.mouse.pos, w->pos, w->size)) {
            w->focused = true;
            break;
        } else if (io.mouse.lmb && !lastFrameLMB && !PointInRect(io.mouse.pos, w->pos, w->size)) {
            w->focused = false;
        }
    }
    for (auto [id, window] : io.window.windowStack) {
        if (window->focused) {
            io.window.focusedWindowID = id;
            auto it = std::find(io.window.windowLayer.begin(), io.window.windowLayer.end(), id);
            std::swap(io.window.windowLayer[it - io.window.windowLayer.begin()], io.window.windowLayer[0]);
            break;
        } else {
            io.window.focusedWindowID = 0; 
        }
    }

    if (io.window.focusedWindowID) {
        auto window = io.window.windowStack[io.window.focusedWindowID];

        if (window->dragging && io.mouse.pos != lastFrameMousePos) {
            window->pos += io.mouse.relPos;
        }
        if (window->scaling && io.mouse.pos != lastFrameMousePos && !window->dragging) {
            auto newSize = window->size + io.mouse.relPos;
            if (style.minuiumWindowSize.x == -1 || style.minuiumWindowSize.y == -1) {
                if (newSize.y > style.windowTitleBarHeight + 1) {
                    window->size += io.mouse.relPos;
                }
            } else {
                if (newSize.y > style.minuiumWindowSize.y + 1) {
                    window->size.y += io.mouse.relPos.y;
                }
                if (newSize.x > style.minuiumWindowSize.x + 1) {
                    window->size.x += io.mouse.relPos.x;
                }
            }
        }
    
        if (io.mouse.lmb && PointInRect(io.mouse.pos, window->pos, Vec2(window->size.x, style.windowTitleBarHeight))) {
            window->dragging = true;
        } 
        if (window->dragging && !io.mouse.lmb) {
            window->dragging = false;
        }
        if (io.mouse.lmb && PointInRect(io.mouse.pos, window->pos + window->size - Vec2(8, 8), window->pos + window->size)) {
            window->scaling = true;
        }
        if (window->scaling && !io.mouse.lmb) {
            window->scaling = false;
        }
    }

    lastFrameMousePos = io.mouse.pos;
    lastFrameLMB = io.mouse.lmb;
}

void engine::preset::GUIRender() {
    auto &io = GetIO();
    // auto window = io.window.windowStack[io.window.currentWindowID];
    // RenderWindowFrame(window);
    auto renderLayer = io.window.windowLayer;
    std::reverse(renderLayer.begin(), renderLayer.end());
    for (auto window : renderLayer) {
        RenderWindowFrame(io.window.windowStack[window]);
    }
}

void engine::preset::RenderWindowFrame(GUIWindow *window) {
    Renderer::SetDrawColor(style.windowBackgroundColor);
    Renderer::FillRoundRect(window->pos, window->size, style.windowCornerRadius);
    if (window->focused) {
        Renderer::SetDrawColor(style.windowTitleBackgroundColor);
    } else {
        Renderer::SetDrawColor(style.windowUnfocusedTitleBackgroundColor);
    }
    Renderer::FillRoundRect(window->pos, Vec2(window->size.x, style.windowTitleBarHeight), style.windowCornerRadius);
    Renderer::FillRect(window->pos + Vec2(0, style.windowTitleBarHeight / 2), Vec2(window->size.x, style.windowTitleBarHeight / 2 + 1));
    auto orignalFontsize = Renderer::GetGlobalFontsize();
    Renderer::ChangeFontSize(style.windowTitleFontsize);
    auto t = Renderer::Text(window->title, style.windowTitleForegroundColor);
    Renderer::RenderTexture(t, window->pos + Vec2(style.windowTitleTextHorizonalMargin, style.windowTitleTextVerticalMargin));
    Renderer::ClearDrawColor();
    Renderer::DeleteRenderContext(t);
    Renderer::ChangeFontSize(orignalFontsize);
}

void engine::preset::SetGUIProc(std::function<void(float dt)> f) {
    while (!InputManager::ShouldQuit()) {
        auto dt = Renderer::GetDeltatime();
        Renderer::Clear();
        f(dt);
        GUIUpdate();
        GUIRender();
        InputManager::Update();
        Renderer::Update();
    }    
}
