#pragma once

#include <iostream>
#include <termios.h>
#include <unistd.h>

#include "rowguelike.hpp"

char getch_with_timeout(int timeout_ms, char default_value)
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt); // get current terminal settings
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO); // disable canonical mode and echo

    newt.c_cc[VMIN] = 0;                 // minimum number of characters for noncanonical read
    newt.c_cc[VTIME] = timeout_ms / 100; // timeout in 100ms units

    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // apply new settings

    char ch;
    int result = read(STDIN_FILENO, &ch, 1); // attempt to read one character

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore old settings

    if (result == 1)
        return ch; // char read successfully
    else
        return default_value; // timeout occurred
}

void terminalRunLoop(const size_t timeout = 100)
{
    // -----

    while (true) {
        std::cout << "\033[2J\033[H"; // clear screen

        // std::cout << handler.line1.substr(0, 16) << "\n";
        // std::cout << handler.line2.substr(0, 16) << "\n";

        std::cout << "Inputs: " << RWE.rawInput.left << RWE.rawInput.right << RWE.rawInput.up
                  << RWE.rawInput.down << RWE.rawInput.select << "\n";

        std::cout << "################\n";
        std::cout << RWE.drawContext.buffer[0] << "\n";
        std::cout << RWE.drawContext.buffer[1] << "\n";
        std::cout << "################\n";

        char ch = getch_with_timeout(timeout, '*'); //getch();

        if (ch == 'q')
            break;

        RWE.rawInput.left = ch == 'a';
        RWE.rawInput.right = ch == 'd';
        RWE.rawInput.up = ch == 'w';
        RWE.rawInput.down = ch == 's';
        RWE.rawInput.select = ch == ' ';

        // sleep(1);

        RWE.runLoop();
    }
}
