#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include "utils.hpp"
#include "camera.h"
#include "pool.hpp"

#define MAP_RGBA(fmt, r, g, b, a) SDL_MapRGBA(fmt, r, g, b, a)
#define MAP_COLOR(fmt, color) MAP_RGBA(fmt, color.r, color.g, color.b, color.a)


namespace engine {
    namespace Colors {
        inline Color ColorAlpha(const Color &orig, Uint8 alpha) {
            Color c = { orig.r, orig.g, orig.b, alpha };
            return c;
        }

        inline Color RandColor(bool alphaRand = false) {
            return alphaRand 
                ? Color { (Uint8) (rand() % 256), (Uint8) (rand() % 256), (Uint8) (rand() % 256), (Uint8) (rand() % 256) } 
                : Color { (Uint8) (rand() % 256), (Uint8) (rand() % 256), (Uint8) (rand() % 256), 255 };
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
        const Color Red = { 255, 0, 0, 255 };
        const Color LemonChiffon = { 255, 250, 205, 255 };
        const Color LightBlue = { 173, 216, 230, 255 };
        const Color LightCoral = { 240, 128, 128, 255 };
        const Color LightCyan = { 224, 255, 255, 255 };
        const Color LightGoldenrodYellow = { 250, 250, 210, 255 };
        const Color LightGray = { 211, 211, 211, 255 };
        const Color LightGreen = { 144, 238, 144, 255 };
        const Color LightPink = { 255, 182, 193, 255 };
        const Color LightSalmon = { 255, 160, 122, 255 };
        const Color LightSeaGreen = { 32, 178, 170, 255 };
        const Color LightSkyBlue = { 135, 206, 250, 255 };
        const Color LightSlateGray = { 119, 136, 153, 255 };
        const Color LightSteelBlue = { 176, 196, 222, 255 };
        const Color LightYellow = { 255, 255, 224, 255 };
        const Color Lime = { 0, 255, 0, 255 };
        const Color LimeGreen = { 50, 205, 50, 255 };
        const Color Linen = { 250, 240, 230, 255 };
        const Color Magenta = { 255, 0, 255, 255 };
        const Color Maroon = { 128, 0, 0, 255 };
        const Color MediumAquamarine = { 102, 205, 170, 255 };
        const Color MediumBlue = { 0, 0, 205, 255 };
        const Color MediumOrchid = { 186, 85, 211, 255 };
        const Color MediumPurple = { 147, 112, 219, 255 };
        const Color MediumSeaGreen = { 60, 179, 113, 255 };
        const Color MediumSlateBlue = { 123, 104, 238, 255 };
        const Color MediumSpringGreen = { 0, 250, 154, 255 };
        const Color MediumTurquoise = { 72, 209, 204, 255 };
        const Color MediumVioletRed = { 199, 21, 133, 255 };
        const Color MidnightBlue = { 25, 25, 112, 255 };
        const Color MintCream = { 245, 255, 250, 255 };
        const Color MistyRose = { 255, 228, 225, 255 };
        const Color Moccasin = { 255, 228, 181, 255 };
        const Color NavajoWhite = { 255, 222, 173, 255 };
        const Color Navy = { 0, 0, 128, 255 };
        const Color OldLace = { 253, 245, 230, 255 };
        const Color Olive = { 128, 128, 0, 255 };
        const Color OliveDrab = { 107, 142, 35, 255 };
        const Color Orange = { 255, 165, 0, 255 };
        const Color OrangeRed = { 255, 69, 0, 255 };
        const Color Orchid = { 218, 112, 214, 255 };
        const Color PaleGoldenrod = { 238, 232, 170, 255 };
        const Color PaleGreen = { 152, 251, 152, 255 };
        const Color PaleTurquoise = { 175, 238, 238, 255 };
        const Color PaleVioletRed = { 219, 112, 147, 255 };
        const Color PapayaWhip = { 255, 239, 213, 255 };
        const Color PeachPuff = { 255, 218, 185, 255 };
        const Color Peru = { 205, 133, 63, 255 };
        const Color Pink = { 255, 192, 203, 255 };
        const Color Plum = { 221, 160, 221, 255 };
        const Color PowderBlue = { 176, 224, 230, 255 };
        const Color Purple = { 128, 0, 128, 255 };
        const Color RebeccaPurple = { 102, 51, 153, 255 };
        const Color RosyBrown = { 188, 143, 143, 255 };
        const Color RoyalBlue = { 65, 105, 225, 255 };
        const Color SaddleBrown = { 139, 69, 19, 255 };
        const Color Salmon = { 250, 128, 114, 255 };
        const Color SandyBrown = { 244, 164, 96, 255 };
        const Color SeaGreen = { 46, 139, 87, 255 };
        const Color Seashell = { 255, 245, 238, 255 };
        const Color Sienna = { 160, 82, 45, 255 };
        const Color Silver = { 192, 192, 192, 255 };
        const Color SkyBlue = { 135, 206, 235, 255 };
        const Color SlateBlue = { 106, 90, 205, 255 };
        const Color SlateGray = { 112, 128, 144, 255 };
        const Color Snow = { 255, 250, 250, 255 };
        const Color SpringGreen = { 0, 255, 127, 255 };
        const Color SteelBlue = { 70, 130, 180, 255 };
        const Color Tan = { 210, 180, 140, 255 };
        const Color Teal = { 0, 128, 128, 255 };
        const Color Thistle = { 216, 191, 216, 255 };
        const Color Tomato = { 255, 99, 71, 255 };
        const Color Turquoise = { 64, 224, 208, 255 };
        const Color Violet = { 238, 130, 238, 255 };
        const Color Wheat = { 245, 222, 179, 255 };
        const Color WhiteSmoke = { 245, 245, 245, 255 };
        const Color Yellow = { 255, 255, 0, 255 };
        const Color YellowGreen = { 154, 205, 50, 255 };
    }

