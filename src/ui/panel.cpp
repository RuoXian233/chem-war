#include "panel.h"
#include <Windows.h>
#ifdef _WIN32
    #include <ShellScalingApi.h>
    #pragma comment(lib, "Shcore.lib")
#endif


using namespace engine::ui;


Panel::Panel(const std::string &id, const Vec2 &pos, const Vec2 &size, const Color &color)
    : UIBase(id), color(color) {
    this->Translate(pos);
    this->size = size;
    this->SetCornerRadius(0);
    this->blurredSurface = nullptr;
    this->hasBorder = false;
}

Panel::~Panel() {
    if (this->blurredSurface) {
        SDL_FreeSurface(this->blurredSurface);
    }
}

// [[deprecated]] void Panel::Update(float dt) {}

void Panel::Render() {
    if (this->hasBorder) {
        Renderer::SetDrawColor(this->borderColor);
        if (this->cornerRadius > 0) {
            Renderer::FillRoundRect(this->GetPos() - Vec2(this->borderWidth, this->borderWidth), this->size + Vec2(this->borderWidth * 2, this->borderWidth * 2), this->cornerRadius);
        } else {
            Renderer::FillRect(this->GetPos() - Vec2(this->borderWidth, this->borderWidth), this->size + Vec2(this->borderWidth * 2, this->borderWidth * 2));

        }
        Renderer::ClearDrawColor();
    }

    if (this->cornerRadius == 0 && !this->blurredSurface) {
        Renderer::SetDrawColor(this->color);
        Renderer::FillRect(this->GetPos(), this->size);
        Renderer::ClearDrawColor();
        return;
    } else if (this->cornerRadius != 0 && !this->blurredSurface) {
        Renderer::SetDrawColor(this->color);
        Renderer::FillRoundRect(this->GetPos(), this->size, this->cornerRadius);
        Renderer::ClearDrawColor();
        return;
    } else {
        auto mask = Renderer::CreateSurface(this->size);
        auto dst = Renderer::CreateSurface(this->size);
        auto cliped = Renderer::ClipCopy(this->blurredSurface, this->GetPos(), this->size);
        Renderer::UpperBlit(cliped, dst);
        Renderer::SetSurfaceBlendMode(mask, SDL_BLENDMODE_MOD);
        SDL_Rect r = { 0, 0, (int) this->size.x, (int) this->size.y };

        if (this->blurredSurface && this->cornerRadius == 0) {
            Renderer::FillRectOn(mask, Vec2(), this->size, this->color);
        } else {
            Renderer::FillRoundRectOn(mask, {0, 0}, this->size, this->cornerRadius, this->color);
        }
        Renderer::UpperBlit(mask, dst);
        Renderer::ApplyColorKey(dst, { 0, 0, 0, 0 });
        auto current = Renderer::CreateTexture(dst);
        Renderer::RenderTexture(current, this->GetPos());

        Renderer::DeleteSurface(cliped);
        Renderer::DeleteSurface(mask);
        Renderer::DeleteSurface(dst);
        Renderer::DeleteRenderContext(current);
    }
}

void Panel::SetColor(const Color &color) {
    this->color = color;
}

void Panel::SetCornerRadius(int radius) {
    this->cornerRadius = radius;
}

void Panel::ApplyStaticBlur(SDL_Surface *src, int radius, bool scale) {
    if (this->blurredSurface) {
        Renderer::DeleteSurface(this->blurredSurface);
    }
    SDL_Surface *surf;
    if (scale) {
        // #ifndef _WIN32
        //     #error "DPI scaling is not supported on this platform"
        // #endif
        // HWND hwnd = GetActiveWindow();
        // UINT dpi = GetDpiForWindow(hwnd);
        // float ratio = dpi / 96.0f;
        surf = Renderer::Scale(src, Renderer::GetRenderSize());   
    } else {
        surf = Renderer::CreateSurface(this->size);
        Renderer::UpperBlit(src, surf);
    }
    
    // this->blurredSurface = surf;
    this->blurredSurface = Renderer::GaussianBlur(surf, radius);
    Renderer::DeleteSurface(surf);
}

void engine::ui::Panel::SetOpacity(int a) {
    this->color.a = a;
}

void Panel::SetBorder(int width, const Color &color) {
    this->hasBorder = true;
    this->borderWidth = width;
    this->borderColor = color;
}

void Panel::ClearBorder() {
    this->hasBorder = false;
}

engine::Vec2 Panel::GetSize() {
    return this->size;
}
