#include "game.h"
#include "lib/object.h"
#include "lib/components.h"
#include "ui/debug_panel.h"
#include "lib/particle.h"

using namespace chem_war;

bool Game::bgmPlaying;
ecs::World Game::world;
std::map<std::string, GameObject *> Game::objects;
Game::State Game::state;
std::vector<Enemy *> Game::enemies;


void Game::AddObject(GameObject *go) {
    Game::objects.insert(std::make_pair(go->GetId(), go));
}

void Game::Prepare(int argc, char **argv) {
    Renderer::Initialize();
    InputManager::Initialize();
    Renderer::CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chem War");
    InputManager::RegisterHandler(SDL_KEYDOWN, Game::OnKeyDown);
    ResourcePack pack;
    ResourceManager::Initialize(argc, argv);

    Renderer::LoadFont("/usr/share/fonts/wenquanyi/wqy-microhei/wqy-microhei.ttc", 16);
    // AudioManager::Initialize();

    for (int i = 1; i <= CHARACTER_FIGURES_COUNT; i++) {
        auto resourceURI = std::format("{}.{}", CHARACTER_FIGURE_URI, i);
        pack.emplace_back(ResourceManager::Load(
            resourceURI, ResourceType::Texture,
            std::format("{}/{}.{}", CHARACTER_FIGURE_PATH, resourceURI, CHARACTER_FIGURE_EXT)
        ));
    }

    // ResourceManager::Load("bgm", ResourceType::Music, std::format("{}/{}.flac", BGM_PATH, "bgm"));

    Character::Instance(Game::world, pack)->Prepare();
    auto c = Character::Instance();
    c->Transform(Vec2(-480, -480));

    for (int i = 0; i < 10; i++) {
        enemies.emplace_back(new Enemy(Game::world));
    }
    auto dbgPanel = new ui::DebugPanel(Game::world, Vec2());
    dbgPanel->AddItem("character.velocity", [=]() { return c->GetVelocity().Length(); });
    dbgPanel->AddItem("character.pos", [=]() { return c->GetPos().ToString(); });

    for (int i = 0; i < 10; i++) {
        dbgPanel->AddItem(std::format("enemy{}.<movement>", i), [=]() { 
            if (enemies[i]) {
                const auto &comp = enemies[i]->GetComponent<components::Movement>(); 
            return std::format("inc={}, v={}, p={}, d(character)={}", enemies[i]->GetSpeedIncreament(), comp.velocity.ToString(), comp.pos.ToString(), (c->GetPos() - comp.pos).ToString());
            }
            return std::string("null");
        });
    }
    
    
    dbgPanel->AddItem("global.dt", [=]() { return Renderer::GetDeltatime(); });
    dbgPanel->AddItem("render.font", [=]() { return std::format("{}", (void *) Renderer::GetFont()); });
    dbgPanel->AddItem("render.size", [=]() { return Renderer::GetRenderSize().ToString(); });
    dbgPanel->AddItem("resources.count", [=]() { return ResourceManager::Size(); });
    dbgPanel->AddItem("input.mouse.pos", [=]() { return InputManager::QueryMousePos().ToString(); });
    dbgPanel->AddItem("input.mouse.button", [=]() { 
        return std::format("({},{},{})", InputManager::QueryMouse(1), InputManager::QueryMouse(2), InputManager::QueryMouse(4));
    });
    dbgPanel->AddItem("character.hp", [=]() { return c->GetHp(); });
    dbgPanel->AddItem("character.collisions", [=]() { 
        return c->GetCollisions().size(); 
    });
    dbgPanel->AddItem("character.bullet.hits", [=]() { return c->hits; });

    auto bar = new ui::Bar("character.hp", Game::world, Vec2(800, 320));
    bar->Config(2, { 255, 0, 0, 255 }, { 255, 255, 255, 255 }, Vec2(c->GetSize().x, 16));
    bar->AttachValue([=]() { return c->GetHp() / c->GetMaxHp(); });

    Game::AddObject(dbgPanel);
    Game::AddObject(bar);

    Game::world.AddSystem(components::MovementSystem)
               .AddSystem(components::GraphRenderSystem)
               .AddSystem(components::BasicTextRenderSystem)
               .AddSystem(components::Texture2DRenderSystem)
               .AddSystem(components::SimpleCollider2DSystem)
               .Startup();

    ParticleManager::Initialize();
    
    Vec2 gravity = { 0, 0 };
    ParticleManager::CreateParticleSystem(gravity);
}

