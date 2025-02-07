#include "../lib/utils.hpp"


namespace engine::preset {
    class Tracer {
    public:
        Tracer();
        Tracer(const Vec2 &initalPos);
        ~Tracer() = default;

        void Update(float dt);
        void Trace(const Vec2 &target);

        void RenderHint();

        inline Vec2 Pos() const { return this->pos; } 
        inline Vec2 Velocity() const { return this->velocity; }
        inline Vec2 Acceleration() const { return this->acceleration; }
        inline Vec2 &Pos() { return this->pos; } 
        inline Vec2 &Velocity() { return this->velocity; }
        inline Vec2 &Acceleration() { return this->acceleration; }
        inline Vec2 &Size() { return this->tracerSize; }
        inline float GetDistance() { return (targetPos - pos).Length(); }

    private:
        Vec2 targetPos;
        Vec2 pos;
        Vec2 velocity;
        Vec2 acceleration;
        Vec2 tracerSize;
        float speed = 3;
    };
}