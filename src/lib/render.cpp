#include "render.h"
#include <SDL_image.h>
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
SDL_Texture *Renderer::globalBackground;
Vec2 Renderer::renderSize;
Uint32 Renderer::windowFlags;
std::vector<TTF_Font *> Renderer::fontSlot;
std::map<TTF_Font *, int> Renderer::slotFontSizeCache;
int Renderer::globalFontSize;
TTF_Font *Renderer::globalFont;
int Renderer::currentFontSlot;
bool Renderer::showFPSCounter;
float Renderer::fpsQueryFreq;
bool Renderer::profiling;
float Renderer::profilingFreq;
std::vector<float> Renderer::profilingData;
std::string Renderer::profileFile;
std::map<std::string, std::pair<ValueRetriver, Color>> Renderer::hud;
bool Renderer::hudEnabled;
float Renderer::hudQueryFreq;

static Logger logger("Renderer");


void Renderer::Initialize() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    TTF_Init();
    logger.SetDisplayLevel(GLOBAL_LOG_LEVEL);
    globalBackground = nullptr;

    SDL_version *ver;
    INFO("Render subsystem initialized");
    DEBUG_F("SDL version: {}.{}.{}", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
    DEBUG_F("SDL_image version: {}.{}.{}", SDL_IMAGE_MAJOR_VERSION, SDL_IMAGE_MINOR_VERSION, SDL_IMAGE_PATCHLEVEL);
    DEBUG_F("SDL_TTF version: {}.{}.{}", TTF_MAJOR_VERSION, TTF_MINOR_VERSION, TTF_PATCHLEVEL);
    Renderer::ticks = 0;
    Renderer::showFPSCounter = false;
}

void Renderer::CreateWindow(int w, int h, const std::string &title) {
    Renderer::CreateWindow(w, h, title.c_str(), SDL_WINDOW_SHOWN | Renderer::windowFlags);
}

void Renderer::CreateWindow(int w, int h, const char *title) {
    Renderer::CreateWindow(w, h, title, SDL_WINDOW_SHOWN | Renderer::windowFlags);
}

void Renderer::CreateWindow(int w, int h, const char *title, Uint32 flags) {
    Renderer::window = SDL_CreateWindow(
        title, 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        w, h,
        flags
    );
    if (!Renderer::window) {
        Fatal("Window creation failed");
        return;
    }

    INFO("Window created");
    logger.StartParagraph(Logger::Level::Debug);
    DEBUG_F("Window size: {}x{}", w, h);
    DEBUG_F("Window flags: {}", flags);
    DEBUG_F("Window handle: {}", (void *) Renderer::window);
    DEBUG_F("Window title: '{}'", title);
    logger.EndParagraph();
    renderSize = Vec2(w, h);

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

    if (Renderer::globalBackground) {
        SDL_RenderCopy(renderer, Renderer::globalBackground, nullptr, nullptr);
    }
}

void Renderer::Update() {
    static SDL_Texture *t;
    static std::vector<SDL_Texture *> hudTextures;
    static SDL_Rect r;
    static std::vector<SDL_Rect> r0;
    static float counterTimer;
    static float counterProfilerTimer;
    static float counterHUDTimer;
    if (Renderer::showFPSCounter) {
        if (Renderer::fpsQueryFreq == 0.0f || counterTimer > Renderer::fpsQueryFreq) {
            if (t) {
                SDL_DestroyTexture(t);
            }
            auto str = std::format("{:.2f}", 1 / Renderer::prevFrameDeltatime);
            const int FPS_COUNTER_FONT_SIZE = 24;
            auto originalFontsize = globalFontSize;
            Renderer::ChangeFontSize(FPS_COUNTER_FONT_SIZE);
            auto surf = TTF_RenderText_Blended(Renderer::globalFont, str.c_str(), { 192, 192, 192, 255 });
            Renderer::ChangeFontSize(originalFontsize);
            t = SDL_CreateTextureFromSurface(Renderer::renderer, surf);
            SDL_FreeSurface(surf);
            TTF_SizeText(Renderer::globalFont, str.c_str(), &r.w, &r.h);
            counterTimer = 0;
        }
        if (t) {
            SDL_RenderCopy(Renderer::renderer, t, nullptr, &r);
        }
        counterTimer += Renderer::prevFrameDeltatime;
    }

    if (Renderer::hudEnabled) {
        const int HUD_FONTSIZE = 16;
        std::vector<SDL_Surface *> hudSurfaces;
        std::vector<std::string> strings;
        if (Renderer::hudQueryFreq == 0.0f || counterHUDTimer > Renderer::hudQueryFreq) {
            auto originalFontsize = Renderer::globalFontSize;
            Renderer::ChangeFontSize(HUD_FONTSIZE);
            for (auto texture : hudTextures) {
                if (texture) {
                    SDL_DestroyTexture(texture);
                }
            }
            hudTextures.clear();
            strings.clear();
            for (const auto &[hudTag, retriverInfo] : Renderer::hud) {
                auto [retreiver, color] = retriverInfo;
                auto str = std::format("{}: {}", hudTag, retreiver());
                strings.push_back(str);
                hudSurfaces.push_back(TTF_RenderText_Blended(Renderer::globalFont, str.c_str(), color));
            }
            for (auto surf : hudSurfaces) {
                hudTextures.push_back(SDL_CreateTextureFromSurface(renderer, surf));
                SDL_FreeSurface(surf);
            }
            for (int i = 0; i < hudSurfaces.size(); i++) {
                hudSurfaces[i] = nullptr;
            }
            counterHUDTimer = 0;
            int h = 32;
            for (auto [i, text] : Enumerate(hudTextures)) {
                SDL_Rect r = { 0, (int) (1.5 * h) };
                TTF_SizeUTF8(globalFont, strings[i].c_str(), &r.w, &r.h);
                h += r.h;
                r0.push_back(r);
            }
            Renderer::ChangeFontSize(originalFontsize);
        }
        for (auto [i, text] : Enumerate(hudTextures)) {
            SDL_RenderCopy(renderer, text, nullptr, &r0[i]);
        }
        counterHUDTimer += Renderer::prevFrameDeltatime;
    }

    if (Renderer::profiling && profilingFreq > 0.0f) {
        if (counterProfilerTimer > Renderer::profilingFreq) {
            Renderer::Sample();
            counterProfilerTimer = 0;
        }
        counterProfilerTimer += Renderer::prevFrameDeltatime;
    }

    SDL_RenderPresent(Renderer::renderer);
    Renderer::prevFrameDeltatime = (SDL_GetPerformanceCounter() - Renderer::ticks) / (float) (SDL_GetPerformanceFrequency());
    if (prevFrameDeltatime < 1.0f / MAX_FRAMERATE) {
        SDL_Delay((Uint32) (1000 * (1.0f / MAX_FRAMERATE - prevFrameDeltatime)));
        Renderer::prevFrameDeltatime += (1.0f / MAX_FRAMERATE - prevFrameDeltatime);
    }
}

