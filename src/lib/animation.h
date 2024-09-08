#pragma once
#include "resource.h"
#include "render.h"


namespace engine {
    using AnimationSequence = std::vector<std::string>;

    struct Animation {
        int totalFrames;
        int currentFrame;
        // ms
        int duration;
        int currentTicks;
        // id of image resource
        std::vector<std::string> frames;

        Animation(const std::vector<std::string> &frames, int duration);
        bool IsValid();
        void Next(const Vec2 &pos);

        void Update(float dt);
        ~Animation();

    private:
        Renderer::Texture current;
    };

    class AnimationManager final {
    public:
        static void Initialize();
        static void Finalize();
        static void Play(Animation &anim, const Vec2 &pos);

        static int Size();

    private:
        AnimationManager() = default;
        ~AnimationManager() = default;
        
    public:
        static std::map<std::string, Animation *> registeredAnimations;

    };
}
