#include "object.h"


engine::GameObjectBase::GameObjectBase(const std::string &id) {
    this->id = id;
}

engine::UIBase::UIBase(const std::string &id) : GameObjectBase(id), window(nullptr) {}

void engine::UIBase::Translate(const Vec2 &pos) {
    this->pos = pos;
}

void engine::UIBase::Move(const Vec2 &delta) {
    this->pos += delta;
}

void engine::UIBase::SetOpacity(int a) {
    this->opacity = a;
}

engine::GameObject::GameObject(const std::string &id, ecs::World &world, ecs::Entity entityId) : GameObjectBase(id), world(world) {
    this->entityId = entityId;
}

std::string engine::GameObjectBase::GetId() const {
    return this->id;
}

void engine::GameObjectBase::AddChildren(GameObjectBase *go) {
    this->childrens.push_back(go);
    go->SetParent(this);
}

void engine::GameObjectBase::RemoveChildrens() {
    this->childrens.clear();
}

int engine::UIBase::GetOpacity() {
    return this->opacity;
}

engine::Vec2 engine::UIBase::GetPos() {
    return this->pos;
}

std::vector<engine::GameObjectBase *> engine::GameObjectBase::GetChildrens() {
    return this->childrens;
}

void engine::GameObjectBase::Update(float dt) {
    for (auto &&children : this->childrens) {
        children->Update(dt);
    }
}

void engine::GameObjectBase::Render() {
    for (auto &&children : this->childrens) {
        children->Render();
    }
}

[[deprecated]] void engine::GameObjectBase::SetParent(GameObjectBase *go) {
    this->parent = go;
    // go->AddChildren(this);
}

engine::GameObject::~GameObject() {
    ecs::Commands cmd(this->world);
    if (this->entityId != ecs::SparseSet<ecs::Entity, 32>::null) {
        cmd.Destroy(this->entityId);    
    }
    cmd.Execute();
}

engine::ecs::Entity engine::GameObject::GetEntity() {
    return this->entityId;
}

engine::ecs::World &engine::GameObject::GetWorld() {
    return this->world;
}
