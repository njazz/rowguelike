#ifndef ARDUINO

#include "pong.hpp"
#include "r_terminal.hpp"


// === MAIN ===

int main()
{
    setupPong();
    terminalRunLoop();
}

#endif
