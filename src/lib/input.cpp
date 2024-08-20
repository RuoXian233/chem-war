#include "input.h"

using namespace chem_war;

bool InputManager::shouldQuit;
SDL_Event InputManager::currentEvent;
std::map<Uint32, InputManager::EventHandler> InputManager::handlers;


void InputManager::Initialize() {
    InputManager::shouldQuit = false;
    InputManager::handlers = std::map<Uint32, InputManager::EventHandler>();

    srand((unsigned) time(nullptr));
}

void InputManager::Update() {
    while (SDL_PollEvent(&InputManager::currentEvent)) {
        if (InputManager::currentEvent.type == SDL_QUIT) {
            InputManager::shouldQuit = true;
        }

        if (utils_MapHasKey(InputManager::handlers, InputManager::currentEvent.type)) {
            InputManager::handlers.at(InputManager::currentEvent.type)(InputManager::currentEvent);
        }
    }
}

void InputManager::Finalize() {}

Vec2 InputManager::GetMousePos(const SDL_Event &e) { 
    return Vec2(e.motion.x, e.motion.y);
}

bool InputManager::KeyDown(const SDL_Event &e, SDL_KeyCode key) { 
    return e.key.keysym.sym == key;
}

bool InputManager::KeyUp(const SDL_Event &e, SDL_KeyCode key) {
    return e.key.keysym.sym == key;
}

bool InputManager::MouseDown(const SDL_Event &e, int button) { 
    return e.button.button == button;
}

void InputManager::RegisterHandler(SDL_EventType type, const InputManager::EventHandler &handler) {
    if (utils_MapHasKey(InputManager::handlers, type)) {
        assert(false && "Event type is already registered");
    }
    InputManager::handlers.insert(std::make_pair(type, handler));
}

bool InputManager::QueryKey(SDL_Scancode key) {
    auto kbState = SDL_GetKeyboardState(nullptr);
    return kbState[key];
}

bool InputManager::QueryMouse(int button) {
    int v = SDL_GetMouseState(nullptr, nullptr);
    return button == v;
}

Vec2 InputManager::QueryMousePos() {
    int w, h;
    SDL_GetMouseState(&w, &h);
    return Vec2((float) w, (float) h);
}
