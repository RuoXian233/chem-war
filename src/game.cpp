#include "game.h"
#include "lib/components.h"
#include "ui/debug_panel.h"
#include "lib/particle.h"
#include "lib/camera.h"
#include "lib/animation.h"

#include "lib/effects.h"

using namespace chem_war;

bool Game::bgmPlaying;
ecs::World Game::world;
Game::State Game::state;


static Logger logger("Game");

void GameScene::OnFirstEnter() {
    auto c = Character::Instance();
    InputManager::RegisterHandler(SDL_KEYDOWN, this, GameScene::OnKeyDown);

    c->Transform(Vec2(-480, -480));

    for (int i = 0; i < 10; i++) {
        enemies.emplace_back(new Enemy(this->world));
    }

    auto dbgPanel = new ui::DebugPanel(this->world, Vec2());
    dbgPanel->AddItem("character.velocity", [=]() { return c->GetVelocity().Length(); });
    dbgPanel->AddItem("character.pos", [=]() { return c->GetPos().ToString(); });

    for (int i = 0; i < 10; i++) {
        dbgPanel->AddItem(std::format("enemy{}.<movement>", i), [=, this]() { 
            if (enemies[i]) {
                const auto &comp = enemies[i]->GetComponent<components::Movement>(); 
            return std::format("inc={}, v={}, p={}, d(character)={}", enemies[i]->GetSpeedIncreament(), comp.velocity.ToString(), comp.pos.ToString(), (c->GetPos() - comp.pos).ToString());
            }
            return std::string("null");
        });
    }

    dbgPanel->AddItem("global.score", [=]() { return Game::score; });
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

    auto bar = new ui::Bar("character.hp", this->world, Vec2(800, 320));
    bar->Config(2, { 255, 0, 0, 255 }, { 255, 255, 255, 255 }, Vec2(c->GetSize().x, 16));
    bar->AttachValue([=]() { return c->GetHp() / c->GetMaxHp(); });
    // bar->SetParent(dbgPanel);
    dbgPanel->AddChildren(bar);

    this->AddObject(dbgPanel);

    c->anim = new Animation({ 
        "character.figure.1",
        "character.figure.2",
        "character.figure.3",
        "character.figure.4",
        "character.figure.5"
    }, 32);

    EffectSystem::SetTargetScene(this);
    SceneManager::PrintSceneHeirarchy();
}

void DeadScene::OnFirstEnter() {
    Scene::OnFirstEnter();
    auto gameScene = SceneManager::GetScene("game");
    auto dbgPanel = gameScene->GetObject<ui::DebugPanel>("ui.debug.panel");
    dbgPanel->Hide(false);
    this->AddBorrowedObject(dbgPanel);
    Renderer::ChangeFontSize(32);
}

void GameScene::Update(float dt) {
    auto c = Character::Instance();
    Scene::Update(dt);
    c->ClearCollision();

    c->CheckMovement();
    c->Update(dt);
    Game::score += dt * 100 + c->hits * 10;
    c->hits = 0;

    for (auto &&enemy : enemies) {
        if (enemy && !enemy->dead) {
            enemy->Update(dt);
        } else if (enemy) {
            delete enemy;
            enemy = nullptr;
        }
    }

    c->anim->Update(dt);
    auto bar = this->GetObject<ui::DebugPanel>("ui.debug.panel")->GetChildrens()[0];
    bar->GetComponent<components::Movement>().pos = c->GetPos() + Vec2(0, -20);
    this->RespawnEnemies();
}

void GameScene::Render() {
    Scene::Render();
    auto c = Character::Instance();

    c->Render();
    c->DrawCollider();

    for (auto &&enemy : enemies) {
        if (enemy && !enemy->dead) {
            enemy->Render();
        }
    }

    c->anim->Next(Vec2(400, 600));
}

void DeadScene::Render() {
    Scene::Render();
    Renderer::SetDrawColor(255, 100, 100, 255);
    Renderer::RenderTexture(Renderer::Text(std::format("You Died! Total Score: {}", Game::score)), Vec2(500, 300));
    Renderer::ClearDrawColor();
}

