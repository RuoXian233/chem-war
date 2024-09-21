#include "game.h"


int main(int argc, char *argv[]) {
    chem_war::Game::Prepare(argc, argv);
    chem_war::Game::Run();
    chem_war::Game::Quit();    
}
