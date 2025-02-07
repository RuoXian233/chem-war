#include "label.h"

using namespace engine::ui;


void engine::ui::UI_Label(GUIWindow *window, Label *lb, const Vec2 &pos) {
    TTF_Font *f;
    int fontsize;
    if (lb->fontslot != -1) {
        f = Renderer::GetSlotFont(lb->fontslot);
        fontsize = Renderer::GetSlotFontsize(lb->fontslot);
        if (fontsize != lb->fontsize) {
            Renderer::SetSlotFontsize(lb->fontslot, lb->fontsize);
        }
    } else {
        f = Renderer::GetGlobalFont();
        fontsize = Renderer::GetGlobalFontsize();
        if (fontsize != lb->fontsize) {
            Renderer::ChangeFontSize(lb->fontsize);
        }
    }
    
    auto t = Renderer::Text(lb->text, lb->fg);
    Renderer::RenderTexture(t, window->pos + pos);
    Renderer::DeleteRenderContext(t);

    if (lb->fontslot != -1) {
        Renderer::SetSlotFontsize(lb->fontslot, fontsize);
    } else {
        Renderer::ChangeFontSize(lb->fontsize);
    }
}
