#include "game.h"
#include "lib/object.h"
#include "components.h"

using namespace chem_war;

bool Game::bgmPlaying;
ecs::World Game::world;


void Game::Prepare(int argc, char **argv) {
    Renderer::Initialize();
    InputManager::Initialize();
    Renderer::CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chem War");
    InputManager::RegisterHandler(SDL_KEYDOWN, Game::OnKeyDown);
    ResourcePack pack;
    ResourceManager::Initialize(argc, argv);
    AudioManager::Initialize();

    for (int i = 1; i <= CHARACTER_FIGURES_COUNT; i++) {
        auto resourceURI = std::format("{}.{}", CHARACTER_FIGURE_URI, i);
        pack.emplace_back(ResourceManager::Load(
            resourceURI, ResourceType::Texture,
            std::format("{}/{}.{}", CHARACTER_FIGURE_PATH, resourceURI, CHARACTER_FIGURE_EXT)
        ));
    }

    ResourceManager::Load("bgm", ResourceType::Music, std::format("{}/{}.flac", BGM_PATH, "bgm"));

    Character::Instance(pack)->Prepare();
    auto c = Character::Instance();
    c->Transform(Vec2(-390, -390));

    auto e = Enemy(Game::world);
    e.GetComponent<components::Movement>().velocity = Vec2(100, 100);

    Game::world.AddSystem(components::MovementSystem)
               .AddSystem(components::Texture2DRenderSystem)
               .AddSystem(components::GraphRenderSystem)
               .Startup();
}

void Game::Run() {
    // AudioManager::PlayBGM();
    Game::bgmPlaying = false;
    while (!InputManager::ShouldQuit()) {
        Renderer::Clear();

        InputManager::Update();
        auto c = Character::Instance();

        c->CheckMovement();
        c->Update(Renderer::GetDeltatime());
        c->Render();
        Game::world.Update();

        Renderer::Update();
    }
}

void Game::Quit() {
    Game::world.Shutdown();
    AudioManager::StopBGM();
    AudioManager::Finalize();
    Character::Instance()->Destroy();
    InputManager::Finalize();
    ResourceManager::Finalize();
    Renderer::Finalize();
}


void Game::OnKeyDown(const SDL_Event &e) {
    auto c = Character::Instance();

    if (InputManager::QueryKey(SDL_SCANCODE_W)) {
        c->AddMovement(Direction::Up);
    }
    if (InputManager::QueryKey(SDL_SCANCODE_S)) {
        c->AddMovement(Direction::Down);
    } 
    if (InputManager::QueryKey(SDL_SCANCODE_A)) {
        c->AddMovement(Direction::Left);
    }
    if (InputManager::QueryKey(SDL_SCANCODE_D)) {
        c->AddMovement(Direction::Right);
    }
    if (InputManager::KeyDown(e, SDLK_1)) {
        c->SetState(1);
    }
    if (InputManager::KeyDown(e, SDLK_2)) {
        c->SetState(2);
    }
    if (InputManager::KeyDown(e, SDLK_3)) {
        c->SetState(3);
    }
    if (InputManager::KeyDown(e, SDLK_4)) {
        c->SetState(4);
    }
    if (InputManager::KeyDown(e, SDLK_5)) {
        c->SetState(5);
    }
    if (InputManager::KeyDown(e, SDLK_UP)) {
        c->Transform(Vec2(10, 10));
    }
    if (InputManager::KeyDown(e, SDLK_DOWN)) {
        c->Transform(Vec2(-10, -10));
    }
    if (InputManager::KeyDown(e, SDLK_SPACE)) {
        Game::bgmPlaying = !Game::bgmPlaying;
        if (Game::bgmPlaying) {
            if (!AudioManager::HasBGM()) {
                auto bgm = ResourceManager::Get("bgm");
                AudioManager::SetBGM(bgm->GetAs<Mix_Music>());
                AudioManager::PlayBGM();
            }
            AudioManager::ResumeBGM();
        } else {
            AudioManager::PauseBGM();
        }
    }
}
