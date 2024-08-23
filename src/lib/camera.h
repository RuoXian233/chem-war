#pragma once
#include "utils.hpp"


namespace engine {
    struct CameraState {
        bool enabled = false;
        Vec2 pos;
        Vec2 viewPort;
        float zoom;
    };

    class Camera final {
    public:
        static void Initalize();
        static void Update(float dt);
        static void Finalize();
        static CameraState& GetState();

        static void Translate(const Vec2& delta);
        static void Zoom(float delta);
        static void SetViewPort(const Vec2& viewPort);
        static void Enabled(bool val);
        static bool Enabled();

        static void Reset();

    private:
        static CameraState currentState;
    };
}