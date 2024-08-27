#include "object.h"


engine::GameObject::GameObject(const std::string &id, ecs::World &world, ecs::Entity entityId) : id(id), world(world) {
    this->entityId = entityId;
}

std::string engine::GameObject::GetId() const {
    return this->id;
}

void engine::GameObject::AddChildren(GameObject *go) {
    this->childrens.push_back(go);
    go->SetParent(this);
}

void engine::GameObject::RemoveChildrens() {
    this->childrens.clear();
}

std::vector<engine::GameObject *> engine::GameObject::GetChildrens() {
    return this->childrens;
}

void engine::GameObject::Update(float dt) {
    for (auto &&children : this->childrens) {
        children->Update(dt);
    }
}

void engine::GameObject::Render() {
    for (auto &&children : this->childrens) {
        children->Render();
    }
}

void engine::GameObject::SetParent(GameObject *go) {
    this->parent = go;
}

engine::GameObject::~GameObject() {
    ecs::Commands cmd(this->world);
    if (this->entityId != ecs::SparseSet<ecs::Entity, 32>::null) {
        cmd.Destroy(this->entityId);    
    }
    cmd.Execute();
}
