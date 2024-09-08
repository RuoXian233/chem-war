#include "effects.h"

using namespace engine;

EffectState *EffectSystem::currentEffectState;
Scene *EffectSystem::targetScene;
std::vector<EffectState*> EffectSystem::effectQueue;

static Logger logger("EffectSystem");


void EffectSystem::Initalize() {
    EffectSystem::targetScene = nullptr;
    EffectSystem::currentEffectState = nullptr;
    Renderer::EnableAlphaBlend();
    INFO("EffectSystem initialized");
}

void EffectSystem::Finalize() {
    EffectSystem::targetScene = nullptr;
    EffectSystem::currentEffectState = nullptr;
    Renderer::DisableAlphaBlend();
}

void EffectSystem::Render() {
    if (EffectSystem::targetScene && EffectSystem::currentEffectState) {
        EffectSystem::currentEffectState->Render(EffectSystem::targetScene);
    } 
}

void EffectSystem::Update(float dt) {
    if (!EffectSystem::currentEffectState && EffectSystem::targetScene) {
        if (!EffectSystem::effectQueue.empty()) {
            EffectSystem::currentEffectState = EffectSystem::effectQueue.back();
            EffectSystem::effectQueue.pop_back();
        }
    }

    if (EffectSystem::currentEffectState && EffectSystem::targetScene) {
        if (EffectSystem::currentEffectState->finished) {
            delete EffectSystem::currentEffectState;
            EffectSystem::currentEffectState = nullptr;
        } else {
            EffectSystem::currentEffectState->Update(EffectSystem::targetScene, dt);        
        }
    }
}

void EffectSystem::SetTargetScene(Scene *scene) {
    EffectSystem::targetScene = scene;
}

void EffectSystem::SetEffectState(EffectState* state, bool forced) {
    if (forced || !EffectSystem::currentEffectState) {
        EffectSystem::currentEffectState = state;
    } else {
        EffectSystem::effectQueue.push_back(state);
    }
}


void effects::SceneFadeIn(int duration) {
    EffectSystem::SetEffectState(new FadeEffect(duration, true));
}

void effects::SceneFadeOut(int duration) {
    EffectSystem::SetEffectState(new FadeEffect(duration, false));
}

void effects::Shine(int interval, Renderer::Color color, int total) {
    EffectSystem::SetEffectState(new ShineEffect(interval, color, total));
}

void EffectSystem::TerminateEffect() {
    if (EffectSystem::currentEffectState && EffectSystem::targetScene) {
        EffectSystem::currentEffectState->Terminate(EffectSystem::targetScene);
        delete EffectSystem::currentEffectState;
        EffectSystem::currentEffectState = nullptr;
    }
}


// effects implementation

effects::FadeEffect::FadeEffect(int duration, bool direction) {
    this->direction = direction;
    this->duration = duration;
    this->timer = 0;

}

void effects::FadeEffect::Update(Scene *scene, float dt) {
    this->timer += (int) (dt * 1000);

    if (this->timer >= this->duration) {
        this->End(scene);
    }
}

void effects::FadeEffect::Render(Scene *scene) {
    if (direction) {
        Renderer::SetDrawColor(0, 0, 0, (1 - timer / duration) * 255);
    } else {
        Renderer::SetDrawColor(0, 0, 0, timer / duration * 255);
    }
    Renderer::FillRect(Vec2(), Vec2(WINDOW_WIDTH, WINDOW_HEIGHT));
    Renderer::ClearDrawColor();
}

void effects::FadeEffect::Terminate(Scene *scene) {
    EffectState::Terminate(scene);
    this->timer = 0;
    this->Render(scene);
}


effects::ShineEffect::ShineEffect(int interval, Renderer::Color color, int total) {
    this->interval = interval;
    this->color = color;
    this->total = total;
}

void effects::ShineEffect::Render(Scene *scene) {
    if (this->current % 2 == 0) {
        Renderer::SetDrawColor(this->color);
        Renderer::FillRect(Vec2(0, 0), Vec2(WINDOW_WIDTH, WINDOW_HEIGHT));
        Renderer::ClearDrawColor();
    }
}

void effects::ShineEffect::Update(Scene *scene, float dt) {
    this->timer += (int) (dt * 1000);

    if (this->timer >= this->interval) {
        this->timer = 0;

        this->current++;
        if (this->current > this->total * 2) {
            this->End(scene);
        }
    }
}
