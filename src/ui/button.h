#pragma once
#include "../lib/object.h"
#include "../preset/gui.h"


namespace engine::ui {
    enum class ButtonSizingType {
        Fixed, Expand
    };

    struct Button {
        std::string text;
        Color fg, bg;
        ButtonSizingType sizingType;
        int horizonalMargin = 0, verticalMargin = 0;
        Vec2 size;
        int cornerRadius = 0;
        int borderWidth = -1;
        Color borderColor;
        int fontSlot = -1;
    };

    engine::preset::ButtonInterationType UI_Button(Button *btn, const Vec2 &pos);
}
