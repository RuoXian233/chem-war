#include "render.h"
#include <SDL2/SDL_image.h>
#include "resource.h"
#include "consts.h"
#include "log.h"

using namespace engine;

SDL_Renderer *Renderer::renderer;
SDL_Window *Renderer::window;
Uint64 Renderer::ticks;
float Renderer::prevFrameDeltatime;
TTF_Font *Renderer::font;
int Renderer::currentFontsize;

static Logger logger("Renderer");


void Renderer::Initialize() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    TTF_Init();
    logger.SetDisplayLevel(GLOBAL_LOG_LEVEL);

    SDL_version *ver;
    INFO("Render subsystem initialized");
    DEBUG_F("SDL version: {}.{}.{}", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
    DEBUG_F("SDL_image version: {}.{}.{}", SDL_IMAGE_MAJOR_VERSION, SDL_IMAGE_MINOR_VERSION, SDL_IMAGE_PATCHLEVEL);
    DEBUG_F("SDL_TTF version: {}.{}.{}", TTF_MAJOR_VERSION, TTF_MINOR_VERSION, TTF_PATCHLEVEL);
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
    INFO("Window created");
    logger.StartParagraph(Logger::Level::Debug);
    DEBUG_F("Window size: {}x{}", w, h);
    DEBUG_F("Window flags: {}", flags);
    DEBUG_F("Window handle: {}", (void *) Renderer::window);
    DEBUG_F("Window title: '{}'", title);
    logger.EndParagraph();

    Renderer::renderer = SDL_CreateRenderer(Renderer::window, -1, 0);
    INFO("Renderer created");
    logger.StartParagraph(Logger::Level::Debug);
    DEBUG_F("Renderer handle: {}", (void *) Renderer::renderer);
    DEBUG_F("Standard framerate: {}", MAX_FRAMERATE);
    logger.EndParagraph();
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
    INFO("Render subsystem finalizing ...");
    if (Renderer::HasFont()) {
        DEBUG_F("Closing font: {}", (void *) Renderer::font);
        TTF_CloseFont(Renderer::font);
    }

    SDL_DestroyRenderer(Renderer::renderer);
    SDL_DestroyWindow(Renderer::window);
    INFO("Render subsystem finalized");
    TTF_Quit();
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
    SDL_Rect r;
    if (Camera::GetState().enabled) {
        r = { (int) (pos.x - Camera::GetState().pos.x), (int) (pos.y - Camera::GetState().pos.y), (int) t.size.x, (int) t.size.y };
    } else {
        r = { (int) pos.x, (int) pos.y, (int) t.size.x, (int) t.size.y };
    }
    SDL_RenderCopy(Renderer::renderer, t.textureData, nullptr, &r);
}

void Renderer::RenderAbsolute(const Renderer::Texture &t, const Vec2 &pos) {
    SDL_Rect r = { (int) pos.x, (int) pos.y, (int) t.size.x, (int) t.size.y };
    SDL_RenderCopy(Renderer::renderer, t.textureData, nullptr, &r);
}

float Renderer::GetDeltatime() {
    return Renderer::prevFrameDeltatime;
}

void Renderer::Draw(const std::string &drawCall) {

}

void Renderer::EnableAlphaBlend() {
    DEBUG("Enabling alpha blending");
    SDL_SetRenderDrawBlendMode(Renderer::renderer, SDL_BLENDMODE_BLEND);
}

void Renderer::DisableAlphaBlend() {
    DEBUG("Disabling alpha blending");
    SDL_SetRenderDrawBlendMode(Renderer::renderer, SDL_BLENDMODE_NONE);
}

void Renderer::DrawRect(const Vec2 &pos, const Vec2 &size) {
    SDL_Rect r;
    if (Camera::GetState().enabled) {
        auto transformedPos = pos - Camera::GetState().pos;
        r = Vec2::CreateRect(transformedPos, size);
    } else {
        SDL_Rect r = Vec2::CreateRect(pos, size);
    }
    SDL_RenderDrawRect(Renderer::renderer, &r);
}

void Renderer::FillRect(const Vec2 &pos, const Vec2 &size) {
    SDL_Rect r;
    if (Camera::GetState().enabled) {
        auto transformedPos = pos - Camera::GetState().pos;
        r = Vec2::CreateRect(transformedPos, size);
    } else {
        SDL_Rect r = Vec2::CreateRect(pos, size);
    }
    SDL_RenderFillRect(Renderer::renderer, &r);
}

void Renderer::SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(Renderer::renderer, r, g, b, a);
}

void Renderer::SetDrawColor(const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer::renderer, color.r, color.g, color.b, color.a);
}

