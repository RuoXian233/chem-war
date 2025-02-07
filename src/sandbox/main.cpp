#include "game.h"
#include "../lib/plugins/profiler.hpp"


int main(int argc, char *argv[]) {
    sandbox::Game::Prepare(argc, argv);
    // Renderer::EnableProfiling("engine_profile.dat");
    Renderer::EnableFPSCounter();
    sandbox::Game::Run();
    sandbox::Game::Quit();
    // engine::OpenProfilingWindow("engine_profile.dat");
    return 0;
}
