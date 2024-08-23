#pragma once
#include "scene.h"


namespace engine {
    struct EffectState {
        float timer = 0;
        std::string effectId;
        bool finished = false;
        int duration;

        virtual void Update(Scene *scene, float dt) = 0;
        virtual void Render(Scene *scene) = 0;
        virtual inline void Terminate(Scene *scene) {
            this->finished = true;
        }

        virtual inline void End(Scene *scene) {
            this->finished = true;
        }
    };

    class EffectSystem final {
    public:
        static void Initalize();

        static void Update(float dt);
        // Should be called after scene is rendered
        static void Render();
        static void SetTargetScene(Scene *scene);
        template <typename T>
        static T *GetEffectState() {
            if (currentEffectState) {
                return dynamic_cast<T *>(currentEffectState);
            }
            return nullptr;
        }

        static void SetEffectState(EffectState *state, bool forced = false);
        static void TerminateEffect();

        static void Finalize();

        private:
            static Scene *targetScene;
            static EffectState *currentEffectState;
            static bool effectRunning;

            static std::vector<EffectState *> effectQueue;
        };

    namespace effects {
        struct FadeEffect : public EffectState {
            FadeEffect(int duration, bool direction);
            bool direction;

            void Update(Scene *scene, float dt) override;
            void Render(Scene *scene) override;
            void Terminate(Scene *scene) override;
        };

        struct ShineEffect : public EffectState {
            int interval;
            Renderer::Color color;
            int total;
            int current = 0;
            ShineEffect(int interval, Renderer::Color color, int total);

            void Update(Scene *scene, float dt) override;
            void Render(Scene *scene) override;
        };

        void SceneFadeIn(int duration);
        void SceneFadeOut(int duration);
        void Shine(int interval, Renderer::Color color, int total);
    }
}
