#include "game.h"
#include "../lib/render.h"
#include "../lib/input.h"
#include "../lib/tile.h"
#include "../lib/resource.h"
#include "character.h"

using namespace engine;


void chem_war::Game::Prepare(int argc, char **argv) {
    Renderer::Initialize();
    InputManager::Initialize();
    ResourceManager::Initialize(argc, argv);

    Renderer::CreateWindow(1366, 768, "Engine");
    ResourceManager::Load("character.figure", engine::ResourceType::Texture, "assets/character/character.figure.4.png");
    auto character = Character::Instance("character.figure");
    character->Prepare();

    TileManager::Initalize();
    TileManager::ConfigTiles(TileManager::LoadFromFile("assets/tile.src"));
    TileManager::SetMap("assets/tilemap.map");
    TileManager::AddLayer("assets/water.map", "assets/water_tile.src");
}

void chem_war::Game::Run() {
    auto character = Character::Instance();

    while (!InputManager::ShouldQuit()) {
        auto dt = Renderer::GetDeltatime();
        Renderer::Clear();
        InputManager::Update();
        TileManager::Update(dt);
        TileManager::Render();
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
    Character::Instance()->Destroy();
    TileManager::Finalize();
    ResourceManager::Finalize();
    InputManager::Finalize();
    Renderer::Finalize();
}
