#pragma once
#include "utils.hpp"
#include "ecs.h"
#include "render.h"


namespace engine {
    class GameObjectBase {
    public:
        GameObjectBase(const std::string &id);

        void AddChildren(GameObjectBase *go);
        void RemoveChildrens();
        void SetParent(GameObjectBase *go);
        std::vector<GameObjectBase *> GetChildrens();
        std::string GetId() const;

        virtual void Update(float dt);
        virtual void Render();

    protected:
        std::string id;
        GameObjectBase *parent {};
        std::vector<GameObjectBase *> childrens;
    };

    struct GUIWindow;
    class UIBase : public GameObjectBase {
    public:
        UIBase(const std::string &id);
        virtual void Translate(const Vec2 &pos);
        virtual void Move(const Vec2 &delta);
        virtual void SetOpacity(int a);
        Vec2 GetPos();
        int GetOpacity();

        virtual ~UIBase() = default;

    protected:
        Vec2 pos;
        int opacity;
        GUIWindow *window;
    };

    using WidgetID = unsigned int;

    struct GUIWindow {
        Vec2 pos;
        Vec2 size;
        // SDL_Surface *windowSurface;
        std::string title;
        bool shown;
        std::vector<WidgetID> widgets;
        // Not implemented yet
        int flags;

        // state flags
        bool dragging = false;
        bool scaling = false;
        bool focused = false;
    };

    class GameObject : public GameObjectBase {
    public:
        GameObject(const std::string &id, ecs::World &world, ecs::Entity entityId = ecs::SparseSet<ecs::Entity, 32>::null);        
        
        template<typename T>
        T &GetComponent() {
            if (entityId == ecs::SparseSet<ecs::Entity, 32>::null) {
                assert(false && "Not a valid ECS object");
            }
            return ecs::Querier(this->world).Get<T>(this->entityId);
        }

        template<typename T>
        bool HasComponent() const {
            if (entityId == ecs::SparseSet<ecs::Entity, 32>::null) {
                assert(false && "Not a valid ECS object");
            }
            return ecs::Querier(this->world).Has<T>(this->entityId);
        }
        
        ecs::Entity GetEntity();
        ecs::World &GetWorld();

        virtual ~GameObject();
    
    protected:
        ecs::World &world;
        ecs::Entity entityId;
    };

    template<const char *Prefix>
    struct ObjectIDGenerator {
        inline static unsigned long current = {};
        static inline std::string Next() {
            static unsigned long id = ++current;
            return std::format("{}_{}", Prefix, id);
        }
    };
}
