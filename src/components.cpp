#include "components.h"


void chem_war::components::MovementSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    for (auto entity : q.Query<Movement>()) {
        auto &movementComp = q.Get<Movement>(entity);
        movementComp.pos += movementComp.velocity * Renderer::GetDeltatime();
    }
}

void chem_war::components::Texture2DRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    for (auto entity : q.Query<Texture2D>()) {
        auto comp = q.Get<Texture2D>(entity);
        Renderer::RenderTexture(comp.t, comp.renderPos);
    }
}

void chem_war::components::BasicGraphRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    for (auto entity : q.Query<BasicGraph>()) {
        auto comp = q.Get<BasicGraph>(entity);
        for (auto drawCall : comp.drawCalls) {
            // Not implemented yet
        }
    }
}

void chem_war::components::GraphRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    for (auto entity : q.Query<Graph>()) {
        auto comp = q.Get<Graph>(entity);
        switch (comp.graphType) {
        case 1: {
            if (!comp.keepColor) {
                Renderer::SetDrawColor(comp.r, comp.g, comp.b, comp.a);
            }
            auto pos = comp.p1;
            if (q.Has<Movement>(entity)) {
                pos = q.Get<Movement>(entity).pos;
            }
            Renderer::FillRect(pos, comp.p2);
            Renderer::ClearDrawColor();
            break;
        }
        default:
            break;
        }
    }
}
