#pragma once
#include "lib/resource.h"
#include "lib/render.h"
#include "lib/input.h"
#include "lib/audio.h"
#include "lib/ecs.h"
#include "enemy.h"

#include "character.h"


namespace chem_war {
    class Game final {
    public:
        enum class State {
            Run, Dead
        };

        static void Prepare(int argc, char **argv);
        static void Run();
        static void Quit();
        template<typename T = GameObject>
        static T *GetObject(const std::string &id) {
            assert(Game::objects.find(id) != Game::objects.end() && "Object not exists");
            return reinterpret_cast<T *>(Game::objects[id]);
        }

        static void AddObject(GameObject *go);

        static void RespawnEnemies();

        static void OnKeyDown(const SDL_Event &e);
    private:
        static State state;
        static bool bgmPlaying;
        static ecs::World world;
        static std::map<std::string, GameObject *> objects;
        static std::vector<Enemy *> enemies;

        static const int maxEnemies = 10;
    };
}

