#pragma once
#include "../lib/ecs.h"
#include "../lib/render.h"

using namespace engine;

namespace sandbox {
    class Game final {
    public:
        static void Prepare(int argc, char **argv);
        static void Run();
        static void Quit();
    };
}
