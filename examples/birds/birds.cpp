#ifndef ARDUINO

#include "birds.hpp"
#include "r_terminal.hpp"


// === MAIN ===

int main()
{
    setupBirds();
    terminalRunLoop();
}

#endif
