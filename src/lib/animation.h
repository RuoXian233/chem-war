#pragma once
#include "resource.h"


namespace chem_war {
    struct Animation {
        int totalFrames;
        int currentFrame;
        // ms
        int duration;
        // id of image resource
        std::vector<std::string> frames;
    };

    class AnimationManager final {

    };
}
