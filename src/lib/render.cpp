#include "render.h"
#include <SDL2/SDL_image.h>

using namespace chem_war;

SDL_Renderer *Renderer::renderer;
SDL_Window *Renderer::window;
Uint64 Renderer::ticks;
float Renderer::prevFrameDeltatime;


void Renderer::Initialize() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    Renderer::ticks = 0;
}

void Renderer::CreateWindow(int w, int h, const std::string &title) {
    Renderer::CreateWindow(w, h, title.c_str(), SDL_WINDOW_SHOWN);
}

void Renderer::CreateWindow(int w, int h, const char *title) {
    Renderer::CreateWindow(w, h, title, SDL_WINDOW_SHOWN);
}

void Renderer::CreateWindow(int w, int h, const char *title, Uint32 flags) {
    Renderer::window = SDL_CreateWindow(
        title, 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        w, h,
        flags
    );

    Renderer::renderer = SDL_CreateRenderer(Renderer::window, -1, 0);
}


void Renderer::Clear() {
    Renderer::ticks = SDL_GetPerformanceCounter();
    SDL_RenderClear(Renderer::renderer);
}

void Renderer::Update() {
    SDL_RenderPresent(Renderer::renderer);
    Renderer::prevFrameDeltatime = (SDL_GetPerformanceCounter() - Renderer::ticks) / (float) (SDL_GetPerformanceFrequency());
    if (prevFrameDeltatime < 1.0f / MAX_FRAMERATE) {
        SDL_Delay((Uint32) (1000 * (1.0f / MAX_FRAMERATE - prevFrameDeltatime)));
        Renderer::prevFrameDeltatime += (1.0f / MAX_FRAMERATE - prevFrameDeltatime);
    }
}

void Renderer::Finalize() {
    SDL_DestroyRenderer(Renderer::renderer);
    SDL_DestroyWindow(Renderer::window);
    IMG_Quit();
    SDL_Quit();
}


Renderer::Texture Renderer::CreateTexture(SDL_Surface *s) {
    Texture t;
    t.textureData = SDL_CreateTextureFromSurface(Renderer::renderer, s);
    t.size = Vec2(s->w, s->h);
    return t;
}

void Renderer::RenderTexture(const Renderer::Texture &t, const Vec2 &pos) {
    SDL_Rect r = { (int) pos.x, (int) pos.y, (int) t.size.x, (int) t.size.y };
    SDL_RenderCopy(Renderer::renderer, t.textureData, nullptr, &r);
}

float Renderer::GetDeltatime() {
    return Renderer::prevFrameDeltatime;
}

void Renderer::Draw(const std::string &drawCall) {

}

void Renderer::DrawRect(const Vec2 &pos, const Vec2 &size) {
    SDL_Rect r = Vec2::CreateRect(pos, size);
    SDL_RenderDrawRect(Renderer::renderer, &r);
}

void Renderer::FillRect(const Vec2 &pos, const Vec2 &size) {
    SDL_Rect r = Vec2::CreateRect(pos, size);
    SDL_RenderFillRect(Renderer::renderer, &r);
}

void Renderer::SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(Renderer::renderer, r, g, b, a);
}

void Renderer::ClearDrawColor() {
    Renderer::SetDrawColor(0, 0, 0, 0);
}
