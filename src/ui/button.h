#include "../lib/object.h"


namespace engine::ui {
    class Button : public GameObject {
    public:
        Button(const std::string &id, ecs::World &world);
        void Config(const std::string &normalRes, const std::string &clickRes = "", const std::string &hoverRes = "");
        void Bind(std::function<void(ecs::World &, void *)>);
    private:
        std::string normal;
        std::string click;
        std::string hover;
        bool binded = false;
        std::function<void(ecs::World &, void *)> callback;
    };
}
