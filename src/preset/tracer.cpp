#include "tracer.h"
#include "../lib/render.h"

using namespace engine::preset;


Tracer::Tracer(const Vec2 &initalPos) {
    this->pos = initalPos;
}

Tracer::Tracer() : engine::preset::Tracer(Vec2()) {} 

void Tracer::Trace(const Vec2 &target) {
    this->targetPos = target;
    this->velocity = target - pos;
}

void Tracer::Update(float dt) {
    auto dpos = this->velocity * dt * speed;
    this->pos += dpos;
}

void Tracer::RenderHint() {
    engine::Renderer::Line(this->pos + this->Size() / 2, this->targetPos, Colors::Lavender);
    auto t1 = engine::Renderer::Text("Target: " + this->targetPos.ToString(), Colors::Green);
    auto t2 = engine::Renderer::Text("Distance: " + std::format("{}", this->GetDistance()), Colors::Aqua);
    auto t3 = engine::Renderer::Text("Velocity: " + this->Velocity().ToString(), Colors::Gold);
    engine::Renderer::RenderTexture(t1, this->pos - Vec2(30, 40));
    engine::Renderer::RenderTexture(t2, this->pos - Vec2(30, 60));
    engine::Renderer::RenderTexture(t3, this->pos - Vec2(30, 80));
    engine::Renderer::SetDrawColor(Colors::Red);
    engine::Renderer::DrawCircle(this->targetPos, 3);
    engine::Renderer::ClearDrawColor();
    engine::Renderer::DeleteRenderContext(t1);
    engine::Renderer::DeleteRenderContext(t2);
    engine::Renderer::DeleteRenderContext(t3);
}
