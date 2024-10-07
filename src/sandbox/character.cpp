#include "character.h"
#include "../lib/resource.h"

using namespace chem_war;
Character *Character::sCharacter;

using namespace engine;


Character *Character::Instance() {
    if (!Character::sCharacter) {
        Character::sCharacter = new Character();
        Character::sCharacter->texture = "character.figure";
    }
    return Character::sCharacter;
}

void Character::Destroy() {
    if (Character::sCharacter) {
        delete Character::sCharacter;
    }
}


void Character::Prepare() {
    this->controller.SetPos(Vec2(100, 100));
    auto data = ResourceManager::Get(this->texture)->GetAs<SDL_Surface>();
    auto t = Renderer::CreateTexture(data);
    this->size = t.size * 0.16;
    ResourceManager::RegisterResource("character.texture", ResourceType::RenderData, data);
    this->textureData = t;
}


void Character::Render() {
    Renderer::RenderTexture(this->textureData, this->controller.GetPos());
}

void Character::Transform(const Vec2 &v) {
    this->size += v;
}

void Character::Scale(float dx) {
    this->size *= dx;
}


void Character::Update(float dt) {
    this->controller.Update(dt);    
    this->textureData.size = this->size;
}

Vec2 Character::GetCentrePos() {
    return this->controller.GetPos() + 0.5 * this->size;
}
