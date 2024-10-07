#pragma once
#include <SDL2/SDL.h>
#include <functional>
#include <map>
#include "utils.hpp"


namespace engine {
    using Event = SDL_Event;

    class InputManager final {
    public:
        using EventHandler = std::function<void(void *, SDL_Event)>;

        static void Initialize();
        static void Update();
        static inline bool ShouldQuit() { return InputManager::shouldQuit; }
        static void Finalize();

        static bool KeyDown(const SDL_Event &e, SDL_KeyCode key);
        static bool MouseDown(const SDL_Event &e, int button);
        static bool KeyUp(const SDL_Event &e, SDL_KeyCode key);
        static Vec2 GetMousePos(const SDL_Event &e);
        static std::vector<SDL_Event> GetEventCache();

        static void ClearHandlers();

        static bool QueryKey(SDL_Scancode key);
        // 1 - LEFT | 2 - MID | 4 - RIGHT
        static bool QueryMouse(int button);
        static Vec2 QueryMousePos();

        static void RegisterHandler(SDL_EventType type, void *listener, const EventHandler &handler);
    private:
        static bool shouldQuit;
        static SDL_Event currentEvent;
        static std::map<Uint32, std::pair<void *, EventHandler>> handlers;
        static std::vector<SDL_Event> eventLastFrame;
    };
}
