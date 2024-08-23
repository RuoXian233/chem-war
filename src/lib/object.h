#pragma once
#include "utils.hpp"
#include "ecs.h"
#include "render.h"


namespace engine {
    class GameObject {
    public:
        GameObject(const std::string &id, ecs::World &world, ecs::Entity entityId = ecs::SparseSet<ecs::Entity, 32>::null);

        std::string GetId() const;

        void AddChildren(GameObject *go);
        void RemoveChildrens();
        void SetParent(GameObject *go);
        
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
        
        virtual void Update(float dt);
        virtual void Render();

        virtual ~GameObject();
    
    protected:
        ecs::World &world;
        std::string id;
        GameObject *parent {};
        std::vector<GameObject *> childrens {};
        ecs::Entity entityId;
    };
}
