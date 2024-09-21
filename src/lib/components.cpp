#include "components.h"
#include "scene.h"


void engine::components::MovementSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    for (auto entity : q.Query<Movement>()) {
        if (q.Has<SceneAssosication>(entity)) {
            auto scene = q.Get<SceneAssosication>(entity).sceneName;
            if (SceneManager::GetCurrentSceneName() != scene) {
                continue;
            }
        }

        auto &movementComp = q.Get<Movement>(entity);
        movementComp.pos += movementComp.velocity * Renderer::GetDeltatime();
    }
}

void engine::components::Texture2DRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    for (auto entity : q.Query<Texture2D>()) {
        if (q.Has<SceneAssosication>(entity)) {
            auto scene = q.Get<SceneAssosication>(entity).sceneName;
            if (SceneManager::GetCurrentSceneName() != scene) {
                continue;
            }
        }

        auto comp = q.Get<Texture2D>(entity);
        if (!q.Has<Movement>(entity)) {
            Renderer::RenderTexture(comp.t, comp.renderPos);
        } else {
            Renderer::RenderTexture(comp.t, q.Get<Movement>(entity).pos);
        }
    }
}

void engine::components::BasicGraphRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    for (auto entity : q.Query<BasicGraph>()) {
        if (q.Has<SceneAssosication>(entity)) {
            auto scene = q.Get<SceneAssosication>(entity).sceneName;
            if (SceneManager::GetCurrentSceneName() != scene) {
                continue;
            }
        }

        auto comp = q.Get<BasicGraph>(entity);
        for (auto drawCall : comp.drawCalls) {
            // Not implemented yet
        }
    }
}

void engine::components::GraphRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    for (auto entity : q.Query<Graph>()) {
        if (q.Has<SceneAssosication>(entity)) {
            auto scene = q.Get<SceneAssosication>(entity).sceneName;
            if (SceneManager::GetCurrentSceneName() != scene) {
                continue;
            }
        }

        auto comp = q.Get<Graph>(entity);
        if (comp.visible) {
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
            case 2: {
                if (!comp.keepColor) {
                    Renderer::SetDrawColor(comp.r, comp.g, comp.b, comp.a);
                }
                auto pos = comp.p1;
                if (q.Has<Movement>(entity)) {
                    pos = q.Get<Movement>(entity).pos;
                }
                Renderer::DrawRect(pos, comp.p2);
                Renderer::ClearDrawColor();
                break;
            }
            default:
                break;
            }
        }
    }
}

void engine::components::BasicTextRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    auto font = Renderer::GetFont();
    for (auto entity : q.Query<BasicText>()) {
        if (q.Has<SceneAssosication>(entity)) {
            auto scene = q.Get<SceneAssosication>(entity).sceneName;
            if (SceneManager::GetCurrentSceneName() != scene) {
                continue;
            }
        }

        auto comp = q.Get<BasicText>(entity);
        Renderer::SetDrawColor(comp.r, comp.g, comp.b, comp.a);
        auto pos = comp.dpos;
        if (q.Has<Movement>(entity)) {
            pos = q.Get<Movement>(entity).pos;
        }
        std::vector<Renderer::Texture> cache;
        for (const auto &text : comp.lines) {
            cache.push_back(Renderer::Text(text));
        }

        int index = 0;
        for (auto && c : cache) {
            int _, h;
            TTF_SizeUTF8(font, comp.lines[index].c_str(), &_, &h);
            Renderer::RenderTexture(c, pos);
            pos.y += h + comp.margin;
            index++;
        }
        
        for (auto &v : cache) {
            SDL_DestroyTexture(v.textureData);
        }
        Renderer::ClearDrawColor();
    }
}

void engine::components::SimpleSwitchSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    for (auto entity : q.Query<components::SimpleSwitch>()) {
        // TODO: Impl
    }
}

