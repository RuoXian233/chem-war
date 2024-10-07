#pragma once

#include "../lib/render.h"
#include "../preset/controller.h"

using namespace engine;


namespace chem_war {
    class Character final {
    public:
        static Character *Instance();
        void Prepare();
        void Render();
        void Update(float dt);
        static void Destroy();
        void Transform(const Vec2 &v);
        void Scale(float dx);
        Vec2 GetCentrePos();

        inline Vec2 GetSize() { return this->size; }
        preset::Basic2DController controller;

    private:
        Vec2 size;
        static Character *sCharacter;
        std::string texture;
        engine::Renderer::Texture textureData;
        std::map<std::string, Vec2> constantForces;
    
    };
}
