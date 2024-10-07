#include "../ui/label.h"


namespace engine::preset {
    class JumpingTextManager;

    class JumpingText {
    public:
        friend class JumpingTextManager;

        JumpingText(ecs::World &world, const Vec2 &pos, const std::string &text, const Color &color, int fontsize = 18);
        void Update(float dt);
        void Render();
        void Play();
        void Clear();

        ~JumpingText();

    private:
        float speedA = 10;
        float speed = 320;
        Vec2 initialPos;
        Color c;
        int alpha = 255;
        bool start = false;
        engine::ui::Label *view;
        static unsigned long long id;
    };

    class JumpingTextManager final {
    public:
        static void Add(JumpingText *t);
        static void Update(float dt);
        static void Render();
    private:
        static std::vector<JumpingText *> textCache;
    };
}
