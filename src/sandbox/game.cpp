#include "game.h"
#include "../lib/render.h"
#include "../lib/input.h"
#include "../lib/resource.h"
#include "../lib/components.h"
#include "character.h"
#include "../lib/fake3d.h"

using namespace engine;

ecs::World chem_war::Game::world;


void chem_war::Game::Prepare(int argc, char **argv) {
    Renderer::Initialize();
    InputManager::Initialize();
    ResourceManager::Initialize(argc, argv);

    Renderer::CreateWindow(1366, 768, "Engine");
    Renderer::LoadFont("assets/wqy-microhei.ttc", 16);
    ResourceManager::Load("character.figure", engine::ResourceType::Texture, "assets/character/character.figure.4.png");
    ResourceManager::Load("enemy.figure", engine::ResourceType::Texture, "assets/imgs/enemy.png");
    Character::Instance()->Prepare();
    Game::world.AddSystem(engine::components::LabelTextRenderSystem)
               .AddSystem(engine::components::MovementSystem);
}

void chem_war::Game::Run() {
    auto character = Character::Instance();

    while (!InputManager::ShouldQuit()) {
        auto dt = Renderer::GetDeltatime();
        Renderer::Clear();
        InputManager::Update();
        Game::world.Update();

        character->Update(dt);
        character->Render();

        if (InputManager::QueryKey(SDL_SCANCODE_W)) {
            character->controller.AddMovement(Direction::Up);
        }
        if (InputManager::QueryKey(SDL_SCANCODE_A)) {
            character->controller.AddMovement(Direction::Left);
        }
        if (InputManager::QueryKey(SDL_SCANCODE_S)) {
            character->controller.AddMovement(Direction::Down);
        }
        if (InputManager::QueryKey(SDL_SCANCODE_D)) {
            character->controller.AddMovement(Direction::Right);
        }

        Renderer::Update(); 
    }
}

void chem_war::Game::Quit() {
    Game::world.Shutdown();
    Character::Instance()->Destroy();
    ResourceManager::Finalize();
    InputManager::Finalize();
    Renderer::Finalize();
}
