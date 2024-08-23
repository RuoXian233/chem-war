#include "enemy.h"
#include "lib/components.h"

using namespace chem_war;
using namespace engine;


Enemy::Enemy(ecs::World &world) : GameObject(std::format("enemy {}", (void *) this), world) {
    ecs::Commands cmd(world);
    auto t = Renderer::Text("原始胎海之水", SDL_Color { 10, 225, 255, 255 });
    auto size = Renderer::GetRenderSize();

    int distanceX = rand() % (int) Enemy::MAX_GERERATE_DISTANCE;
    int distanceY = rand() % (int) Enemy::MAX_GERERATE_DISTANCE;
    int side = rand() % 4;
    Vec2 generatePos;
    switch (side) {
        case 0:
            generatePos = Vec2(distanceX, -distanceY);
            break;
        case 1:
            generatePos = Vec2(distanceX, size.y + distanceY);
            break;
        case 2:
            generatePos = Vec2(-distanceX, distanceY);
            break;
        case 3:
            generatePos = Vec2(size.x + distanceX, distanceY);
            break;
        default:
            assert(false);
    }

    this->entityId = cmd.Spawned<components::Movement, components::Texture2D, components::SimpleCollider2D, components::SceneAssosication>(
        components::Movement { Vec2(), generatePos },
        components::Texture2D { t, Vec2() },
        components::SimpleCollider2D { "$enemy", Enemy::showCollider, Vec2(), t.size, [this](auto, auto, auto id, auto) {
            if (id == "$character") {
                this->dead = true;
            } else {
                this->speedIncreament += this->speedIncreamenRate;
            }
        } },
        components::SceneAssosication { "game" }
    );
    this->c = Character::Instance();
    cmd.Execute();
}

void Enemy::Seek() {
    auto &movement = this->GetComponent<components::Movement>();
    auto dp = c->GetCentrePos() - movement.pos;
    movement.velocity = dp * this->baseSpeed * (1 + this->speedIncreament);
}

void Enemy::OnCollideWithCharacter() {
    // point & rect collision
    if (PointInRect(this->GetComponent<components::Movement>().pos, c->GetPos(), c->GetSize())) {
        c->AddCollision(this);
        c->IncHp(-0.16);
    }
}   


void Enemy::Update(float) {
    this->Seek();
    this->OnCollideWithCharacter();
    this->speedIncreament = 0;
}

Enemy::~Enemy() {}
