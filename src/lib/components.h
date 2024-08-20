#pragma once
#include "ecs.h"
#include "render.h"


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
            bool visible = true;
            bool keepColor;
            int graphType;
            Vec2 p1;
            Vec2 p2;
            Uint8 r;
            Uint8 g;
            Uint8 b;
            Uint8 a;
        };

        struct BasicText {
            std::vector<std::string> lines;
            int margin;
            Uint8 r;
            Uint8 g;
            Uint8 b;
            Uint8 a;
            Vec2 dpos;
        };

        struct SimpleSwitch {
            bool status;
            int type;
            Uint8 val;
        };

        struct SimpleCollider2D {
            std::string tag;
            bool showCollider = false;
            Vec2 pos;
            Vec2 size;
            std::function<void(const std::string &, ecs::Entity, const std::string &, ecs::Entity)> onCollide;
        };

        void MovementSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void Texture2DRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void BasicGraphRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void GraphRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void BasicTextRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void SimpleSwitchSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void SimpleCollider2DSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
    }
}