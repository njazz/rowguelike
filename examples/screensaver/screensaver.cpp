#ifndef ARDUINO

#include "screensaver.hpp"
#include "r_terminal.hpp"


// === MAIN ===

int main()
{
    setupScreensaver();
    terminalRunLoop();
}

#endif