    using ValueRetriver = std::function<std::string()>;

    class Renderer final {
    public:
        using Vec2 = ::engine::Vec2;
        using Color = SDL_Color;

        struct Texture {
            SDL_Texture *textureData;
            Vec2 size;
        };

        struct DrawCall {
            char type;
            union {
                int p, q;
                float x, y;
            } p1;
            union {
                int p, q;
                float x, y;
            } p2;
            union {
                int p, q;
                float x, y;
            } p3;
            union {
                void *_placeholder;
                Texture t;
                std::string text;
                Uint32 color;
            } data;

            ~DrawCall() = delete;
        };

        using Surface = SDL_Surface;

        static const int DC_POINT = 0;
        static const int DC_LINE = 1;
        static const int DC_RECT = 2;
        static const int DC_FRECT = 3;
        static const int DC_CIRCLE = 4;
        static const int DC_FCIRCLE = 5;
        static const int DC_TEXTURE = 6;
        static const int DC_CHANGE_COLOR = 7;
        static const int DC_TEXT = 8;
        static const int DC_CLEAR = 9;

        static DrawCall *GenerateDrawCall(const Texture &t);
        static DrawCall *GenerateDrawCall(const std::string &t);
        static DrawCall *GenerateDrawCall(Uint32 color);
        static DrawCall *GenerateDrawCall(char type);

        static void Initialize();
        static void CreateWindow(int w, int h, const char *title);
        static void CreateWindow(int w, int h, const std::string &title);
        static void CreateWindow(int w, int h, const char *title, Uint32 flags);
        static void Finalize();
        static void LoadFont(const std::string &path, int ptSize);
        static void ChangeFontSize(int ptSize);
        static void ReloadFont(const std::string &path, int ptSize);
        static TTF_Font *GetGlobalFont();
        static int GetGlobalFontsize();
        static TTF_Font *GetCurrentFont();
        static int GetCurrentFontSize();
        static bool HasFont();
        static Vec2 GetRenderSize(bool update = false);
        static void SetWindowFlag(Uint32 flags);

        static float GetDeltatime();

        static void Clear();
        static Texture CreateTexture(SDL_Surface *t);
        static void RenderTexture(const Texture &t, const Vec2 &pos);
        static void Update();
        static void RenderAbsolute(const Texture &t, const Vec2 &pos);

        static void EnableAlphaBlend();
        static void DisableAlphaBlend();

        static void Draw(const std::vector<DrawCall *> &drawCalls);
        static void ClearDrawColor();
        static Color GetDrawColor();
        static void SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        static void SetDrawColor(const SDL_Color &color);
        static void DrawRect(const Vec2 &pos, const Vec2 &size);
        static void DrawLineOn(SDL_Surface *dst, Vec2 p1, Vec2 p2, const Color &color);
        static void FillRect(const Vec2 &pos, const Vec2 &size);
        static void FillRectOn(SDL_Surface *dst, const Vec2 &pos, const Vec2 &size, const Color &color);
        static void DrawCircle(const Vec2 &pos, int radius, float delta = 5);
        static void DrawCircleCliped(const Vec2 &center, float radius, int sd, int ed);
        static void FillCircle(const Vec2 &pos, float radius, float delta = 5);
        static void FillCircleCliped(const Vec2 &center, float radius, int sd, int ed, bool fastBlit = true);
        static void FillCircleClipedOn(SDL_Surface *dst, const Vec2 &center, int radius, int sd, int ed, const Color &color);
        static void Line(const Vec2 &st, const Vec2 &et, const Color &color, int stroke = 1);
        static void Line(const Vec2 &st, const Vec2 &et);

        static Texture Text(const std::string &text);
        static Texture Text(const std::string &text, const SDL_Color &color);
        static Texture Text(const std::string &text, const SDL_Color &color, const SDL_Color &bg);
        static Texture TextAlpha(const std::string &text, const SDL_Color &color, const SDL_Color &key);

