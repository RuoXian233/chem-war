#pragma once
#include "../lib/render.h"
#include "../lib/object.h"
#include "../lib/ecs.h"
#include <map>
#include <functional>


namespace chem_war {
    namespace ui {
        class DebugPanel : public chem_war::GameObject {
        public:
            using StringValRetriever = std::function<std::string()>;
            using NumericValRetriever = std::function<float()>;

            void Update(float dt) override;
            void Render() override;
            void Hide(bool state);
            bool IsHide();

            void AddItem(const std::string &name, StringValRetriever r);
            void AddItem(const std::string &name, NumericValRetriever n);

            DebugPanel(chem_war::ecs::World &world, const Vec2 &pos);
            ~DebugPanel();

        private:
            static inline const std::string DEFUALT_DEBUG_PANEL_ID = "ui.debug.panel";
            std::map<std::string, StringValRetriever> stringVals;
            std::map<std::string, NumericValRetriever> numericVals; 
            bool show = true;
        };  
    }
}
