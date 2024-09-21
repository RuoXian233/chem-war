#include "../lib/object.h"


namespace engine::ui {
    class Label : public GameObject {
    public:
        Label(const std::string &id, const std::string &text, ecs::World &world);
        void Config(int fontsize, Color fg, Color bg = { 0, 0, 0, 255 });
        std::string GetText();
        void SetText(const std::string &text);
        
    private:
        ~Label();
        std::string text;
        int fontsize;
        Color fg, bg;
    };
}
