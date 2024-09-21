#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include "utils.hpp"
#include "camera.h"
#include "pool.hpp"


namespace engine {
    namespace Colors {
        inline Color ColorAlpha(const Color &orig, Uint8 alpha) {
            Color c = { orig.r, orig.g, orig.b, alpha };
            return c;
        } 

        const Color White = { 255, 255, 255, 255 };
        const Color Black = { 0, 0, 0, 255 };
        const Color AliceBlue = { 240, 248, 255, 255 };
        const Color AntiqueWhite = { 250, 235, 215, 255 };
        const Color Aqua = { 0, 255, 255, 255 };
        const Color Aquamarine = { 127, 255, 212, 255 };
        const Color Azure = { 240, 255, 255, 255 };
        const Color Beige = { 245, 245, 220, 255 };
        const Color Bisque = { 255, 228, 196, 255 };
        const Color Blanchedalmond = { 255, 235, 205, 255 };
        const Color Blue = { 0, 0, 255, 255 };
        const Color Blueviolet = { 138, 43, 226, 255 };
        const Color Brown = { 165, 42, 42, 255 };
        const Color Burlywood = { 222, 184, 135, 255 };
        const Color Cadetblue = { 95, 158, 160, 255 };
        const Color Chartreuse = { 127, 255, 0, 255 };
        const Color Chocolate = { 210, 105, 30, 255 };
        const Color Coral = { 255, 127, 80, 255 };
        const Color Cornflowerblue = { 100, 49, 237, 255 };
        const Color Cornsilk = { 255, 248, 220, 255 };
        const Color Crimson = { 220, 20, 60, 255 };
        const Color Cyan = Aqua;
        const Color Darkblue = { 0, 0, 139, 255 };
        const Color Darkcyan = { 0,139, 139, 255 };
        const Color Darkgoldenrod = { 184, 134, 11, 255 };
        const Color Darkgray = { 169, 169, 169, 255 };
        const Color Darkgreen = { 0, 100, 0, 255 };
        const Color Darkkhaki = { 189, 183, 107, 255 };
        const Color Darkmagenta = { 139, 0, 139, 255 };
        const Color Darkolivegreen = { 85, 107, 47, 255 };
        const Color Darkorange = { 255, 140, 0, 255 };
        const Color Darkorchid = { 153, 50, 204, 255 };
        const Color Darkred = { 139, 0, 0, 255 };
        const Color Darksalmon = { 233, 150, 122, 255 };
        const Color Darkseagreen = { 143, 188, 143, 255 };
        const Color Darkslateblue = { 72, 61, 139, 255 };
        const Color Darkslategray = { 47, 79, 79, 255 };
        const Color Darkturquoise = { 0, 206,209, 255 };
        const Color Darkviolet = { 148, 0, 211, 255 };
        const Color Deeppink = { 255, 20, 147, 255 };
        const Color Deepskyblue = { 0, 191, 255, 255 };
        const Color Dimgray = { 105, 105, 105, 255 };
        const Color Dodgerblue = { 30, 144, 255, 255 };
        const Color Firebrick = { 178, 34, 34, 255 };
        const Color Forestgreen = { 34, 139, 34, 255 };
        const Color Fuchsia = { 255, 0, 255, 255 };
        const Color Gainsboro = { 220, 220, 220, 255 }; 
        const Color Gold = { 255, 215, 0, 255 };
        const Color Goldenrod = { 218, 165, 32, 255 };
        const Color Gray = { 128, 128, 128, 255 };
        const Color Green = { 0, 128, 0, 255 };
        const Color Greenyellow = { 173, 255, 47, 255 };
        const Color Hotpink = { 255, 105, 180, 255 };
        const Color Indianred = { 205, 92, 92, 255 };
        const Color Indigo = { 75, 0, 130, 255 };
        const Color Khaki = { 240, 230, 140, 255 };
        const Color Lavender = { 230, 230, 250, 255 };
        const Color Lawngreen = { 124,252, 0, 255 };
    }

    class Renderer final {
    public:
        using Vec2 = ::engine::Vec2;
        using Color = SDL_Color;

        struct Texture {
            SDL_Texture *textureData;
            Vec2 size;
        };

        static void Initialize();
        static void CreateWindow(int w, int h, const char *title);
        static void CreateWindow(int w, int h, const std::string &title);
        static void CreateWindow(int w, int h, const char *title, Uint32 flags);
        static void Finalize();
        static void LoadFont(const std::string &path, int ptSize);
        static void ChangeFontSize(int ptSize);
        static void ReloadFont(const std::string &path, int ptSize);
        static TTF_Font *GetFont();
        static int GetCurrentFontsize();
        static bool HasFont();
        static Vec2 GetRenderSize();

        static float GetDeltatime();

        static void Clear();
        static Texture CreateTexture(SDL_Surface *t);
        static void RenderTexture(const Texture &t, const Vec2 &pos);
        static void Update();
        static void RenderAbsolute(const Texture &t, const Vec2 &pos);

        static void EnableAlphaBlend();
        static void DisableAlphaBlend();

        static void Draw(const std::string &drawCall);
        static void ClearDrawColor();
        static void SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        static void SetDrawColor(const SDL_Color &color);
        static void DrawRect(const Vec2 &pos, const Vec2 &size);
        static void FillRect(const Vec2 &pos, const Vec2 &size);
        static void DrawCircle(const Vec2 &pos, int radius, float delta = 5);

        static Texture Text(const std::string &text);
        static Texture Text(const std::string &text, const SDL_Color &color);
        static Texture FastText(const std::string &text);
        static Texture FastText(const std::string &text, const SDL_Color &color);

        static Texture CreateRenderContext(const Vec2 &size);
        static void SetRenderContext(const Texture &ctx);
        static void ClearRenderContext();
        static void DeleteRenderContext(Texture &ctx);

        static Texture Clip(SDL_Surface *t, const Vec2 &pos, const Vec2 &size);

    private:
        static Uint64 ticks;
        static float prevFrameDeltatime;
        static SDL_Window *window;
        static SDL_Renderer *renderer;
        static TTF_Font *font;

        static int currentFontsize;

        static ObjectPool<Texture> texturePool;
    };
}
