#ifndef ARDUINO

#include "one.hpp"
#include "r_terminal.hpp"

// === MAIN ===

int main()
{
    setupOne();
    terminalRunLoop();
}

#endif
