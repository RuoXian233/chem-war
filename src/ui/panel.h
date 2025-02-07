#include "../lib/render.h"
#include "../lib/object.h"


namespace engine::ui {
    class Panel : public UIBase {
    public:
        using BlurProvider = std::function<SDL_Surface *(int, bool)>;
        Panel(const std::string &id, const Vec2 &pos, const Vec2 &size, const Color &color);
        // virtual void Update(float dt) override;
        virtual void Render() override;
        void SetColor(const Color &color);
        void SetCornerRadius(int radius);
        void SetBorder(int width, const Color &color);
        void ClearBorder();
        void ApplyStaticBlur(SDL_Surface *src, int radius, bool scale = false);
        void SetOpacity(int a) override;
        // void ApplyDynamicBlur(BlurProvider provider, int radius, bool quality = false);
        Vec2 GetSize();
        ~Panel();
    
    protected:
        Vec2 size;
        Color color;
        int cornerRadius;
        SDL_Surface *blurredSurface;
        bool hasBorder;
        int borderWidth;
        Color borderColor;
    };
}
