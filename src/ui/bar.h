#include "../lib/object.h"
#include "../lib/ecs.h"


namespace chem_war {
    namespace ui {
        class Bar : public chem_war::GameObject {
        public:
            Bar(const std::string &name, ecs::World &world, const Vec2 &pos);
        
            void AttachValue(std::function<float()> valueRetriever);
            void Config(int margin, const SDL_Color &inner, const SDL_Color &outer, const Vec2 &size);

            virtual void Update(float dt) override;
            virtual void Render() override;

            ~Bar();

        private:
            int margin {};
            std::function<float()> retriever {};
            SDL_Color innerColor;
            SDL_Color outerColor;
            float percentage {};

            Renderer::Texture renderContext;
        };
    }
}
