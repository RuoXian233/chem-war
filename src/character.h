#pragma once
#include <SDL2/SDL.h>
#include "lib/resource.h"
#include "lib/render.h"
#include "lib/object.h"
#include "lib/particle.h"
#include "ui/bar.h"

using namespace engine;


namespace chem_war {
    class Character final {
    public:
        struct Bullet {
            ecs::Entity entity;
        };

        static Character *Instance(ecs::World &world, const ResourcePack &textures);
        static Character *Instance();
        void Prepare();
        void Render();
        void Update(float dt);
        static void Destroy();

        void Translate(const Vec2 &v);
        void Transform(const Vec2 &v);
        void Scale(float dx);

        void SetState(int state);        
        void AddForce(const Vec2 &f);
        void AddConstantForce(const std::string &fname, const Vec2 &f);
        void RemoveConstantForce(const std::string &name);
        void AddVelocity(const Vec2 &v);
        void SetVelocity(const Vec2 &v);
        void SetVelocity(float x, float y);
        void SetAcceleration(const Vec2 &v);
        void SetAcceleration(float ax, float vy);
        inline float GetHp() { return this->hp; }
        void SetHp(float v);
        void IncHp(float v);

        Vec2 GetCentrePos();

        void CheckMovement();
        bool CheckBound();
        void AddMovement(Direction d);
        inline Vec2 GetVelocity() { return this->velocity; }
        inline Vec2 GetAcceleration() { return this->acceleration; }
        inline Vec2 GetPos() { return this->pos; }
        inline Vec2 GetSize() { return this->size; }
        inline float GetMaxHp() { return this->hpMax; }

        void StartAttack();
        void StopAttack();
        void OnDie();

        // debug use
        void DrawCollider();
        void ColliderVisibility(bool v);
        void AddCollision(GameObject *go);
        void ClearCollision();
        std::vector<GameObject *> &GetCollisions();

        // debug use
        int hits = 0;
        bool showBulletCollider = true;
        Bullet bullet;
        static constexpr auto idNULL = (ecs::Entity) ecs::SparseSet<ecs::Entity, 32>::null;

    private:
        ecs::World &world;

        Character(ecs::World &world);
        Vec2 size;
        Vec2 pos;
        Vec2 velocity;
        Vec2 acceleration;
        static Character *sCharacter;
        ResourcePack textures;
        std::map<std::string, Renderer::Texture> textureData;
        std::map<std::string, Vec2> constantForces;
        int currentState;
        float mass;

        float maxVelocityX = 600;
        float maxVelocityY = 600;
        float minVelocityX = -600;
        float minVelocityY = -600;
    
        float velocityInterval[4];

        // Game Attributes
        float hp = 0;
        float hpMax = 100;
        bool moving = false;
        bool attacking = false;
        ParticleLauncher *particleEffect;
        float bulletLength = 160;
        float bulletWidth = 64;

        // Debug attributes
        std::vector<GameObject *> collsions;
        bool drawCollider = true;
    };
}
