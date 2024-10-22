#include "input.h"
#include "log.h"

using namespace engine;

bool InputManager::shouldQuit;
SDL_Event InputManager::currentEvent;
std::map<Uint32, std::pair<void *, InputManager::EventHandler>> InputManager::handlers;
std::vector<SDL_Event> InputManager::eventLastFrame;

static Logger logger("InputManager");


void InputManager::Initialize() {
    InputManager::shouldQuit = false;
    InputManager::handlers = decltype(InputManager::handlers)();

    srand((unsigned) time(nullptr));
    logger.SetDisplayLevel(GLOBAL_LOG_LEVEL);
    INFO("InputManager ready");
}

void InputManager::Update() {
    InputManager::eventLastFrame.clear();
    while (SDL_PollEvent(&InputManager::currentEvent)) {
        if (InputManager::currentEvent.type == SDL_QUIT) {
            InputManager::shouldQuit = true;
        }

        if (utils_MapHasKey(InputManager::handlers, InputManager::currentEvent.type)) {
            auto [listener, handler] = InputManager::handlers.at(InputManager::currentEvent.type);
            DEBUG_F("Triggering handler: {}->{} (listener={})", (int) InputManager::currentEvent.type, (void *) listener, (void *) &handler);
            handler(listener, InputManager::currentEvent);
        }

        InputManager::eventLastFrame.push_back(currentEvent);
    }
}

void InputManager::ClearHandlers() {
    InputManager::handlers.clear();
}

void InputManager::Finalize() {}

std::vector<SDL_Event> InputManager::GetEventCache() {
    return InputManager::eventLastFrame;
}

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

void InputManager::RegisterHandler(SDL_EventType type, void *listener, const InputManager::EventHandler &handler) {
    if (utils_MapHasKey(InputManager::handlers, type)) {
        assert(false && "Event type is already registered");
    }
    InputManager::handlers.insert(std::make_pair(type, std::make_pair(listener, handler)));
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
