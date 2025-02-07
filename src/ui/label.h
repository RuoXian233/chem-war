#pragma once
#include "../lib/object.h"


namespace engine::ui {
    struct Label {
        std::string text;
        int fontsize;
        Color fg, bg;
        int fontslot = -1;
    };

    void UI_Label(GUIWindow *window, Label *lb, const Vec2 &pos);
}
