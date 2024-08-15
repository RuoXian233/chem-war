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
        static void Prepare(int argc, char **argv);
        static void Run();
        static void Quit();

        static void OnKeyDown(const SDL_Event &e);
    private:
        static bool bgmPlaying;
        static ecs::World world;
        static std::vector<std::shared_ptr<GameObject>> objects;
    };
}

