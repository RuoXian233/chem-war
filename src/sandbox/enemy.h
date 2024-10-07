#include "../ui/bar.h"


namespace chem_war {
    class Enemy final {
    public:
        Enemy();
        ~Enemy();

    private:
        engine::ui::Bar healthBar;
        float hp;
        float currentHp;
    };
}