void engine::components::SimpleCollider2DSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    std::vector<QuadTree::ObjectWithRect> collidingEntities;
    std::vector<QuadTree::ObjectWithRect> filtedEntities;
    QuadTree qt(0, Vec2(0, 0), Vec2(WINDOW_WIDTH, WINDOW_HEIGHT));

    // Filter some entities that must not be colliding
    for (auto entity : q.Query<SimpleCollider2D>()) {
        if (q.Has<SceneAssosication>(entity)) {
            auto scene = q.Get<SceneAssosication>(entity).sceneName;
            if (SceneManager::GetCurrentSceneName() != scene) {
                continue;
            }
        }

        auto pos = q.Get<Movement>(entity).pos;
        if (q.Has<Movement>(entity)) {
            pos = q.Get<Movement>(entity).pos;
        }
        auto comp = q.Get<SimpleCollider2D>(entity);

        if (comp.showCollider) {
            Renderer::SetDrawColor(0, 255, 255, 255);
            Renderer::DrawRect(pos, comp.size);
            Renderer::ClearDrawColor();
        }
        collidingEntities.push_back(QuadTree::ObjectWithRect { comp.tag, reinterpret_cast<void *>(entity), pos, comp.size });
    }

    // for (auto e : collidingEntities) {
    //     qt.Retrieve(filtedEntities, e.pos, e.size);
    // }

    // Check for collisions
    #ifdef __linux__
        using NType = long;
    #else
        using NType = long long;
    #endif

    for (auto entity : collidingEntities) {
        auto r1 = Vec2::CreateFRect(entity.pos, entity.size);
        for (auto item : collidingEntities) {
            if (item.object == entity.object) {
                continue;
            }
            auto id = (ecs::Entity) ((NType) entity.object);
            auto collideId = (ecs::Entity) ((NType) item.object);

            auto CollisionHandler = q.Get<SimpleCollider2D>(id).onCollide;
            
            auto r2 = Vec2::CreateFRect(item.pos, item.size);
            auto r3 = Vec2::CreateFRect(Vec2(), Vec2());
            if (SDL_IntersectFRect(&r1, &r2, &r3)) {
                CollisionHandler(entity.id, id, item.id, collideId);
            }
        }
    }
}

void engine::components::SimpleTimerSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    for (auto entity : q.Query<SimpleTimer>()) {
        if (q.Has<SceneAssosication>(entity)) {
            if (q.Get<SceneAssosication>(entity).sceneName != SceneManager::GetCurrentSceneName()) {
                continue;
            }
        }
        auto dt = Renderer::GetDeltatime();
        auto &timer = q.Get<SimpleTimer>(entity);
        if (timer.isActivated && timer.shots < timer.maxShots) {
            timer.current += dt;
            if ((int) (timer.current * 1000) > timer.duration) {
                timer.current = 0;
                timer.callback(entity);
                timer.shots++;
            }
        }
    }
}

void engine::components::LabelTextRenderSystem(ecs::Commands &commander, ecs::Querier q, ecs::Resources r, ecs::Events &e) {
    for (auto entity : q.Query<LabelText>()) {
        if (q.Has<SceneAssosication>(entity)) {
            if (q.Get<SceneAssosication>(entity).sceneName != SceneManager::GetCurrentSceneName()) {
                continue;
            }
        }

        Vec2 pos;
        const auto &textComp = q.Get<LabelText>(entity);
        if (q.Has<Movement>(entity)) {
            pos = q.Get<Movement>(entity).pos;
        } else {
            pos = textComp.dPos;
        }
        int origFontSize = Renderer::GetCurrentFontsize();
        Renderer::ChangeFontSize(textComp.fontsize);
        SDL_Point p;
        TTF_SizeUTF8(Renderer::GetFont(), textComp.text.c_str(), &p.x, &p.y);
        Vec2 size(p.x, p.y);

        Renderer::SetDrawColor(textComp.bg);
        Renderer::FillRect(pos, size);
        auto t = Renderer::Text(textComp.text, textComp.fg);
        Renderer::RenderTexture(t, pos);
        Renderer::DeleteRenderContext(t);
        Renderer::ClearDrawColor();
        Renderer::ChangeFontSize(origFontSize);
    }
}
