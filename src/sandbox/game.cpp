#include "game.h"
#include "engine.h"
#include "../preset/gui.h"

using namespace engine;


void sandbox::Game::Prepare(int argc, char **argv) {
    preset::BeginGUIContext(argc, argv, 800, 600, "Engine");
    Renderer::LoadFont("assets/genshin-font.ttf", 32);
    Renderer::SetGlobalBackGround("assets/bg.png");
}

void sandbox::Game::Run() {
    preset::SetGUIProc([=](float) {
        preset::Begin(Vec2(100, 100), Vec2(300, 300), "Hello, my IMGUI", 0);
        preset::Begin(Vec2(320, 160), Vec2(320, 160), "SubWindow", 0);
        preset::Begin(Vec2(400, 100), Vec2(128, 64), "SubWindow2", 0);
        preset::Begin(Vec2(100, 100), Vec2(300, 300), "Hello, my IMGUI #2", 0);
        preset::Begin(Vec2(320, 160), Vec2(320, 160), "SubWindow3", 0);
        preset::Begin(Vec2(400, 100), Vec2(128, 64), "SubWindow4", 0);
        preset::End();
        preset::End();
        preset::End();
        preset::End();
        preset::End();
        preset::End();
    });
} 

void sandbox::Game::Quit() {
    preset::EndGUIContext();
}
