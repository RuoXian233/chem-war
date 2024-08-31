#pragma once
#include "resource.h"


namespace engine {
    struct Animation {
        int totalFrames;
        int currentFrame;
        // ms
        int duration;
        // id of image resource
        std::vector<std::string> frames;

        Animation(const std::vector<std::string> &frames, int duration);
        bool IsValid();
        ~Animation();
    };

    class AnimationManager final {
    public:
        static void Initialize();
        static void Finalize();

        static int Size();

    private:
        AnimationManager() = default;
        ~AnimationManager() = default;
        
    public:
        static std::map<std::string, Animation *> registeredAnimations;

    };
}
