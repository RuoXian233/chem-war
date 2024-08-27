#pragma once
#include "lib/resource.h"
#include "lib/render.h"
#include "lib/input.h"
#include "lib/audio.h"
#include "lib/ecs.h"
#include "lib/scene.h"
#include "lib/log.h"
#include "enemy.h"

#include "character.h"

using namespace engine;


namespace chem_war {
    class GameScene final : public Scene {
    public:
        GameScene(ecs::World &world) : Scene(world) {}
        void OnFirstEnter() override;
        void Update(float dt) override;
        void Render() override;

        inline void OnAdd() override {};
        inline void OnRemove() override {};

        void RespawnEnemies();
        static void OnKeyDown(void *scene, const SDL_Event &e);

    private:
        std::vector<Enemy *> enemies;
    }; 

    class DeadScene final : public Scene {
    public:
        DeadScene(ecs::World &world) : Scene(world) {}
        void OnFirstEnter() override;
        void Render() override;

        inline void OnAdd() override {};
        inline void OnRemove() override {};
        inline void Exit() override {};
    };

    class Game final {
    public:
        enum class State {
            Run, Dead
        };

        static void Prepare(int argc, char **argv);
        static void Run();
        static void Quit();

        static const int maxEnemies = 32;
        static inline int score = 0;

        static bool bgmPlaying;
    private:
        static State state;
        static ecs::World world;
        std::vector<std::string> scenes;
    };
}

