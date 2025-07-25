#ifndef ARDUINO

#include "pixel_arcade.hpp"
#include "r_terminal.hpp"

// === MAIN ===

int main()
{
    setupPixelArcade();
    terminalRunLoop();

    IF_LET(RWE.make().spawn(), { value = 0; });
}

#endif