void Game::Prepare(int argc, char **argv) {
    Renderer::Initialize();
    InputManager::Initialize();
    Renderer::CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chem War");
    Camera::Initalize();
    Camera::Enabled(true);
    ResourceManager::Initialize(argc, argv);
    AnimationManager::Initialize();
    ResourcePack pack;
    Renderer::LoadFont("assets/wqy-microhei.ttc", 16);
    AudioManager::Initialize();
    for (int i = 1; i <= CHARACTER_FIGURES_COUNT; i++) {
        auto resourceURI = std::format("{}.{}", CHARACTER_FIGURE_URI, i);
        pack.emplace_back(ResourceManager::Load(
            resourceURI, ResourceType::Texture,
            std::format("{}/{}.{}", CHARACTER_FIGURE_PATH, resourceURI, CHARACTER_FIGURE_EXT)
        ));
    }
    Character::Instance(Game::world, pack)->Prepare();

    ResourceManager::Load("bgm", ResourceType::Music, std::format("{}/{}.flac", BGM_PATH, "bgm"));
    AudioManager::SetBGM(ResourceManager::Get("bgm")->GetAs<Mix_Music>());
    auto gameScene = new GameScene(Game::world);
    auto deadScene = new DeadScene(Game::world);
    SceneManager::Initialize();
    SceneManager::AddScene("game", gameScene);
    SceneManager::AddScene("dead", deadScene);

    Game::world.AddSystem(components::MovementSystem)
               .AddSystem(components::GraphRenderSystem)
               .AddSystem(components::BasicTextRenderSystem)
               .AddSystem(components::Texture2DRenderSystem)
               .AddSystem(components::SimpleCollider2DSystem)
               .AddSystem(components::SimpleTimerSystem)
               .Startup();

    ParticleManager::Initialize();
    EffectSystem::Initalize();
    
    Vec2 gravity = { 0, 0 };
    ParticleManager::CreateParticleSystem(gravity);
}

void Game::Run() {
    AudioManager::PlayBGM();
    Game::bgmPlaying = false;
    Game::state = Game::State::Run;
    SceneManager::SwitchScene("game");

    WARNING("Current in alpha version, debug mode is default to `on`");

    while (!InputManager::ShouldQuit()) {
        Renderer::Clear();
        InputManager::Update();
        float dt = Renderer::GetDeltatime();
        
        Game::world.Update();

        SceneManager::Update(dt);
        SceneManager::Render();
        ParticleManager::Update(dt);

        auto c = Character::Instance();
        if (c->GetHp() <= 0 && Game::state == Game::State::Run) {    
            Game::state = Game::State::Dead;
            Character::Instance()->OnDie();
            SceneManager::SwitchScene("dead");
        }

        EffectSystem::Update(dt);
        EffectSystem::Render();

        ResourceManager::Check();
        Renderer::Update();
    }
}

void Game::Quit() {
    Camera::Finalize();
    EffectSystem::Finalize();
    SceneManager::Finalize();
    ParticleManager::Finalize();
    Game::world.Shutdown();
    AudioManager::StopBGM();
    AudioManager::Finalize();
    Character::Instance()->Destroy();
    InputManager::Finalize();
    AnimationManager::Finalize();
    ResourceManager::Finalize();
    Renderer::Finalize();
}

void GameScene::OnKeyDown(void *gameScene, const SDL_Event &e) {
    auto c = Character::Instance();
    auto scene = static_cast<GameScene *>(gameScene);

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
    if (InputManager::QueryKey(SDL_SCANCODE_SPACE)) {
        c->StartAttack();
    }
    if (InputManager::KeyDown(e, SDLK_m)) {
        Game::bgmPlaying = !Game::bgmPlaying;
        if (Game::bgmPlaying)
        {
            if (!AudioManager::HasBGM())
            {
                auto bgm = ResourceManager::Get("bgm");
                AudioManager::SetBGM(bgm->GetAs<Mix_Music>());
                AudioManager::PlayBGM();
            }
            AudioManager::ResumeBGM();
        }
        else
        {
            AudioManager::PauseBGM();
        }
    }
    if (InputManager::KeyDown(e, SDLK_F3)) {
        auto o = scene->GetObject<ui::DebugPanel>("ui.debug.panel");
        bool state = !o->IsHide();
        auto c = Character::Instance();

        c->ColliderVisibility(state);
        o->Hide(state);
        ecs::Querier q(scene->world);
        c->showBulletCollider = state;

        for (auto o : scene->enemies) {
            if (o) {
                o->GetComponent<components::SimpleCollider2D>().showCollider = state;
            }
        }
        Enemy::showCollider = state;
    }

    if (InputManager::KeyDown(e, SDLK_e)) {
        effects::SceneFadeOut(640);
    }
    if (InputManager::KeyDown(e, SDLK_q)) {
        effects::Shine(160, { 255, 255, 0, 255 }, 3);
    }
}

void GameScene::RespawnEnemies() {
    int enemyCount = 0;
    for (const auto enemy : this->enemies) {
        if (enemy) {
            enemyCount++;
        }
    }

    if (enemyCount < 1) {
        enemies.clear();
        for (int i = 0; i < Game::maxEnemies; i++) {
            enemies.emplace_back(new Enemy(this->world));
        }
    }
}
