#pragma once
#include <SDL2/SDL.h>
#include <memory>
#include "utils.hpp"


namespace chem_war {
    class Renderer final {
    public:
        using Vec2 = ::chem_war::Vec2;

        struct Texture {
            SDL_Texture *textureData;
            Vec2 size;
        };

        static void Initialize();
        static void CreateWindow(int w, int h, const char *title);
        static void CreateWindow(int w, int h, const std::string &title);
        static void CreateWindow(int w, int h, const char *title, Uint32 flags);
        static void Finalize();

        static float GetDeltatime();

        static void Clear();
        static Texture CreateTexture(SDL_Surface *t);
        static void RenderTexture(const Texture &t, const Vec2 &pos);
        static void Update();

        static void Draw(const std::string &drawCall);
        static void ClearDrawColor();
        static void SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        static void DrawRect(const Vec2 &pos, const Vec2 &size);
        static void FillRect(const Vec2 &pos, const Vec2 &size);

    private:
        static Uint64 ticks;
        static float prevFrameDeltatime;
        static SDL_Window *window;
        static SDL_Renderer *renderer;
    };
}
