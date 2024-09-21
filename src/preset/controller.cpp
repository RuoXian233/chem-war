#include "controller.h"


using namespace engine::preset;


Basic2DController::Basic2DController(const Vec2 &pos, float mass, float dx, float mdx, float dy, float mdy) : mass(mass) {
    this->velocity = Vec2();
    this->pos = pos;
    this->acceleration = Vec2();
    this->velocityInterval[0] = dx;
    this->velocityInterval[1] = mdx;
    this->velocityInterval[2] = dy;
    this->velocityInterval[3] = mdy;
}

Basic2DController::Basic2DController() : engine::preset::Basic2DController(Vec2()) {}

void Basic2DController::AddVelocity(const Vec2 &v) {
    this->velocity += v;
}

void Basic2DController::SetVelocity(const Vec2 &v) {
    this->velocity = v;
}

void Basic2DController::SetVelocity(float x, float y) {
    this->velocity.x = x;
    this->velocity.y = y;
}

void Basic2DController::SetAcceleration(const Vec2 &v) {
    this->acceleration = v;
}

void Basic2DController::SetAcceleration(float ax, float ay) {
    this->acceleration.x = ax;
    this->acceleration.y = ay;
}

bool Basic2DController::CheckBound() {
    return true;
}

void Basic2DController::CheckMovement() {
    if (this->GetVelocity().y < 0 && !InputManager::QueryKey(SDL_SCANCODE_W)) {
        this->AddVelocity(Vec2(0, this->velocityInterval[0]));
    }
    if (this->GetVelocity().y > 0 && !InputManager::QueryKey(SDL_SCANCODE_S)) {
        this->AddVelocity(Vec2(0, this->velocityInterval[1]));
    }
    if (this->GetVelocity().x < 0 && !InputManager::QueryKey(SDL_SCANCODE_A)) {
        this->AddVelocity(Vec2(this->velocityInterval[2], 0));
    }
    if (this->GetVelocity().x > 0 && !InputManager::QueryKey(SDL_SCANCODE_D)) {
        this->AddVelocity(Vec2(this->velocityInterval[3], 0));
    }
}

void Basic2DController::Translate(const Vec2 &v) {
    this->pos += v;
}

void Basic2DController::AddForce(const Vec2 &force) {
    this->acceleration += (force * this->mass);
}

void Basic2DController::AddConstantForce(const std::string &fname, const Vec2 &f) {
    if (this->constantForces.find(fname) != this->constantForces.end()) {
        this->constantForces.at(fname) = f;
        return;
    }
    this->constantForces.insert(std::make_pair(fname, f));
}

void Basic2DController::RemoveConstantForce(const std::string &name) {
    if (this->constantForces.find(name) != this->constantForces.end()) {
        this->constantForces.erase(this->constantForces.find(name));
    }
}

void Basic2DController::Update(float dt) {
    if (this->velocity.Length() != 0) {
        this->moving = true;
    } else {
        this->moving = false;
    }

    for (const auto [name, force] : this->constantForces) {
        this->AddForce(force);
    }
    this->CheckMovement();
    this->Translate(this->velocity * dt);
}

void Basic2DController::AddMovement(Direction d) {
    if (!this->CheckBound()) {
        return;
    }
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


void Basic2DController::SetMaxVelocity(Vec2 &v) {
    this->maxVelocityX = v.x;
    this->maxVelocityY = v.y;
}

void Basic2DController::SetMinVelocity(Vec2 &v) {
    this->minVelocityX = v.x;
    this->maxVelocityY = v.y;
}

engine::Vec2 Basic2DController::GetMaxVelocity() {
    return { this->maxVelocityX, this->maxVelocityY };
}

engine::Vec2 Basic2DController::GetMinVelocity() {
    return { this->minVelocityX, this->minVelocityY };
}

void Basic2DController::SetPos(const Vec2 &v) {
    this->pos = v;
}
