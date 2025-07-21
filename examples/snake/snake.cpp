#ifndef ARDUINO

#include "snake.hpp"

#include "r_terminal.hpp"

// === MAIN ===

int main()
{
    setupExampleSnake();
    terminalRunLoop(250);
}

#endif