void Renderer::ClearDrawColor() {
    Renderer::SetDrawColor(0, 0, 0, 0);
}


bool Renderer::HasFont() {
    return Renderer::font != nullptr;
}

void Renderer::LoadFont(const std::string &font, int ptSize) {
    assert(!HasFont() && "Font resource already exists");
    INFO_F("Loading global font", font);
    DEBUG_F("Font size: {}", ptSize);

    auto f = TTF_OpenFont(font.c_str(), ptSize);
    if (!f) {
        Fatal("Font load failed");
    }
    Renderer::font = f;
    DEBUG_F("Font handle: {}", (void *) Renderer::font);
    Renderer::currentFontsize = ptSize;
}

TTF_Font *Renderer::GetFont() {
    return Renderer::font;
}

void Renderer::ChangeFontSize(int ptSize) {
    assert(HasFont() && "No font to be changed");
    TTF_SetFontSize(Renderer::font, ptSize);
    // DEBUG_F("Font size changed to: {}", ptSize);
    Renderer::currentFontsize = ptSize;
}

void Renderer::ReloadFont(const std::string &font, int ptSize) {
    assert(HasFont() && "No font is loaded, use Renderer::LoadFont() instead");
    TTF_CloseFont(Renderer::font);
    Renderer::font = nullptr;
    Renderer::LoadFont(font, ptSize);
}

Renderer::Texture Renderer::Text(const std::string &text) {
    SDL_Color c;
    SDL_GetRenderDrawColor(Renderer::renderer, &c.r, &c.g, &c.b, &c.a);
    return Renderer::Text(text, c);
}

Renderer::Texture Renderer::Text(const std::string &text, const SDL_Color &color) {
    assert(HasFont() && "No font resource");
    SDL_Surface *surf = TTF_RenderUTF8_Blended(Renderer::font, text.c_str(), color);
    auto texture = SDL_CreateTextureFromSurface(Renderer::renderer, surf);
#ifndef __linux__
    ResourceManager::RegisterResource(std::format("text.Texture {}{}{}", ResourceManager::Size(), (void *) texture, (long long) rand() + (long long) rand()), ResourceType::Texture, surf);
#else
    ResourceManager::RegisterResource(std::format("text.Texture {}{}", (void *) texture, rand()), ResourceType::Texture, surf, LifeCycleSpec::Temporary);
#endif

    Renderer::Texture t;
    t.size.x = surf->w;
    t.size.y = surf->h;
    t.textureData = texture;
    return t;
}

Vec2 Renderer::GetRenderSize() {
    int w, h;
    SDL_GetWindowSize(Renderer::window, &w, &h);
    return Vec2 { (float) w, (float) h };
}

void Renderer::DrawCircle(const Vec2 &pos, int radius, float delta) {
    auto drawPos = pos;
    if (Camera::GetState().enabled) {
        drawPos = pos - Camera::GetState().pos;
    }

    float angle = 0;
    for (int i = 0; i < 360 / delta; i++){
        float prevradian = DEG_TO_RAD(angle), nextradian = DEG_TO_RAD(angle + delta);
        SDL_RenderDrawLine(Renderer::renderer, 
            drawPos.x + radius * cosf(prevradian),
            drawPos.y + radius * sinf(prevradian),
            drawPos.x + radius * cosf(nextradian),
            drawPos.y + radius * sinf(nextradian)
        );
        angle += delta;
    }
}

Renderer::Texture Renderer::CreateRenderContext(const Vec2 &size) {
    auto t = SDL_CreateTexture(Renderer::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.x, size.y);
    Renderer::Texture textureWrapper = { t, size };
    return textureWrapper;
}

void Renderer::SetRenderContext(const Renderer::Texture &texture) {
    SDL_SetRenderTarget(Renderer::renderer, texture.textureData);
}

void Renderer::ClearRenderContext() {
    SDL_SetRenderTarget(Renderer::renderer, nullptr);
}

void Renderer::DeleteRenderContext(Renderer::Texture &texture) {
    SDL_DestroyTexture(texture.textureData);
    texture.textureData = nullptr;
    texture.size = Vec2();
}

int Renderer::GetCurrentFontsize() {
    return Renderer::currentFontsize;
}

Renderer::Texture Renderer::Clip(SDL_Surface *t, const Vec2 &pos, const Vec2 &size) {
    auto texture = Renderer::CreateRenderContext(size);
    auto textCache = SDL_CreateTextureFromSurface(renderer, t);
    auto rect = Vec2::CreateRect(pos, size);

    Renderer::SetRenderContext(texture);
    SDL_RenderCopy(renderer, textCache, &rect, nullptr); 
    Renderer::ClearRenderContext();
    SDL_DestroyTexture(textCache);
    return texture;
}
