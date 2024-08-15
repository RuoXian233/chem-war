#pragma once
#include "lib/object.h"


namespace chem_war {
    class Enemy : public GameObject {
    public:
        Enemy(ecs::World &world);

        virtual ~Enemy();
    };
}