void Renderer::Finalize() {
    INFO("Render subsystem finalizing ...");
    Renderer::ClearGlobalBackGround();
    if (Renderer::HasFont()) {
        DEBUG_F("Closing font: {}", (void *) Renderer::font);
        TTF_CloseFont(Renderer::font);
    }

    if (Renderer::profiling) {
        INFO("Save profiling data");
        EndSample();
    }

    ResetFontSlot();
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
    #ifdef LOW_FRAMERATE_WARNING
        if (Renderer::prevFrameDeltatime > 1 / (0.6 * MAX_FRAMERATE)) {
            WARNING_F("Too much render time cost: {} sec", Renderer::prevFrameDeltatime);
        }
    #endif

    return Renderer::prevFrameDeltatime;
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
        r = Vec2::CreateRect(pos, size);
    }
    SDL_RenderDrawRect(Renderer::renderer, &r);
}

void Renderer::FillRect(const Vec2 &pos, const Vec2 &size) {
    SDL_Rect r;
    if (Camera::GetState().enabled) {
        auto transformedPos = pos - Camera::GetState().pos;
        r = Vec2::CreateRect(transformedPos, size);
    } else {
        r = Vec2::CreateRect(pos, size);
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
    Renderer::globalFont = f;
    DEBUG_F("Font handle: {}", (void *) Renderer::globalFont);
    Renderer::globalFontSize = ptSize;
}

TTF_Font *Renderer::GetGlobalFont() {
    return Renderer::globalFont;
}

void Renderer::ChangeFontSize(int ptSize) {
    assert(globalFont && "No font to be changed");
    TTF_SetFontSize(Renderer::globalFont, ptSize);
    // DEBUG_F("Font size changed to: {}", ptSize);
    Renderer::globalFontSize = ptSize;
}

void Renderer::ReloadFont(const std::string &font, int ptSize) {
    assert(globalFont && "No font is loaded, use Renderer::LoadFont() instead");
    TTF_CloseFont(Renderer::globalFont);
    Renderer::globalFont = nullptr;
    Renderer::LoadFont(font, ptSize);
}

Renderer::Texture Renderer::Text(const std::string &text) {
    SDL_Color c;
    SDL_GetRenderDrawColor(Renderer::renderer, &c.r, &c.g, &c.b, &c.a);
    return Renderer::Text(text, c);
}

Renderer::Texture Renderer::Text(const std::string &text, const SDL_Color &color) {
    assert((HasFont() || globalFont) && "No font resource");
    SDL_Surface *surf;
    if (font) {
        surf = TTF_RenderUTF8_Blended(Renderer::font, text.c_str(), color);
    } else {
        surf = TTF_RenderUTF8_Blended(Renderer::globalFont, text.c_str(), color);
    }
    auto texture = SDL_CreateTextureFromSurface(Renderer::renderer, surf);
// #ifndef __linux__
//     ResourceManager::RegisterResource(std::format("text.Texture {}{}{}", ResourceManager::Size(), (void *) texture, (long long) rand() + (long long) rand()), ResourceType::Texture, surf);
// #else
//     ResourceManager::RegisterResource(std::format("text.Texture {}{}", (void *) texture, rand()), ResourceType::Texture, surf, LifeCycleSpec::Temporary);
// #endif

    Renderer::Texture t;
    t.size.x = surf->w;
    t.size.y = surf->h;
    t.textureData = texture;
    SDL_FreeSurface(surf);
    return t;
}

Renderer::Texture Renderer::Text(const std::string &text, const SDL_Color &color, const SDL_Color &bg) {
    assert((HasFont() || globalFont) && "No font resource");

    SDL_Surface *surf;
    if (font) {
        surf = TTF_RenderUTF8_Blended(Renderer::font, text.c_str(), color);
    } else {
        surf = TTF_RenderUTF8_Blended(Renderer::globalFont, text.c_str(), color);
    }
    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, bg.r, bg.g, bg.b));
    auto texture = SDL_CreateTextureFromSurface(Renderer::renderer, surf);
// #ifndef __linux__
//     ResourceManager::RegisterResource(std::format("text.Texture {}{}{}", ResourceManager::Size(), (void *) texture, (long long) rand() + (long long) rand()), ResourceType::Texture, surf);
// #else
//     ResourceManager::RegisterResource(std::format("text.Texture {}{}", (void *) texture, rand()), ResourceType::Texture, surf, LifeCycleSpec::Temporary);
// #endif

    Renderer::Texture t;
    t.size.x = surf->w;
    t.size.y = surf->h;
    t.textureData = texture;
    SDL_FreeSurface(surf);
    return t;
}

Renderer::Texture Renderer::TextAlpha(const std::string &text, const SDL_Color &color, const SDL_Color &key) {
    assert((HasFont() || globalFont) && "No font resource");
    SDL_Surface *surf;
    
    if (font) {
        surf = TTF_RenderUTF8_Blended(Renderer::font, text.c_str(), color);
    } else {
        surf = TTF_RenderUTF8_Blended(Renderer::globalFont, text.c_str(), color);   
    }
    SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, key.r, key.g, key.b));
    auto texture = SDL_CreateTextureFromSurface(Renderer::renderer, surf);

    Renderer::Texture t;
    t.size.x = surf->w;
    t.size.y = surf->h;
    t.textureData = texture;
    SDL_FreeSurface(surf);
    return t;
}

