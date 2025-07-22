#ifndef ARDUINO

#include "pages.hpp"
#include "r_terminal.hpp"

// === MAIN ===

int main()
{
    setupPages();
    terminalRunLoop();
}

#endif
