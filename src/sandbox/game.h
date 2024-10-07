#pragma once
#include "../lib/ecs.h"


namespace chem_war {
    class Game final {
    public:
        static void Prepare(int argc, char **argv);
        static void Run();
        static void Quit();
    private:
        static engine::ecs::World world;
    };
}