Vec2 Renderer::GetRenderSize(bool update) {
    int w, h;
    SDL_GetWindowSize(Renderer::window, &w, &h);
    if (update) {
        Renderer::renderSize = Vec2(w, h);
    }
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

int Renderer::GetGlobalFontsize() {
    return Renderer::globalFontSize;
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

void Renderer::SetGlobalBackGround(const std::string &img) {
    INFO_F("Background changed to: {}", img);
    if (!Renderer::globalBackground) {
        auto bgSurf = IMG_Load(img.c_str());
        if (!bgSurf) {
            ERROR_F("Load background failed: {} (loading image)", img);
            ERROR_F("Cause: {}", SDL_GetError());
            return;
        }
        Renderer::globalBackground = SDL_CreateTextureFromSurface(renderer, bgSurf);
        if (!Renderer::globalBackground) {
            ERROR_F("Load background failed: {} (creating texture)", img);
            ERROR_F("Cause: {}", SDL_GetError());
        }
        SDL_FreeSurface(bgSurf);
    }
}

void Renderer::ClearGlobalBackGround() {
    if (Renderer::globalBackground) {
        SDL_DestroyTexture(Renderer::globalBackground);
        Renderer::globalBackground = nullptr;
    }
}

void Renderer::ApplyColorKey(SDL_Surface *s, Color c) {
    SDL_SetColorKey(s, SDL_TRUE, SDL_MapRGB(s->format, c.r, c.g, c.b));
}

void Renderer::Line(const Vec2 &st_, const Vec2 &et_, const Color &color, int stroke) {
    Vec2 st = st_, et = et_;
    if (Camera::GetState().enabled) {
        st -= Camera::GetState().pos;
        et -= Camera::GetState().pos;
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (stroke == 1) {
        SDL_RenderDrawLine(renderer, st.x, st.y, et.x, et.y);
    } else if (stroke > 1) {
        // draw multiple parallel lines to simulate thick line
        for (int i = 0; i < stroke / 2; i++) {
            SDL_RenderDrawLine(renderer, st.x, st.y - i, et.x, et.y - i);
        }
        for (int i = 0; i < stroke / 2; i++) {
            SDL_RenderDrawLine(renderer, st.x, st.y + i, et.x, et.y + i);
        }
    }
    Renderer::ClearDrawColor();
}

void Renderer::Line(const Vec2 &st, const Vec2 &et) {
    SDL_RenderDrawLine(renderer, st.x, st.y, et.x, et.y);
}

Vec2 Renderer::GetBottomLeftPos() {
    return Vec2(0, Renderer::renderSize.y);
}

Vec2 Renderer::GetBottomRightPos() {
    return Renderer::renderSize;
}

Vec2 Renderer::GetTopRightPos() {
    return Vec2(Renderer::renderSize.x, 0);
}

Color Renderer::GetDrawColor() {
    Color c;
    SDL_GetRenderDrawColor(Renderer::renderer, &c.r, &c.g, &c.b, &c.a);
    return c;
}

void Renderer::EnableContextBlend(const Texture &context, bool enabled) {
    SDL_SetTextureBlendMode(context.textureData, enabled ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
}

SDL_Renderer *Renderer::GetRenderer() {
    return Renderer::renderer;
}

void Renderer::SetWindowFlag(Uint32 flags) {
    Renderer::windowFlags = flags;
}

void Renderer::GaussianBlur(SDL_Surface *src, SDL_Surface *dst, int radius, bool quality) {
    Uint8 *srcpx = (Uint8 *)src->pixels;
    Uint8 *dstpx = (Uint8 *)dst->pixels;
    Uint8 nb = src->format->BytesPerPixel;
    int w = dst->w, h = dst->h;
    int dst_pitch = dst->pitch;
    int src_pitch = src->pitch;
    int i, j, x, y, color;
    int kernel_radius = radius * 2;
    float *buf = (float *) malloc(dst_pitch * sizeof(float));
    float *buf2 = (float *) malloc(dst_pitch * sizeof(float));
    float *lut = (float *) malloc((kernel_radius + 1) * sizeof(float));
    float lut_sum = 0.0;

    for (i = 0; i <= kernel_radius; i++) {  // init gaussian lut
        // Gaussian function
        lut[i] =
            expf(-powf((float)i, 2.0f) / (2.0f * powf((float)radius, 2.0f)));
        lut_sum += lut[i] * 2;
    }
    lut_sum -= lut[0];
    for (i = 0; i <= kernel_radius; i++) {
        lut[i] /= lut_sum;
    }

    for (i = 0; i < dst_pitch; i++) {
        buf[i] = 0.0;
        buf2[i] = 0.0;
    }

    for (y = 0; y < h; y++) {
        for (j = -kernel_radius; j <= kernel_radius; j++) {
            for (i = 0; i < dst_pitch; i++) {
                if (y + j >= 0 && y + j < h) {
                    buf[i] +=
                        (float)srcpx[src_pitch * (y + j) + i] * lut[abs(j)];
                }
                else if (quality) {
                    if (y + j < 0) {
                        buf[i] += (float)srcpx[i] * lut[abs(j)];
                    }
                    else {
                        buf[i] += (float)srcpx[src_pitch * (h - 1) + i] *
                                  lut[abs(j)];
                    }
                }
            }
        }

        for (x = 0; x < w; x++) {
            for (j = -kernel_radius; j <= kernel_radius; j++) {
                for (color = 0; color < nb; color++) {
                    if (x + j >= 0 && x + j < w) {
                        buf2[nb * x + color] +=
                            buf[nb * (x + j) + color] * lut[abs(j)];
                    }
                    else if (quality) {
                        if (x + j < 0) {
                            buf2[nb * x + color] += buf[color] * lut[abs(j)];
                        }
                        else {
                            buf2[nb * x + color] +=
                                buf[nb * (w - 1) + color] * lut[abs(j)];
                        }
                    }
                }
            }
        }
        for (i = 0; i < dst_pitch; i++) {
            dstpx[dst_pitch * y + i] = (Uint8)buf2[i];
            buf[i] = 0.0;
            buf2[i] = 0.0;
        }
    }

    free(buf);
    free(buf2);
    free(lut);
}

SDL_Surface *Renderer::GaussianBlur(SDL_Surface *src, int radius, bool quality) {
    SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormat(0, src->w, src->h, src->format->BitsPerPixel, src->format->format);
    Renderer::GaussianBlur(src, dst, radius, quality);
    return dst;
}

void Renderer::FillCircle(const Vec2 &center_, float radius, float delta) {
    Vec2 center = center_;
    if (Camera::GetState().enabled) {
        center -= Camera::GetState().pos;
    }
    for (float w = 0; w < radius * 2; w += delta * 0.1) {
        for (float h = 0; h < radius * 2; h += delta * 0.1) {
            float dx = radius - w;
            float dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPointF(renderer, dx + center.x, center.y + dy);
            }
        }
    }
}

void Renderer::DrawCircleCliped(const Vec2 &center, float radius, int sd, int ed) {
    if (ed < sd) {
        int tmp = sd;
        sd = ed;
        ed = tmp;
    }
    if (ed == 0 && sd == 0) {
        ed = 360;
    }
    for (int deg = sd; deg < ed; deg++) {
        float theta = (float) deg / 180.0f * PI;
        SDL_RenderDrawPointF(renderer, center.x + SDL_cosf(theta) * radius, center.y + SDL_sinf(theta) * radius);
    }
}

void Renderer::FillCircleCliped(const Vec2 &center, float radius, int sd, int ed, bool fastBlit) {
    if (ed < sd) {
        int tmp = sd;
        sd = ed;
        ed = tmp;
    }
    if (ed == 0 && sd == 0) {
        ed = 360;
    }

    if (fastBlit) {
        for (int deg = sd; deg < ed; deg++) {
            float theta = (float) deg / 180.0f * PI;
            SDL_RenderDrawLineF(renderer, center.x, center.y, center.x + SDL_cosf(theta) * radius, center.y + SDL_sinf(theta) * radius);
        }
    } else {
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                auto v = Vec2(radius - w, radius - h);
                if (v.Length() <= radius && Vec2::Angle(v) >= sd && Vec2::Angle(v) <= ed) {
                    SDL_RenderDrawPointF(renderer, v.x + center.x, center.y + v.y);
                }
            }
        }
    }
}

void Renderer::DrawRoundRect(const Vec2 &pos_, const Vec2 &size, int radius) {
    Vec2 pos = pos_;
    if (Camera::GetState().enabled) {
        pos -= Camera::GetState().pos;
    }
    Renderer::DrawCircleCliped(Vec2(pos.x + radius, pos.y + radius), radius, 180, 270); // Top-left
    Renderer::DrawCircleCliped(Vec2(pos.x + size.x - radius, pos.y + radius), radius, 270, 360); // Top-right
    Renderer::DrawCircleCliped(Vec2(pos.x + radius, pos.y + size.y - radius), radius, 90, 180); // Bottom-left
    Renderer::DrawCircleCliped(Vec2(pos.x + size.x - radius, pos.y + size.y - radius), radius, 0, 90); // Bottom-right

    SDL_RenderDrawLine(renderer, pos.x + radius, pos.y, pos.x + size.x - radius, pos.y); // Top
    SDL_RenderDrawLine(renderer, pos.x + radius, pos.y + size.y, pos.x + size.x - radius, pos.y + size.y); // Bottom
    SDL_RenderDrawLine(renderer, pos.x, pos.y + radius, pos.x, pos.y + size.y - radius); // Left
    SDL_RenderDrawLine(renderer, pos.x + size.x, pos.y + radius, pos.x + size.x, pos.y + size.y - radius); // Right
}

void Renderer::FillRoundRect(const Vec2 &pos_, const Vec2 &size, int radius) {
    Vec2 pos = pos_;
    if (Camera::GetState().enabled) {
        pos -= Camera::GetState().pos;
    }
    // -1 is floating point error correction
    Renderer::FillCircleCliped(pos + Vec2(radius - 1, radius - 1), radius, 180, 270, false); // Top-left
    Renderer::FillCircleCliped(pos + Vec2(size.x - radius, radius), radius, 270, 360, false); // Top-right
    Renderer::FillCircleCliped(pos + Vec2(radius - 1, size.y - radius), radius, 90, 180, false); // Bottom-left
    Renderer::FillCircleCliped(pos + Vec2(size.x - radius, size.y - radius), radius, 0, 90, false); // Bottom-right

    SDL_RenderDrawLineF(renderer, pos.x + radius, pos.y, pos.x + size.x - radius, pos.y); // Top
    SDL_RenderDrawLineF(renderer, pos.x + radius, pos.y + size.y, pos.x + size.x - radius, pos.y + size.y); // Bottom
    SDL_RenderDrawLineF(renderer, pos.x, pos.y + radius, pos.x, pos.y + size.y - radius); // Left
    SDL_RenderDrawLineF(renderer, pos.x + size.x, pos.y + radius, pos.x + size.x, pos.y + size.y - radius); // Right

    SDL_FRect rect = { pos.x + radius, pos.y, size.x - 2 * radius, size.y };
    SDL_FRect mask1 = { pos.x, pos.y + radius, (float) radius, size.y - 2 * radius };
    SDL_FRect mask2 = { pos.x + size.x - radius, pos.y + radius, (float) radius, size.y - 2 * radius };
    SDL_RenderFillRectF(renderer, &rect);
    SDL_RenderFillRectF(renderer, &mask1);
    SDL_RenderFillRectF(renderer, &mask2);
}

SDL_Surface *Renderer::ClipSurface(SDL_Surface *t, const Vec2 &pos, const Vec2 &size) {
    if (!t) {
        return nullptr;
    }

    SDL_Rect clipRect;
    clipRect.x = static_cast<int>(pos.x);
    clipRect.y = static_cast<int>(pos.y);
    clipRect.w = static_cast<int>(size.x);
    clipRect.h = static_cast<int>(size.y);

    SDL_Surface *clippedSurface = SDL_CreateRGBSurfaceWithFormatFrom(
        static_cast<void*>(static_cast<Uint8*>(t->pixels) + clipRect.y * t->pitch + clipRect.x * t->format->BytesPerPixel),
        clipRect.w,
        clipRect.h,
        t->format->BitsPerPixel,
        t->pitch,
        t->format->format
    );

    if (!clippedSurface) {
        return nullptr;
    }

    return clippedSurface;
}

SDL_Surface *Renderer::ClipCopy(SDL_Surface *t, const Vec2 &pos, const Vec2 &size) {
    if (!t) {
        return nullptr;
    }

    SDL_Rect clipRect;
    clipRect.x = static_cast<int>(pos.x);
    clipRect.y = static_cast<int>(pos.y);
    clipRect.w = static_cast<int>(size.x);
    clipRect.h = static_cast<int>(size.y);

    SDL_Surface *clippedSurface = SDL_CreateRGBSurface(
        0,
        clipRect.w,
        clipRect.h,
        t->format->BitsPerPixel,
        t->format->Rmask,
        t->format->Gmask,
        t->format->Bmask,
        t->format->Amask
    );

    if (!clippedSurface) {
        return nullptr;
    }

    SDL_BlitSurface(t, &clipRect, clippedSurface, nullptr);

    return clippedSurface;
}

SDL_Surface *Renderer::Scale(SDL_Surface *t, const Vec2 &size) {
    auto s = SDL_CreateRGBSurfaceWithFormat(0, (int) size.x, (int) size.y, t->format->BitsPerPixel, t->format->format);
    SDL_BlitScaled(t, nullptr, s, nullptr);
    return s;
}

void Renderer::DrawLineOn(SDL_Surface *surface, Vec2 p1, Vec2 p2, const Color &color) {

    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
    int sx = (p1.x < p2.x) ? 1 : -1;
    int sy = (p1.y < p2.y) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (p1.x >= 0 && p1.x < surface->w && p1.y >= 0 && p1.y < surface->h) {
            ((Uint32 *) surface->pixels)[(int) (p1.y * surface->pitch / 4) + (int) p1.x] = 
                SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
        }

        if (p1.x == p2.x && p1.y == p2.y) {
            break;
        }

        float e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            p1.x += sx;
        }
        if (e2 < dx) {
            err += dx;
            p1.y += sy;
        }
    }
}

