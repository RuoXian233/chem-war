#include "../lib/input.h"


namespace engine::preset {
    class Basic2DController {
        Vec2 pos;
        Vec2 velocity;
        Vec2 acceleration;
        Vec2 boundPos;
        Vec2 boundSize;

        bool moving = false;

        float maxVelocityX = 600;
        float maxVelocityY = 600;
        float minVelocityX = -600;
        float minVelocityY = -600;
        std::map<std::string, Vec2> constantForces;
        float mass;
        float velocityInterval[4];        

    public:
        Basic2DController(const Vec2 &pos, float mass = 1.0f, float dx = 60, float mdx = -60, float dy = 60, float mdy = -60);
        Basic2DController();

        void CheckMovement();
        void AddMovement(Direction d);
        bool CheckBound();

        inline Vec2 GetVelocity() { return this->velocity; }
        inline Vec2 GetAcceleration() { return this->acceleration; }
        inline Vec2 GetPos() { return this->pos; }

        void SetPos(const Vec2 &v);

        void SetMaxVelocity(Vec2 &v);
        void SetMinVelocity(Vec2 &v);
        Vec2 GetMaxVelocity();
        Vec2 GetMinVelocity();

        void AddForce(const Vec2 &f);
        void AddConstantForce(const std::string &fname, const Vec2 &f);
        void RemoveConstantForce(const std::string &name);
        void AddVelocity(const Vec2 &v);
        void SetVelocity(const Vec2 &v);
        void SetVelocity(float x, float y);
        void SetAcceleration(const Vec2 &v);
        void SetAcceleration(float ax, float vy);
        void Translate(const Vec2 &v);

        void Update(float dt);
    };
};