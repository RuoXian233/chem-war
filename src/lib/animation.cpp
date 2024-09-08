#include "animation.h"
#include "log.h"

using namespace engine;

static Logger logger("AnimationManager");
std::map<std::string, Animation *> AnimationManager::registeredAnimations;


Animation::Animation(const std::vector<std::string> &frames, int duration) {
    this->frames = frames;
    this->duration = duration;
    this->currentFrame = 0;
    this->currentTicks = 0;

    logger.StartParagraph(Logger::Level::Debug);

    DEBUG_F("Animation #{}:", AnimationManager::Size() + 1);
    DEBUG_F("  Duration: {}", duration);
    DEBUG("Frames:");
    for (const auto &frame : frames) {
        DEBUG_F("    {}", frame);
    }
    logger.EndParagraph();

    DEBUG("Starting animation validation check ...");
    if (!this->IsValid()) {
        ERROR("Corrupted animation");
        exit(EXIT_FAILURE);
    }
}

bool Animation::IsValid() {
    if (this->frames.empty() || this->duration <= 0) {
        return false;
    }

    for (const auto &frame : this->frames) {
        auto frameContent = ResourceManager::Get(frame);
        if (frameContent->type != ResourceType::Texture) {
            return false;
        }
        
        if (frameContent->state != ResourceState::Good) {
            return false;
        }
    }
    return true;
}

void Animation::Next(const Vec2 &pos) {
    if (this->current.textureData) {
        Renderer::RenderTexture(this->current, pos);
    } else {
        WARNING_F("Could not load animation frame #{}: `{}` (=nil)", this->currentFrame, this->frames[this->currentFrame]);
    }
}

void Animation::Update(float dt) {
    this->currentTicks += (int) (dt * 1000);
    if (this->currentTicks > this->duration) {
        auto surf = ResourceManager::Get(this->frames[this->currentFrame]);
        if (this->current.textureData) {
            SDL_DestroyTexture(this->current.textureData);
        }
        this->current = Renderer::CreateTexture(surf->GetAs<SDL_Surface>());
        this->currentFrame++;
        if (this->currentFrame == this->frames.size()) {
            this->currentFrame = 0;
        }
        this->currentTicks = 0;
    }
}

Animation::~Animation() {}


int AnimationManager::Size() {
    return AnimationManager::registeredAnimations.size();
}

void AnimationManager::Initialize() {
    logger.SetDisplayLevel(GLOBAL_LOG_LEVEL);
    INFO("AnimationManager initialized");
}

void AnimationManager::Finalize() {
    INFO("AnimationManager finalize");
}


void AnimationManager::Play(Animation &anim, const Vec2 &pos) {
    
}

