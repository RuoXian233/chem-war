#include "particle.h"
#include "log.h"

using namespace engine;
ParticleSystem *ParticleManager::world;
std::vector<ParticleLauncher*> ParticleManager::particleLaunchers;

static engine::Logger logger("ParticleManager");


void ParticleManager::Initialize() {
    ParticleManager::world = nullptr;
    INFO("Particle system initialized");
    logger.SetDisplayLevel(Logger::Level::Debug);
}

ParticleSystem *ParticleManager::GetParticleSystem() {
    assert(ParticleManager::world && "Particle system not created");
    return ParticleManager::world;
}


void ParticleManager::CreateParticleSystem(const Vec2 &gravity) {
    if (!ParticleManager::world) {
        srand((unsigned) time(nullptr));
        ParticleManager::world = new ParticleSystem;

        world->gravity = gravity;
        world->particleNum = WORLD_PARTICAL_INIT_NUM;
        world->particles = new Particle[WORLD_PARTICAL_INIT_NUM];

        DEBUG_F("Particle system: gravity = {}, particleNum = {}, particles = {}", gravity.ToString(), world->particleNum, (void *) world->particles);

        for (int i = 0; i < WORLD_PARTICAL_INIT_NUM; i++) {
            world->particles[i].isDead = true;
        }
    }
}


ParticleLauncher *ParticleManager::CreateParticleLauncher(const Vec2 &pos, const Vec2 &shootDirection, int particleHpMax, float halfDegree, const SDL_Color &color, int num) {
    ParticleLauncher *launcher = new ParticleLauncher;
    launcher->shootDirection = shootDirection;
    launcher->particleHpMax = particleHpMax;
    launcher->halfDegree = halfDegree;
    launcher->color = color;
    if (num <= 0) {
        launcher->num = (int) ceil(halfDegree * 2 * ParticleManager::particleDensity);
    } else {
        launcher->num = num;
    }
    launcher->pos = pos;
    launcher->world = ParticleManager::world;

    INFO("Creating a new particle launcher");
    DEBUG_F("{}", (void *) launcher);
    DEBUG_F("pos = {}, shootDirection = {}, particleHpMax = {}, halfDegree = {}, color = {}, num = {}", pos.ToString(), shootDirection.ToString(), particleHpMax, halfDegree, color.r, color.g, color.b, color.a, num);
    ParticleManager::particleLaunchers.push_back(launcher);
    DEBUG_F("Current {} launchers", ParticleManager::particleLaunchers.size());
    return launcher;
}

void ParticleManager::IncreaseParticleSystemCapacity() {
    ParticleManager::world->particles = (Particle *) realloc(ParticleManager::world->particles,
    sizeof(Particle) * (ParticleManager::world->particleNum + PARTICAL_SINK_INC));
    DEBUG_F("Particle system capacity increased to {}", ParticleManager::world->particleNum + PARTICAL_SINK_INC);

    for (int i = ParticleManager::world->particleNum - 1; i < ParticleManager::world->particleNum + PARTICAL_SINK_INC; i++) {
        ParticleManager::world->particles[i].isDead = true;
    }
    ParticleManager::world->particleNum += PARTICAL_SINK_INC;
}

Particle* ParticleManager::GetNextDeadParticle(int *idx){
    int sum = 0;
    (*idx)++;
    if (*idx >= world->particleNum) {
        *idx = 0;
    }

    while (!world->particles[*idx].isDead) {
        (*idx)++;
        if (*idx >= world->particleNum) {
            (*idx) = 0;
        }
        sum++;
        if (sum >= world->particleNum) {
            break;
        }
    }

    if (sum >= world->particleNum) {
        return nullptr;
    }
    return &world->particles[*idx];
}

Particle *ParticleManager::GetNextUndeadParticle(int *idx){
    int sum = 0;
    (*idx)++;
    if (*idx >= world->particleNum) {
        *idx = 0;
    }

    while (world->particles[*idx].isDead) {
        (*idx)++;
        if (*idx >= world->particleNum) {
            (*idx) = 0;
        }
        sum++;
        if (sum >= world->particleNum) {
            return nullptr;
        }
    }
    return &world->particles[*idx];
}


void ParticleManager::DrawParticle(Particle *particle) {
    Renderer::SetDrawColor(particle->color);
    Renderer::DrawCircle(particle->pos, ParticleManager::particleRadius);
    Renderer::ClearDrawColor();
}

void ParticleManager::ShootParticle(ParticleLauncher *launcher){
    auto world = launcher->world;
    int idx = 0;

    for (int i = 0; i < launcher->num; i++) {
        Particle *particle;

        while ((particle = ParticleManager::GetNextDeadParticle(&idx)) == nullptr) {
            ParticleManager::IncreaseParticleSystemCapacity();
        }

        int randNum = rand() % (int) (2 * launcher->halfDegree * 1000 + 1) - (int) launcher->halfDegree * 1000;
        float randDegree = randNum/1000.0f;

        particle->color = launcher->color;
        Vec2 direct = Vec2::Rotate(launcher->shootDirection, randDegree);
        particle->direction = direct;
        particle->hp = launcher->particleHpMax + rand() % 11 - 5;
        particle->isDead = false;
        particle->pos.x = launcher->pos.x;
        particle->pos.y = launcher->pos.y;
    }
}

void ParticleManager::Update(float dt){
    Particle *particle;

    for (int i = 0; i < world->particleNum; i++) {
        particle = &world->particles[i];

        if (!particle->isDead) {
            if (particle->hp > 0) {
                particle->pos.x += particle->direction.x + world->gravity.x / 2.0;
                particle->pos.y += particle->direction.y + world->gravity.y / 2.0;
                ParticleManager::DrawParticle(particle);
            }
            particle->hp--;
        }

        if (particle->hp <= 0) {
            particle->isDead = true;
        }
    }
}

void ParticleManager::Finalize() {
    for (auto launcher : ParticleManager::particleLaunchers) {
        DEBUG_F("Deleting particle launcher {}", (void *) launcher);
        delete launcher;
    }
    INFO("Particle system finalized");
}
