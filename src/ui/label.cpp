#include "label.h"
#include "../lib/components.h"

using namespace engine;


ui::Label::Label(const std::string &id, const std::string &text, ecs::World &world) : GameObject(id, world) {
    this->text = text;
    ecs::Commands command(this->world);
    this->entityId = command.Spawned<components::Movement, components::LabelText>(
        components::Movement { Vec2(), Vec2() },
        components::LabelText {
            this->text, 0
        }
    );
    command.Execute();
    this->Config(Renderer::GetCurrentFontsize(), Colors::White);
}

void ui::Label::Config(int fontsize, Color fg, Color bg) {
    this->fontsize = fontsize;
    this->fg = fg;
    this->bg = bg;

    auto &textComp = this->GetComponent<components::LabelText>();
    textComp.text = this->text;
    textComp.fg = fg;
    textComp.bg = bg;
    textComp.fontsize = fontsize;
}


ui::Label::~Label() {}


std::string ui::Label::GetText() {
    return this->text;
}

void ui::Label::SetText(const std::string &text) {
    this->text = text;
    this->GetComponent<components::LabelText>().text = text;
}
