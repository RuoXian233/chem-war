#include "game.h"
#include "lib/drefl.h"


#undef main
int main(int argc, char *argv[]) {
    chem_war::Game::Prepare(argc, argv);
    chem_war::Game::Run();
    chem_war::Game::Quit();
    return 0;
}