void Renderer::FillCircleClipedOn(SDL_Surface *surface, const Vec2 &center, int radius, int sd, int ed, const Color &color) {
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    Uint32 mappedColor = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);

    auto isInAngleRange = [](int angle, int start, int end) {
        if (angle < 0) {
            angle = 360 + angle;
        }
        if (start <= end) {
            return angle >= start && angle <= end;
        } else {
            return angle >= start || angle <= end;
        }
    };

    while (x >= y) {
        for (int i = center.x - x; i <= center.x + x; i++) {
            int angle1 = static_cast<int>(atan2(y, i - center.x) * 180 / M_PI);
            int angle2 = static_cast<int>(atan2(-y, i - center.x) * 180 / M_PI);
            if (isInAngleRange(angle1, sd, ed) && i >= 0 && i < surface->w && center.y + y >= 0 && center.y + y < surface->h) {
                ((Uint32*)surface->pixels)[(int) ((center.y + y) * surface->pitch / 4 + i)] = mappedColor;
            }
            if (isInAngleRange(angle2, sd, ed) && i >= 0 && i < surface->w && center.y - y >= 0 && center.y - y < surface->h) {
                ((Uint32*)surface->pixels)[(int) ((center.y - y) * surface->pitch / 4 + i)] = mappedColor;
            }
        }

        for (int i = center.x - y; i <= center.x + y; i++) {
            int angle1 = static_cast<int>(atan2(x, i - center.x) * 180 / M_PI);
            int angle2 = static_cast<int>(atan2(-x, i - center.x) * 180 / M_PI);
            if (isInAngleRange(angle1, sd, ed) && i >= 0 && i < surface->w && center.y + x >= 0 && center.y + x < surface->h) {
                ((Uint32*)surface->pixels)[(int) ((center.y + x) * surface->pitch / 4 + i)] = mappedColor;
            }
            if (isInAngleRange(angle2, sd, ed) && i >= 0 && i < surface->w && center.y - x >= 0 && center.y - x < surface->h) {
                ((Uint32*)surface->pixels)[(int) ((center.y - x) * surface->pitch / 4 + i)] = mappedColor;
            }
        }

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void Renderer::FillRoundRectOn(SDL_Surface *dst, const Vec2 &pos, const Vec2 &size, int radius, const Color &color) {
    // -1 is floating point error correction
    Renderer::FillCircleClipedOn(dst, pos + Vec2(radius - 1, radius - 1), radius, 180, 270, color); // Top-left
    Renderer::FillCircleClipedOn(dst, pos + Vec2(size.x - radius, radius), radius, 270, 360, color); // Top-right
    Renderer::FillCircleClipedOn(dst, pos + Vec2(radius - 1, size.y - radius), radius, 90, 180, color); // Bottom-left
    Renderer::FillCircleClipedOn(dst, pos + Vec2(size.x - radius, size.y - radius), radius, 0, 90, color); // Bottom-right

    // what did '-1' do? It just works fine :)
    Renderer::DrawLineOn(dst, { pos.x + radius, pos.y }, { pos.x + size.x - radius, pos.y }, color); // Top
    Renderer::DrawLineOn(dst, { pos.x + radius, pos.y + size.y - 1 }, { pos.x + size.x - radius, pos.y + size.y - 1 }, color); // Bottom
    Renderer::DrawLineOn(dst, { pos.x, pos.y + radius }, { pos.x, pos.y + size.y - radius }, color); // Left
    Renderer::DrawLineOn(dst, { pos.x + size.x - 1, pos.y + radius }, { pos.x + size.x - 1, pos.y + size.y - radius }, color); // Right

    SDL_Rect rect = { (int) pos.x + radius, (int) pos.y, (int) size.x - 2 * radius, (int) size.y };
    SDL_Rect mask1 = { (int) pos.x, (int) pos.y + radius, (int) radius, (int) size.y - 2 * radius };
    SDL_Rect mask2 = { (int) pos.x + (int) size.x - radius, (int) pos.y + radius, (int) radius, (int) size.y - 2 * radius };
    auto colorValue = SDL_MapRGBA(dst->format, color.r, color.g, color.b, color.a);
    SDL_FillRect(dst, &rect, colorValue);
    SDL_FillRect(dst, &mask1, colorValue);
    SDL_FillRect(dst, &mask2, colorValue);
}


Renderer::DrawCall *Renderer::GenerateDrawCall(const Texture &t) {
    auto dc = (DrawCall *) malloc(sizeof(DrawCall));
    dc->type = (char) DC_TEXTURE;
    dc->data.t = t;
    return dc;
}

Renderer::DrawCall *Renderer::GenerateDrawCall(const std::string &t) {
    auto dc = (DrawCall *) malloc(sizeof(DrawCall));
    dc->type = (char) DC_TEXT;
    dc->data.text = t;
    return dc;
}

Renderer::DrawCall *Renderer::GenerateDrawCall(Uint32 color) {
    auto dc = (DrawCall *) malloc(sizeof(DrawCall));
    dc->type = (char) DC_CHANGE_COLOR;
    dc->data.color = color;
    return dc;
}

Renderer::DrawCall *Renderer::GenerateDrawCall(char type) {
    auto dc = (DrawCall *) malloc(sizeof(DrawCall));
    dc->type = type;
    dc->data._placeholder = nullptr;
    return dc;
}

// NOTE: Draw call list will be immediately released after render a frame
void Renderer::Draw(const std::vector<DrawCall *> &drawCalls) {
    for (DrawCall *dc : drawCalls) {
        if (dc) {
            switch (dc->type) {
            case DC_POINT:
                SDL_RenderDrawPointF(Renderer::renderer, dc->p1.x, dc->p1.y);
                break;
            case DC_LINE:
                SDL_RenderDrawLineF(Renderer::renderer, dc->p1.x, dc->p1.y, dc->p2.x, dc->p2.y);
                break;
            case DC_RECT: 
                {
                    SDL_FRect r = { dc->p1.x, dc->p1.y, dc->p2.x, dc->p2.y };
                    SDL_RenderDrawRectF(Renderer::renderer, &r);
                    break;
                }
            case DC_FRECT:
                {
                    SDL_FRect r = { dc->p1.x, dc->p1.y, dc->p2.x, dc->p2.y };
                    SDL_RenderFillRectF(Renderer::renderer, &r);
                    break;
                }
            case DC_CHANGE_COLOR: 
                {
                    Uint8 r, g, b, a;
                    SDL_PixelFormat *f = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
                    SDL_GetRGBA(dc->data.color, f, &r, &g, &b, &a);
                    SDL_FreeFormat(f);
                    SDL_SetRenderDrawColor(Renderer::renderer, r, g, b, a);
                    break;
                }
            default:
                break;
            }
        }
    }

    for (DrawCall *dc : drawCalls) {
        free(dc);
    }
}


Renderer::Texture Renderer::CreateRenderContext(SDL_Surface *surf) {
    Texture t;
    t.size = Vec2(surf->w, surf->h);
    t.textureData = SDL_CreateTextureFromSurface(Renderer::renderer, surf);
    return t;
}

TTF_Font *Renderer::GetCurrentFont() {
    return Renderer::font;
}

int Renderer::GetCurrentFontSize() {
    return Renderer::currentFontsize;
}


void Renderer::AddSlotFont(const std::string &file, int ptSize) {
    TTF_Font *f = TTF_OpenFont(file.c_str(), ptSize);
    DEBUG_F("Add a new font to font slot: {},{}", file, ptSize);
    if (!f) {
        ERROR_F("Could not load font to slot: {}", file);
        return;
    }
    Renderer::slotFontSizeCache.insert(std::make_pair(f, ptSize));
    Renderer::fontSlot.push_back(f);
}

TTF_Font *Renderer::GetSlotFont(int slot) {
    if (slot >= fontSlot.size()) {
        ERROR_F("Font slot {} does not exists", slot);
        return nullptr;
    }
    return fontSlot[slot];
}

void Renderer::SetSlotFontsize(int slot, int ptSize) {
    if (slot <= fontSlot.size() && fontSlot[slot]) {
        TTF_SetFontSize(font, ptSize);
        slotFontSizeCache[font] = ptSize;
    }
}

int Renderer::GetSlotFontsize(int slot) {
    if (slot <= fontSlot.size() && fontSlot[slot]) {
        return slotFontSizeCache[fontSlot[slot]];
    }
    return -1;
}

void Renderer::SetSlotFontsize(int ptSize) {
    if (Renderer::font) {
        TTF_SetFontSize(font, ptSize);
        slotFontSizeCache[fontSlot[currentFontSlot]] = ptSize;
    }
}

int Renderer::TotalSlotFontsCount() {
    return fontSlot.size();
}

void Renderer::ChangeFont(int slot, const std::string &file, int ptSize) {
    if (slot <= fontSlot.size()) {
        if (fontSlot[slot]) {
            slotFontSizeCache.erase(fontSlot[slot]);
            TTF_CloseFont(fontSlot[slot]);
            fontSlot[slot] = nullptr;
        }
        fontSlot[slot] = TTF_OpenFont(file.c_str(), ptSize);
        if (!fontSlot[slot]) {
            ERROR_F("Could not load font to slot: {}", file);
        }
    }
}

void Renderer::ResetFontSlot() {
    for (auto t : fontSlot) {
        if (t) {
            TTF_CloseFont(t);
        }
    }
    fontSlot.clear();
    slotFontSizeCache.clear();
}

void Renderer::ReleaseSlotFont(int slot) {
    if (slot <= fontSlot.size() && fontSlot[slot]) {
        slotFontSizeCache.erase(fontSlot[slot]);
        delete fontSlot[slot];
        fontSlot[slot] = nullptr;
    }
}

void Renderer::SetSlotFont(int slot) {
    font = Renderer::GetSlotFont(slot);
    currentFontsize = Renderer::GetSlotFontsize(slot);
    currentFontSlot = slot;
}

void Renderer::SetGlobalFont() {
    if (globalFont) {
        TTF_CloseFont(globalFont);
    }
    font = globalFont;
    currentFontsize = globalFontSize;
}

int Renderer::GetCurrentFontSlot() {
    return currentFontSlot;
}


void Renderer::FastGaussianBlur(SDL_Surface *src, SDL_Surface *dst, int radius) {
    SDL_Surface *imgSurf = SDL_CreateRGBSurfaceWithFormat(0, src->w, src->h, src->format->BitsPerPixel, src->format->format);
    SDL_BlitSurface(src, nullptr, imgSurf, nullptr);
    unsigned char *img = (unsigned char *) imgSurf->pixels;
    int width = imgSurf->w;
    int height = imgSurf->h;

    radius = std::min(std::max(1, radius), 248);
    unsigned  int  kernelSize = 1 + radius * 2;
    unsigned  int * kernel = (unsigned  int *) malloc (kernelSize*  sizeof (unsigned  int ));
    memset (kernel, 0, kernelSize*  sizeof (unsigned  int ));
    int (*mult)[256] = ( int (*)[256]) malloc (kernelSize * 256 *  sizeof ( int ));
    memset (mult, 0, kernelSize * 256 *  sizeof ( int ));
    
    int  xStart = 0;
    int  yStart = 0;
    width = xStart + width - std::max(0, (xStart + width) - width);
    height = yStart + height - std::max(0, (yStart + height) - height);
    int  imageSize = width*height;
    int  widthstep = width*4;
    unsigned  char  *    CacheImg =  nullptr ;
    CacheImg = (unsigned  char  *) malloc ( sizeof (unsigned  char ) * imageSize * 6);
    if  (CacheImg ==  nullptr )  return ;
    unsigned  char  *    rCache = CacheImg;
    unsigned  char  *    gCache = CacheImg + imageSize;
    unsigned  char  *    bCache = CacheImg + imageSize * 2;
    unsigned  char  *    r2Cache = CacheImg + imageSize * 3;
    unsigned  char  *    g2Cache = CacheImg + imageSize * 4;
    unsigned  char  *    b2Cache = CacheImg + imageSize * 5;
    int  sum = 0;
    for  ( int  K = 1; K < radius; K++){
        unsigned  int  szi = radius - K;
        kernel[radius + K] = kernel[szi] = szi*szi;
        sum += kernel[szi] + kernel[szi];
        for  ( int  j = 0; j < 256; j++){
            mult[radius + K][j] = mult[szi][j] = kernel[szi] * j;
        }
    }
    kernel[radius] = radius*radius;
    sum += kernel[radius];
    for  ( int  j = 0; j < 256; j++){
        mult[radius][j] = kernel[radius] * j;
    }
    for  ( int  Y = 0; Y < height; ++Y) {
        unsigned  char *     LinePS = img + Y*widthstep;
        unsigned  char *     LinePR = rCache + Y*width;
        unsigned  char *     LinePG = gCache + Y*width;
        unsigned  char *     LinePB = bCache + Y*width;
        for  ( int  X = 0; X < width; ++X) {
            int      p2 = X*4;
            LinePR[X] = LinePS[p2];
            LinePG[X] = LinePS[p2 + 1];
            LinePB[X] = LinePS[p2 + 2];
        }
    }
    int  kernelsum = 0;
    for  ( int  K = 0; K < kernelSize; K++){
        kernelsum += kernel[K];
    }
    float  fkernelsum = 1.0f / kernelsum;
    for  ( int  Y = yStart; Y < height; Y++){
        int  heightStep = Y * width;
        unsigned  char *     LinePR = rCache + heightStep;
        unsigned  char *     LinePG = gCache + heightStep;
        unsigned  char *     LinePB = bCache + heightStep;
        for  ( int  X = xStart; X < width; X++){
            int  cb = 0;
            int  cg = 0;
            int  cr = 0;
            for  ( int  K = 0; K < kernelSize; K++){
                unsigned     int      readPos = ((X - radius + K + width) % width);
                int  * pmult = mult[K];
                cr += pmult[LinePR[readPos]];
                cg += pmult[LinePG[readPos]];
                cb += pmult[LinePB[readPos]];
            }
            unsigned  int  p = heightStep + X;
            r2Cache[p] = cr* fkernelsum;
            g2Cache[p] = cg* fkernelsum;
            b2Cache[p] = cb* fkernelsum;
        }
    }
    for  ( int  X = xStart; X < width; X++){
        int  WidthComp = X*4;
        int  WidthStep = width*4;
        unsigned  char *     LinePS = img + X*4;
        unsigned  char *     LinePR = r2Cache + X;
        unsigned  char *     LinePG = g2Cache + X;
        unsigned  char *     LinePB = b2Cache + X;
        for  ( int  Y = yStart; Y < height; Y++){
            int  cb = 0;
            int  cg = 0;
            int  cr = 0;
            for  ( int  K = 0; K < kernelSize; K++){
                unsigned  int    readPos = ((Y - radius + K + height) % height) * width;
                int  * pmult = mult[K];
                cr += pmult[LinePR[readPos]];
                cg += pmult[LinePG[readPos]];
                cb += pmult[LinePB[readPos]];
            }
            int     p = Y*WidthStep;
            LinePS[p] = (unsigned  char )(cr * fkernelsum);
            LinePS[p + 1] = (unsigned  char )(cg * fkernelsum);
            LinePS[p + 2] = (unsigned  char )(cb* fkernelsum);
        }
    }
    free (CacheImg);
    free (kernel);
    free (mult);

    SDL_Surface *result = SDL_CreateRGBSurfaceWithFormatFrom(img, src->w, src->h, src->format->BitsPerPixel, src->pitch, src->format->format);
    SDL_BlitSurface(result, nullptr, dst, nullptr);
    SDL_FreeSurface(imgSurf);
    SDL_FreeSurface(result);
}

SDL_Surface *Renderer::FastGaussianBlur(SDL_Surface *src, int radius) {
    auto blurred = SDL_CreateRGBSurfaceWithFormat(0, src->w, src->h, src->format->BitsPerPixel, src->format->format);
    Renderer::FastGaussianBlur(src, blurred, radius);
    return blurred;
}

SDL_Surface *Renderer::ScaledFastGaussianBlur(SDL_Surface *src, float ratio, int radius) {
    auto origSize = Vec2(src->w, src->h);
    auto scaledSrc = Renderer::Scale(src, Vec2(src->w * ratio, src->h * ratio));
    auto blurred = Renderer::FastGaussianBlur(scaledSrc, radius);
    SDL_FreeSurface(scaledSrc);
    auto result = Renderer::Scale(blurred, origSize);
    SDL_FreeSurface(blurred);
    return result;
}

Uint64 Renderer::GetTicks() {
    return Renderer::ticks;
}

void Renderer::EnableFPSCounter(float interval) {
    if (Renderer::globalFont) {
        Renderer::showFPSCounter = true;
        Renderer::fpsQueryFreq = interval;
    }
}

void Renderer::DisableFPSCounter() {
    Renderer::showFPSCounter = false;
}


void Renderer::BoxBlur(SDL_Surface *src, SDL_Surface *dst, int radius) {
    int width = src->w;
    int height = src->h;
    int diameter = radius * 2 + 1;

    // Create integral images
    Uint32 *integralR = new Uint32[width * height];
    Uint32 *integralG = new Uint32[width * height];
    Uint32 *integralB = new Uint32[width * height];
    Uint32 *integralA = new Uint32[width * height];

    // Compute integral images
    for (int y = 0; y < height; y++) {
        Uint32 sumR = 0, sumG = 0, sumB = 0, sumA = 0;
        for (int x = 0; x < width; x++) {
            Uint8 r, g, b, a;
            Uint32 pixel = ((Uint32 *)src->pixels)[y * width + x];
            SDL_GetRGBA(pixel, src->format, &r, &g, &b, &a);
            sumR += r;
            sumG += g;
            sumB += b;
            sumA += a;

            if (y == 0) {
                integralR[y * width + x] = sumR;
                integralG[y * width + x] = sumG;
                integralB[y * width + x] = sumB;
                integralA[y * width + x] = sumA;
            } else {
                integralR[y * width + x] = integralR[(y - 1) * width + x] + sumR;
                integralG[y * width + x] = integralG[(y - 1) * width + x] + sumG;
                integralB[y * width + x] = integralB[(y - 1) * width + x] + sumB;
                integralA[y * width + x] = integralA[(y - 1) * width + x] + sumA;
            }
        }
    }

    // Apply box blur using integral images
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int x1 = std::max(0, x - radius);
            int x2 = std::min(width - 1, x + radius);
            int y1 = std::max(0, y - radius);
            int y2 = std::min(height - 1, y + radius);

            int area = (x2 - x1 + 1) * (y2 - y1 + 1);

            Uint32 sumR = integralR[y2 * width + x2] - (x1 > 0 ? integralR[y2 * width + x1 - 1] : 0) - (y1 > 0 ? integralR[(y1 - 1) * width + x2] : 0) + (x1 > 0 && y1 > 0 ? integralR[(y1 - 1) * width + x1 - 1] : 0);
            Uint32 sumG = integralG[y2 * width + x2] - (x1 > 0 ? integralG[y2 * width + x1 - 1] : 0) - (y1 > 0 ? integralG[(y1 - 1) * width + x2] : 0) + (x1 > 0 && y1 > 0 ? integralG[(y1 - 1) * width + x1 - 1] : 0);
            Uint32 sumB = integralB[y2 * width + x2] - (x1 > 0 ? integralB[y2 * width + x1 - 1] : 0) - (y1 > 0 ? integralB[(y1 - 1) * width + x2] : 0) + (x1 > 0 && y1 > 0 ? integralB[(y1 - 1) * width + x1 - 1] : 0);
            Uint32 sumA = integralA[y2 * width + x2] - (x1 > 0 ? integralA[y2 * width + x1 - 1] : 0) - (y1 > 0 ? integralA[(y1 - 1) * width + x2] : 0) + (x1 > 0 && y1 > 0 ? integralA[(y1 - 1) * width + x1 - 1] : 0);

            ((Uint32 *)dst->pixels)[y * width + x] = SDL_MapRGBA(src->format, sumR / area, sumG / area, sumB / area, sumA / area);
        }
    }

    delete[] integralR;
    delete[] integralG;
    delete[] integralB;
    delete[] integralA;
}

