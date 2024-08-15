#include "character.h"
#include "lib/input.h"

using namespace chem_war;
Character *Character::sCharacter;


Character *Character::Instance(const ResourcePack &textures) {
    if (!Character::sCharacter) {
        Character::sCharacter = new Character;
        Character::sCharacter->textures = textures;
    }
    return Character::sCharacter;
}

Character *Character::Instance() {
    assert(Character::sCharacter && "Character not initialized");
    return Character::sCharacter;
}

Character::Character() {
    this->velocity = Vec2();
    this->acceleration = Vec2();
    this->mass = 1.0f;
    this->velocityInterval[0] = 60;
    this->velocityInterval[1] = -60;
    this->velocityInterval[2] = 60;
    this->velocityInterval[3] = -60;
}

void Character::Destroy() {
    if (Character::sCharacter) {
        delete Character::sCharacter;
    }
}


void Character::Prepare() {
    this->size = Vec2();
    this->pos = Vec2();
    int i = 1;
    for (const auto &r : this->textures) {
        auto data = r->GetAs<SDL_Surface>();
        auto t = Renderer::CreateTexture(data);
        auto uri = std::format("{}.{}", CHRACTER_FIGURE_DATA, i);
        ResourceManager::RegisterResource(uri, ResourceType::RenderData, data);
        this->textureData.insert(std::make_pair(uri, t));
        i++;
    }
    this->currentState = 1;
}

void Character::SetState(int state) {
    assert(state <= this->textures.size() && "Invilid state");
    this->currentState = state;
}

void Character::Render() {
    auto uri = std::format("{}.{}", CHRACTER_FIGURE_DATA, this->currentState);
    Renderer::RenderTexture(this->textureData[uri], this->pos);
}

void Character::Translate(const Vec2 &v) {
    this->pos += v;
}

void Character::Transform(const Vec2 &v) {
    auto uri = std::format("{}.{}", CHRACTER_FIGURE_DATA, this->currentState);
    this->textureData[uri].size += v;
}

void Character::Scale(float dx) {
    auto uri = std::format("{}.{}", CHRACTER_FIGURE_DATA, this->currentState);
    this->textureData[uri].size *= dx;
}

void Character::AddForce(const Vec2 &force) {
    this->acceleration += (force * this->mass);
}

void Character::AddConstantForce(const std::string &fname, const Vec2 &f) {
    if (this->constantForces.find(fname) != this->constantForces.end()) {
        this->constantForces.at(fname) = f;
        return;
    }
    this->constantForces.insert(std::make_pair(fname, f));
}

void Character::RemoveConstantForce(const std::string &name) {
    if (this->constantForces.find(name) != this->constantForces.end()) {
        this->constantForces.erase(this->constantForces.find(name));
    }
}

void Character::AddVelocity(const Vec2 &v) {
    this->velocity += v;
}

void Character::SetVelocity(const Vec2 &v) {
    this->velocity = v;
}

void Character::SetVelocity(float x, float y) {
    this->velocity.x = x;
    this->velocity.y = y;
}

void Character::SetAcceleration(const Vec2 &v) {
    this->acceleration = v;
}

void Character::SetAcceleration(float ax, float ay) {
    this->acceleration.x = ax;
    this->acceleration.y = ay;
}

void Character::Update(float dt) {
    for (const auto [name, force] : this->constantForces) {
        this->AddForce(force);
    }
    this->Translate(this->velocity * dt);
}

bool Character::CheckBound() {
    return true;
}

void Character::CheckMovement() {
    auto c = this;
    if (c->GetVelocity().y < 0 && !InputManager::QueryKey(SDL_SCANCODE_W)) {
        c->AddVelocity(Vec2(0, 60));
    }
    if (c->GetVelocity().y > 0 && !InputManager::QueryKey(SDL_SCANCODE_S)) {
        c->AddVelocity(Vec2(0, -60));
    }
    if (c->GetVelocity().x < 0 && !InputManager::QueryKey(SDL_SCANCODE_A)) {
        c->AddVelocity(Vec2(60, 0));
    }
    if (c->GetVelocity().x > 0 && !InputManager::QueryKey(SDL_SCANCODE_D)) {
        c->AddVelocity(Vec2(-60, 0));
    }
}

void Character::AddMovement(Direction d) { 
    int dn = static_cast<int>(d);
    auto c = this;
    auto currentVelocity = this->velocity;
    switch (d) {
    case Direction::Up:
        if (this->velocity.y > this->minVelocityY) {
            c->AddVelocity(Vec2(0, -this->velocityInterval[dn]));
        }
        c->SetVelocity(Vec2(currentVelocity.x, minVelocityY));
        break;
    case Direction::Down:
        if (this->velocity.y < this->maxVelocityY) {
            c->AddVelocity(Vec2(0, -this->velocityInterval[dn]));
        }
        c->SetVelocity(Vec2(currentVelocity.x, maxVelocityY));
        break;
    case Direction::Left:
        if (this->velocity.x > this->minVelocityX) {
            c->AddVelocity(Vec2(-this->velocityInterval[dn], 0));
        }
        c->SetVelocity(Vec2(minVelocityY, this->velocity.y));
        break;
    case Direction::Right:
        if (this->velocity.x < this->maxVelocityX) {
            c->AddVelocity(Vec2(-this->velocityInterval[dn], 0));
        }
        c->SetVelocity(Vec2(maxVelocityY, this->velocity.y));
        break;
    }
}
