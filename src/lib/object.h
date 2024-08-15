#pragma once
#include "utils.hpp"
#include "ecs.h"
#include "render.h"


namespace chem_war {
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
        
        void Update();
        void Render();

        virtual ~GameObject() = default;
    
    protected:
        ecs::World &world;
        std::string id;
        GameObject *parent {};
        std::vector<GameObject *> childrens {};
        ecs::Entity entityId;
    };
}
