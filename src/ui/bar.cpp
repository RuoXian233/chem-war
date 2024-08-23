#include "bar.h"
#include "../lib/components.h"


using namespace engine::ui;

Bar::Bar(const std::string &name, engine::ecs::World &world, const Vec2 &pos) : GameObject("ui.bar." + name, world) {
    ecs::Commands cmd(this->world);
    this->entityId = cmd.Spawned<components::Movement, components::Texture2D, components::SceneAssosication>(
        components::Movement { Vec2(), pos },
        components::Texture2D { 
            Renderer::Texture { nullptr, Vec2() },
            Vec2()
        },
        components::SceneAssosication { "game" }
    );
    cmd.Execute();
}


void Bar::Config(int margin, const SDL_Color &inner, const SDL_Color &outer, const Vec2 &size) {
    auto &comp = this->GetComponent<components::Texture2D>();
    comp.t = Renderer::CreateRenderContext(size);
    comp.t.size = size;
    this->outerColor = outer;
    this->innerColor = inner;
    this->margin = margin;
    this->renderContext = comp.t;
}

void Bar::Update(float dt) {
    this->percentage = this->retriever();
}

void Bar::Render() {
    Renderer::SetRenderContext(this->renderContext);
    Renderer::SetDrawColor(this->outerColor);
    Renderer::FillRect(Vec2(), this->renderContext.size);
    Renderer::SetDrawColor(this->innerColor);
    Renderer::FillRect(Vec2(margin, margin), Vec2(this->renderContext.size.x * this->percentage - margin * 2, this->renderContext.size.y - margin * 2));
    Renderer::ClearDrawColor();
    Renderer::ClearRenderContext();
}

void Bar::AttachValue(std::function<float()> retriever) {
    this->retriever = retriever;
}

Bar::~Bar() {
    Renderer::DeleteRenderContext(this->renderContext);
}
