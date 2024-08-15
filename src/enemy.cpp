#include "enemy.h"
#include "components.h"

using namespace chem_war;


Enemy::Enemy(ecs::World &world) : GameObject(std::format("Enemy {}", (void *) this), world) {
    ecs::Commands cmd(world);
    this->entityId = cmd.Spawned<components::Movement, components::Graph>(
        components::Movement { Vec2(), Vec2() },
        components::Graph { false, 1, Vec2(200, 200), Vec2(100, 100), 255, 255, 55, 255 }
    );
    cmd.Execute();
}

Enemy::~Enemy() {}
