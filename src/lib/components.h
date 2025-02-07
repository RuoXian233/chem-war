#pragma once
#include "ecs.h"
#include "render.h"


namespace engine {
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
            // Vec2 pos;
            Vec2 size;
            std::function<void(const std::string &, ecs::Entity, const std::string &, ecs::Entity)> onCollide;
        };

        struct SimpleTimer {
            int shots = 0;
            int maxShots;
            bool isActivated = true;
            float duration;
            float current = 0;
            std::function<void(ecs::Entity)> callback;

            inline void Configure(int maxShots, float duration, std::function<void(ecs::Entity)> callback) {
                this->maxShots = maxShots;
                this->duration = duration;
                this->callback = callback;
            }
        
            inline void Clear() {
                this->shots = 0;
                this->isActivated = true;
                this->current = 0;
            }
        };

        struct SceneAssosication {
            std::string sceneName;
        };

        struct IndivisualTimer {};

        struct LabelText {
            std::string text;
            int fontsize;
            Color fg;
            Color bg;
            bool useColorKey;
            Color key;
            Vec2 dPos;
            Vec2 size;
        };

        void MovementSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void Texture2DRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void BasicGraphRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void GraphRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void BasicTextRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void SimpleSwitchSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void SimpleCollider2DSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void SimpleTimerSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
        void LabelTextRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e);
    }
}