SDL_Surface *Renderer::BoxBlur(SDL_Surface *src, int radius) {
    auto dst = SDL_CreateRGBSurfaceWithFormat(0, src->w, src->h, src->format->BitsPerPixel, src->format->format);
    Renderer::BoxBlur(src, dst, radius);
    return dst;
}

void Renderer::EnableProfiling(const std::string &file, float sampleFreq) {
    Renderer::profiling = true;
    Renderer::profileFile = file;
    Renderer::profilingFreq = sampleFreq;
    INFO_F("Profiling enabled with sample frequency: {}", sampleFreq);
}

void Renderer::Sample() {
    auto dt = Renderer::GetDeltatime();
    Renderer::profilingData.push_back(1.0f / dt);
}

void Renderer::EndSample() {
    float runningTime = SDL_GetTicks64() / 1000.0f;
    float freq = Renderer::profilingFreq;
    int dataPointCount = Renderer::profilingData.size();
    const char *title = SDL_GetWindowTitle(Renderer::window);
    std::ofstream file;
    file.open(profileFile, std::ios::binary);
    if (!file.is_open()) {
        ERROR("Failed to write profiling data to file");
        ERROR_F("info: {}", strerror(errno));
        file.close();
        return;
    }
    file.write((char *) &runningTime, sizeof(float));
    file << 0x00;
    file.write((char *) &freq, sizeof(float));
    file << 0x00;
    file.write((char *) &dataPointCount, sizeof(int));
    file << 0x00;

    for (float dataPoint : Renderer::profilingData) {
        file.write((char *) &dataPoint, sizeof(float));
        file << 0x00;
    }
    file.close();
}

