#include "object.h"


chem_war::GameObject::GameObject(const std::string &id, ecs::World &world, ecs::Entity entityId) : id(id), world(world) {
    this->entityId = entityId;
}

std::string chem_war::GameObject::GetId() const {
    return this->id;
}

void chem_war::GameObject::AddChildren(GameObject *go) {
    this->childrens.push_back(go);
    go->SetParent(this);
}

void chem_war::GameObject::RemoveChildrens() {
    this->childrens.clear();
}

void chem_war::GameObject::Update(float dt) {
    for (auto &&children : this->childrens) {
        children->Update(dt);
    }
}

void chem_war::GameObject::Render() {
    for (auto &&children : this->childrens) {
        children->Render();
    }
}

void chem_war::GameObject::SetParent(GameObject *go) {
    this->parent = go;
}

chem_war::GameObject::~GameObject() {
    ecs::Commands cmd(this->world);
    if (this->entityId != ecs::SparseSet<ecs::Entity, 32>::null) {
        cmd.Destroy(this->entityId);    
    }
    cmd.Execute();
}
