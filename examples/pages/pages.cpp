#ifndef ARDUINO

#include "pages.hpp"
#include "r_terminal.hpp"

// === MAIN ===

int main()
{
    setupPages();
    terminalRunLoop();

    IF_LET(RWE.make().spawn(), { value = 0; });
}

#endif
