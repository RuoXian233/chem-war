#include "jumping_text.h"
#include "../lib/components.h"

using namespace engine::preset;

std::vector<JumpingText *> JumpingTextManager::textCache;
unsigned long long JumpingText::id = 0;


JumpingText::JumpingText(ecs::World &world, const Vec2 &pos, const std::string &text, const Color &color, int fontsize) {
    this->view = new ui::Label(std::format("  _jumping_text_{}", JumpingText::id), text, world);
    this->view->Config(fontsize, color);
    this->c = color;
    this->view->GetComponent<engine::components::Movement>().pos = pos;
    auto &cfg = this->view->GetComponent<engine::components::LabelText>();
    cfg.useColorKey = true;
    cfg.key = Colors::Black;
    id++;
}

void JumpingText::Update(float dt) {
    if (this->alpha > 0 && start && this->view) {
        this->alpha -= this->speedA;
        this->c.a = this->alpha;
        this->view->GetComponent<engine::components::Movement>().pos += Vec2(0, -this->speed * dt);
        this->view->GetComponent<engine::components::LabelText>().fg.a = this->alpha;
    }
    if (this->alpha > 0) {
        if (this->view) {
            this->view->Update(dt);
        }
    } else {
        this->Clear();
    }
}

void JumpingText::Render() {
    if (this->alpha > 0) {
        this->view->Render();
    }
}

void JumpingText::Play() {
    this->start = true;
}

void JumpingText::Clear() {
    if (this->view) {
        ecs::Commands cmd(this->view->GetWorld());
        cmd.Destroy(this->view->GetEntity());
        cmd.Execute();
        delete this->view;
        this->view = nullptr;
    }
}

JumpingText::~JumpingText() {
    this->Clear();
}

void JumpingTextManager::Add(JumpingText *t) {
    t->Play();
    JumpingTextManager::textCache.push_back(t);
}

void JumpingTextManager::Update(float dt) {
    for (int i = 0; i < JumpingTextManager::textCache.size(); i++) {
        if (!JumpingTextManager::textCache[i]->start || !JumpingTextManager::textCache[i]->view) {
            JumpingTextManager::textCache[i]->Clear();
            delete JumpingTextManager::textCache[i];
            JumpingTextManager::textCache.erase(JumpingTextManager::textCache.begin() + i);
        }
    }

    for (auto t : JumpingTextManager::textCache) {
        t->Update(dt);
    }
}

void JumpingTextManager::Render() {
    for (auto t : JumpingTextManager::textCache) {
        t->Render();
    }
}
