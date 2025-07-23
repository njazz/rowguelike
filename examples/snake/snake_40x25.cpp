#ifndef ARDUINO

#define RW_SETUP_SCREEN_WIDTH 40
#define RW_SETUP_SCREEN_HEIGHT 25

#include "snake.hpp"

#include "r_terminal.hpp"

// === MAIN ===

int main()
{
    setupExampleSnake();
    terminalRunLoop(150);
}

#endif
