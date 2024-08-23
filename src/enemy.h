#pragma once
#include "lib/object.h"
#include "character.h"


namespace chem_war {
    class Enemy : public engine::GameObject {
    public:
        Enemy(engine::ecs::World &world);

        void Update(float) override;
        virtual void Seek();
        virtual void OnCollideWithCharacter();
        inline float GetSpeedIncreament() { return this->speedIncreament; }

        virtual ~Enemy();

        bool dead = false;
        inline static bool showCollider = true;
    private:
        float baseSpeed = 1.6;
        float speedIncreament = 0;
        float speedIncreamenRate = 0.32;
        Character *c;

        static constexpr float MAX_GERERATE_DISTANCE = 640.0f;
    };
}