// **WARNING**: This is a slow operation!!!   Avoid calling it per frame
SDL_Surface *Renderer::GetRenderBackdrop() {
    Vec2 size = Renderer::GetRenderSize(true);
    Uint32 *pixels = new Uint32[size.x * size.y];
    SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGBA8888, pixels, size.x);
    auto tmp = SDL_CreateRGBSurfaceWithFormatFrom(pixels, size.x, size.y, 32, size.x, SDL_PIXELFORMAT_RGBA8888);
    auto result = SDL_CreateRGBSurfaceWithFormat(0, size.x, size.y, 32, SDL_PIXELFORMAT_RGBA8888);
    SDL_BlitSurface(tmp, nullptr, result, nullptr);
    SDL_FreeSurface(tmp);
    delete pixels;
    return result;
}


Renderer::Surface *Renderer::CreateSurface(const Vec2 &size) {
    return SDL_CreateRGBSurfaceWithFormat(0, (int) size.x, (int) size.y, 32, SDL_PIXELFORMAT_RGBA8888);
}

Renderer::Surface *Renderer::CreateSurfaceWithPixelData(void *pixels, const Vec2 &size) {
    return SDL_CreateRGBSurfaceWithFormatFrom(pixels, (int) size.x, (int) size.y, 32, size.x * 4, SDL_PIXELFORMAT_RGBA8888);
}

