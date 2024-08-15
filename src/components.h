#pragma once
#include "lib/ecs.h"
#include "lib/render.h"


namespace chem_war {
    namespace components {
        struct Movement {
            Vec2 velocity;
            Vec2 pos;
        };

        struct Texture2D {
            Renderer::Texture t;
            Vec2 renderPos;
        };

        struct BasicGraph {
            std::vector<std::string> drawCalls;
        };

        struct Graph {
            bool keepColor;
            int graphType;
            Vec2 p1;
            Vec2 p2;
            Uint8 r;
            Uint8 g;
            Uint8 b;
            Uint8 a;
        };

        void MovementSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void Texture2DRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void BasicGraphRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void GraphRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
    }
}