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

void chem_war::GameObject::Update() {
    for (auto &&children : this->childrens) {
        children->Update();
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