void Game::Run() {
    // AudioManager::PlayBGM();
    auto bar = Game::GetObject<ui::Bar>("ui.bar.character.hp");
    Game::bgmPlaying = false;
    Game::state = Game::State::Run;
    while (!InputManager::ShouldQuit()) {
        Renderer::Clear();
        InputManager::Update();
        float dt = Renderer::GetDeltatime();
        if (Game::state == Game::State::Run) {
            auto c = Character::Instance();
            c->hits = 0;
            Game::world.Update();

            c->CheckMovement();
            c->Update(dt);
            c->DrawCollider();
            c->Render();
            c->ClearCollision();

            ParticleManager::Update(dt);
            bar->GetComponent<components::Movement>().pos = c->GetPos() + Vec2(0, -20);

            if (c->GetHp() <= 0) {    
                Game::state = Game::State::Dead;
            }

             for (auto &&[_, o] : Game::objects) {
                o->Update(dt);
                o->Render();
            }

            for (auto &&enemy : Game::enemies) {
                if (enemy && !enemy->dead) {
                    enemy->Update(dt);
                    enemy->Render();
                } else if (enemy) {
                    delete enemy;
                    enemy = nullptr;
                }
            }
            Game::RespawnEnemies();
        }

        if (Game::state == Game::State::Dead) {
            Renderer::SetDrawColor(255, 100, 100, 255);
            Renderer::ChangeFontSize(32);
            Renderer::RenderTexture(Renderer::Text("You Died!"), Vec2(600, 300));
            Renderer::ChangeFontSize(16);
            Renderer::ClearDrawColor();
        }
        
        ResourceManager::Check();
        Renderer::Update();
    }
}

void Game::Quit() {
    for (auto &&[_, obj] : Game::objects) {
        delete obj;
    }

    ParticleManager::Finalize();
    Game::world.Shutdown();
    // AudioManager::StopBGM();
    // AudioManager::Finalize();
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
    // if (InputManager::KeyDown(e, SDLK_1)) {
    //     c->SetState(1);
    // }
    // if (InputManager::KeyDown(e, SDLK_2)) {
    //     c->SetState(2);
    // }
    // if (InputManager::KeyDown(e, SDLK_3)) {
    //     c->SetState(3);
    // }
    // if (InputManager::KeyDown(e, SDLK_4)) {
    //     c->SetState(4);
    // }
    // if (InputManager::KeyDown(e, SDLK_5)) {
    //     c->SetState(5);
    // }
    // if (InputManager::KeyDown(e, SDLK_UP)) {
    //     c->Transform(Vec2(10, 10));
    // }
    // if (InputManager::KeyDown(e, SDLK_DOWN)) {
    //     c->Transform(Vec2(-10, -10));
    // }
    if (InputManager::QueryKey(SDL_SCANCODE_SPACE)) {
        c->StartAttack();
    }
    if (InputManager::KeyDown(e, SDLK_m)) {
        // Game::bgmPlaying = !Game::bgmPlaying;
        // if (Game::bgmPlaying)
        // {
        //     if (!AudioManager::HasBGM())
        //     {
        //         auto bgm = ResourceManager::Get("bgm");
        //         AudioManager::SetBGM(bgm->GetAs<Mix_Music>());
        //         AudioManager::PlayBGM();
        //     }
        //     AudioManager::ResumeBGM();
        // }
        // else
        // {
        //     AudioManager::PauseBGM();
        // }
    }
    if (InputManager::KeyDown(e, SDLK_F3)) {
        auto o = Game::GetObject<ui::DebugPanel>("ui.debug.panel");
        bool state = !o->IsHide();
        auto c = Character::Instance();

        c->ColliderVisibility(state);
        o->Hide(state);
        ecs::Querier q(Game::world);
        c->showBulletCollider = state;

        for (auto o : Game::enemies) {
            if (o) {
                o->GetComponent<components::SimpleCollider2D>().showCollider = state;
            }
        }
    }
}

void Game::RespawnEnemies() {
    int enemyCount = 0;
    for (const auto enemy : Game::enemies) {
        if (enemy) {
            enemyCount++;
        }
    }

    if (enemyCount < 1) {
        enemies.clear();
        for (int i = 0; i < maxEnemies; i++) {
            enemies.emplace_back(new Enemy(Game::world));
        }
    }
}
