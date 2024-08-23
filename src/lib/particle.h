#pragma once
#include "render.h"


namespace engine {
    struct Particle {
        int hp;
        Vec2 direction;
        bool isDead;
        SDL_Color color;
        Vec2 pos;
    };

    struct ParticleSystem {
        Vec2 gravity;
        int particleNum;
        Particle *particles;    
    };

    struct ParticleLauncher {
        Vec2 shootDirection;
        int particleHpMax;
        float halfDegree;
        SDL_Color color;
        ParticleSystem *world;
        int num;
        Vec2 pos;
    };


    class ParticleManager final {
    public:
        static void Initialize();
        static void Finalize();

        static void CreateParticleSystem(const Vec2 &gravity);
        static ParticleSystem *GetParticleSystem();
        static ParticleLauncher *CreateParticleLauncher(const Vec2 &pos, const Vec2 &shootDirection, int particleHpMax, float halfDegree, const SDL_Color &color, int num = -1);
        static void IncreaseParticleSystemCapacity();
        static Particle *GetNextDeadParticle(int *idx);
        static Particle *GetNextUndeadParticle(int *idx);
        static void DrawParticle(Particle *p);
        static void ShootParticle(ParticleLauncher *launcher);

        static void Update(float dt);
    private:
        static inline int particleRadius = 3;
        static inline float particleDensity = 0.16;
        static ParticleSystem *world;
        static std::vector<ParticleLauncher*> particleLaunchers;
    };
}
