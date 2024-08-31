#include "animation.h"
#include "log.h"

using namespace engine;

static Logger logger("AnimationManager");
std::map<std::string, Animation *> AnimationManager::registeredAnimations;


Animation::Animation(const std::vector<std::string> &frames, int duration) {
    this->frames = frames;
    this->duration = duration;

    logger.StartParagraph(Logger::Level::Debug);

    DEBUG_F("Animation #{}:", AnimationManager::Size() + 1);
    DEBUG_F("  Duration: {}", duration);
    DEBUG("Frames:");
    for (const auto &frame : frames) {
        DEBUG_F("    {}", frame);
    }
    logger.EndParagraph();
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

Animation::~Animation() {}


int AnimationManager::Size() {
    return AnimationManager::registeredAnimations.size();
}

void AnimationManager::Initialize() {
    logger.SetDisplayLevel(Logger::Level::Debug);
    INFO("AnimationManager initialized");
}

void AnimationManager::Finalize() {
    INFO("AnimationManager finalize");
}

