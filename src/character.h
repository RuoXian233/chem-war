#pragma once
#include <SDL2/SDL.h>
#include "lib/resource.h"
#include "lib/render.h"

namespace chem_war {
    class Character final {
    public:
        static Character *Instance(const ResourcePack &textures);
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

        void CheckMovement();
        bool CheckBound();
        void AddMovement(Direction d);
        inline Vec2 GetVelocity() { return this->velocity; }
        inline Vec2 GetAcceleration() { return this->acceleration; }
        inline Vec2 GetPos() { return this->pos; }

    private:
        Character();
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
    };
}
