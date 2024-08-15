#include "game.h"
#include "lib/ecs.h"
#include "lib/srefl.h"


#undef main
int main(int argc, char *argv[]) {
    // using vxt = chem_war::refl_common::variable_traits<decltype(&chem_war::Vec2::x)>;
    // using vyt = chem_war::refl_common::variable_traits<decltype(&chem_war::Vec2::y)>;
    // using vft = chem_war::refl_common::function_traits<decltype(&chem_war::Vec2::ToString)>;

    // using type1 = chem_war::srefl::function_pointer_type_t<&chem_war::Vec2::length>;
    // static_assert(std::is_same_v<type1, float (chem_war::Vec2::*)() const>);
    
    chem_war::Game::Prepare(argc, argv);
    chem_war::Game::Run();
    chem_war::Game::Quit();
    return 0;
}