        static Texture CreateRenderContext(const Vec2 &size);
        static void SetRenderContext(const Texture &ctx);
        static void ClearRenderContext();
        static void DeleteRenderContext(Texture &ctx);
        static void EnableContextBlend(const Texture &context, bool enabled = false);
        static Texture CreateRenderContext(SDL_Surface *surf);

        static void ApplyColorKey(SDL_Surface *s, Color c);

        static void SetGlobalBackGround(const std::string &img);
        static void ClearGlobalBackGround();

        static Vec2 GetTopRightPos();
        static Vec2 GetBottomLeftPos();
        static Vec2 GetBottomRightPos();

        static Texture Clip(SDL_Surface *t, const Vec2 &pos, const Vec2 &size);
        static SDL_Surface *ClipSurface(SDL_Surface *t, const Vec2 &pos, const Vec2 &size);
        static SDL_Surface *ClipCopy(SDL_Surface *t, const Vec2 &pos, const Vec2 &size);
        static SDL_Surface *Scale(SDL_Surface *t, const Vec2 &size);

        static SDL_Renderer *GetRenderer();

        // TODO: Add guassian blur effect
        static SDL_Surface *GaussianBlur(SDL_Surface *src, int radius, bool quality = false);
        static void GaussianBlur(SDL_Surface *src, SDL_Surface *dst, int radius, bool quality = false);
        static void FastGaussianBlur(SDL_Surface *src, SDL_Surface *dst, int radius);
        static SDL_Surface *FastGaussianBlur(SDL_Surface *src, int radius);
        static SDL_Surface *ScaledFastGaussianBlur(SDL_Surface *src, float ratio, int radius);
        static void BoxBlur(SDL_Surface *src, SDL_Surface *dst, int radius);
        static SDL_Surface *BoxBlur(SDL_Surface *src, int radius);

        static void DrawRoundRect(const Vec2 &pos, const Vec2 &size, int radius);
        static void FillRoundRect(const Vec2 &pos, const Vec2 &size, int radius);
        static void FillRoundRectOn(SDL_Surface *dst, const Vec2 &pos, const Vec2 &size, int radius, const Color &color);

        static void AddSlotFont(const std::string &file, int ptSize);
        static TTF_Font *GetSlotFont(int slot);
        static void SetSlotFontsize(int slot, int ptSize);
        static void SetSlotFontsize(int ptSize);
        static int GetSlotFontsize(int slot);
        static int TotalSlotFontsCount();
        static void ChangeFont(int slot, const std::string &file, int ptSize);
        static void ResetFontSlot();
        static void ReleaseSlotFont(int slot);
        static void SetSlotFont(int slot);
        static void SetGlobalFont();
        static int GetCurrentFontSlot();

        static Uint64 GetTicks();
        static void EnableFPSCounter(float fpsQueryFreq = 0.32f);
        static void DisableFPSCounter();
        static void EnableProfiling(const std::string &file, float sampleFreq = 0.32f);
        static void Sample();
        static void EndSample();

        // Low-level SDL surface api
        using SurfaceBlendMode = SDL_BlendMode;
        static Surface *CreateSurface(const Vec2 &size);
        static Surface *CreateSurfaceWithPixelData(void *pixels, const Vec2 &size);
        static void UpperBlit(Surface *src, Surface *dst, const Vec2 &pos, const Vec2 &size);
        static void UpperBlit(Surface *src, Surface *dst);
        static void SetSurfaceBlendMode(Surface *s, SurfaceBlendMode mode);
        static void DeleteSurface(Surface *surf);

        static SDL_Surface *GetRenderBackdrop();
        static void DebugAddHUD(const std::string &name, ValueRetriver retriver, const Color &color = Colors::White);
        static void EnableHUD(float queryRate = 0.32f);
        static void DisableHUD();

    private:
        static Uint64 ticks;
        static float prevFrameDeltatime;
        static SDL_Window *window;
        static SDL_Renderer *renderer;
        static TTF_Font *font;
        static TTF_Font *globalFont;
        static int globalFontSize;

        static Vec2 renderSize;

        static int currentFontsize;
        static SDL_Texture *globalBackground;

        static ObjectPool<Texture> texturePool;
        static Uint32 windowFlags;

        static std::map<TTF_Font *, int> slotFontSizeCache;
        static std::vector<TTF_Font *> fontSlot;
        static int currentFontSlot;
        static bool showFPSCounter;
        static float fpsQueryFreq;
        static bool profiling;
        static float profilingFreq;
        static std::vector<float> profilingData;
        static std::string profileFile;
        static std::map<std::string, std::pair<ValueRetriver, Color>> hud;
        static bool hudEnabled;
        static float hudQueryFreq;
    };

    // To be implemented
    class GLRenderer final {};
}
