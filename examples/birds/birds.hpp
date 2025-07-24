/// @file birds.hpp
/// Custom character demo with animation

#pragma once

#include "rowguelike.hpp"
#include <stdlib.h>

using namespace rwe ;

uint8_t bird[5][8] = {
    {//
     0b00000,
     0b00000,
     0b00000,
     0b10001,
     0b01010,
     0b00100,
     0b00000,
     0b00000},
    {//
     0b00000,
     0b00000,
     0b11011,
     0b00100,
     0b00000,
     0b00000,
     0b00000,
     0b00000},
    {//
     0b00000,
     0b00000,
     0b00000,
     0b11011,
     0b00100,
     0b00000,
     0b00000,
     0b00000},
    {//
     0b00000,
     0b00000,
     0b10001,
     0b01010,
     0b00100,
     0b00000,
     0b00000,
     0b00000},
    {//
     0b00000,
     0b00000,
     0b00000,
     0b00000,
     0b00000,
     0b00000,
     0b00000,
     0b00000}
    //
};

void setupBirds()
{
    // not supported:
    if (RWE.drawContext.customCharacters == 0) {
        RWE.make().text(" Not supported   ").spawn();
        return;
    }

    RWE.drawContext.disableDirectBufferDraw = true;

    // A::Background().spawn();

    for (int i = 0; i < 5; i++)
        RWE.drawContext.defineChar(i, CustomCharacter{bird[i]});

    // birds
    for (int i = 0; i < 5; i++) {
        auto x = rand() % 16;
        auto y = rand() % 2;

        auto a = RWE.make() //
                     .text("")
                     .position(x, y)
                     //  .speed(0,0)
                     // NB: storing a value here
                     .collider(
                         0,
                         COLLIDER_FN {
                             auto &p = RWE.getPosition(receiver);
                             auto &s = RWE.getSpeed(receiver);
                             auto &v = RWE.getCollider(receiver);

                             RWE.drawContext.addChar(p.x, p.y, v.value);
                         })
                     .timer(
                         3,
                         TIMER_FN {
                             auto &p = RWE.getPosition(receiver);
                             auto &v = RWE.getCollider(receiver);
                             //  v.value = 0;

                             v.value++;
                             if (v.value % 2 == 1) {
                                 auto &s = RWE.getSpeed(receiver);

                                 RWE.drawContext.addChar(p.x, p.y, 4);

                                 p.x += s.vy;
                                 p.y += s.vy;

                                 if (p.x <= 0 || p.x >= 15)
                                     s.vx = -s.vx;
                                 if (p.y <= 0 || p.y >= 15)
                                     s.vy = -s.vy;
                             }
                             if (v.value > 3) {
                                 v.value = 0;

                                 auto &s = RWE.getSpeed(receiver);

                                 if (rand() % 10 < 3) {
                                     s.vx = rand() % 3 - 1;
                                     s.vy = rand() % 3 - 1;
                                 }
                             }
                         })
                     .spawn();

        if (a.has_value())
            RWE.getCollider(a.value()).value = rand() % 4;
    }
}
