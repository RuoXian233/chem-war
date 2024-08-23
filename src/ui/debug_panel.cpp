#include "debug_panel.h"
#include "../lib/components.h"

using namespace engine::ui;


DebugPanel::DebugPanel(engine::ecs::World &world, const Vec2 &pos) : 
    engine::GameObject(DebugPanel::DEFUALT_DEBUG_PANEL_ID, world) {
        engine::ecs::Commands cmd(this->world);
        this->entityId = cmd.Spawned<components::BasicText, components::SimpleTimer>(
            components::BasicText { 
                {},
                4,
                255, 255, 255, 255,
                pos
            },
            components::SimpleTimer {}
        );
        cmd.Execute();

        // this->GetComponent<components::SimpleTimer>().Configure(10, 1000, [](ecs::Entity) { std::cout << "Debug Panel ticked" << std::endl; });
}

void DebugPanel::AddItem(const std::string &name, NumericValRetriever n) {
    this->numericVals.insert(std::make_pair(name, n));
}

void DebugPanel::AddItem(const std::string &name, StringValRetriever r) {
    this->stringVals.insert(std::make_pair(name, r));
}

void DebugPanel::Render() {}

void DebugPanel::Update(float dt) {
    auto &textView = this->GetComponent<components::BasicText>();
    if (!textView.lines.empty()) {
        textView.lines.clear();
    }

    if (this->show) {
        for (const auto [name, r] : this->numericVals) {
            textView.lines.push_back(std::format("{}: {}", name, r()));
        }
        for (const auto [name, r] : this->stringVals) {
            textView.lines.push_back(std::format("{}: {}", name, r()));
        }
    }
}

bool DebugPanel::IsHide() { return this->show; }
void DebugPanel::Hide(bool state) { this->show = state; }

DebugPanel::~DebugPanel() {}