void Renderer::UpperBlit(Surface *src, Surface *dst, const Vec2 &pos, const Vec2 &size) {
    SDL_Rect r = Vec2::CreateRect(pos, size);
    SDL_UpperBlit(src, nullptr, dst, &r);
}

void Renderer::UpperBlit(Surface *src, Surface *dst) {
    SDL_UpperBlit(src, nullptr, dst, nullptr);
}

void Renderer::SetSurfaceBlendMode(Surface *s, SurfaceBlendMode mode) {
    SDL_SetSurfaceBlendMode(s, mode);
}

void Renderer::DeleteSurface(Surface *surf) {
    SDL_FreeSurface(surf);
}

void Renderer::FillRectOn(SDL_Surface *dst, const Vec2 &pos, const Vec2 &size, const Color &color) {
    SDL_Rect r = Vec2::CreateRect(pos, size);
    SDL_FillRect(dst, &r, MAP_COLOR(dst->format, color));
}

void Renderer::EnableHUD(float rate) {
    Renderer::hudEnabled = true;
    Renderer::hudQueryFreq = rate;
}

void Renderer::DisableHUD() {
    Renderer::hudQueryFreq = -1.0f;
    Renderer::hudEnabled = false;
}

void Renderer::DebugAddHUD(const std::string &name, ValueRetriver retriver, const Color &color) {
    Renderer::hud.insert(std::make_pair(name, std::make_pair(retriver, color)));
}
