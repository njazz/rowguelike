/// @file demo3d.hpp
/// basic demo of "3d engine"

#pragma once

#define RW_SETUP_ACTORS 4 // reduce memory usage
#define RW_SETUP_WITH_3D true

#include "rowguelike.hpp"
#include <stdlib.h>

using namespace rwe ;

void setupDemo3D()
{
    // not supported:
    if (RWE.drawContext.customCharacters == 0) {
        RWE.make().text(" Not supported   ").spawn();
        return;
    }

    RWE.drawContext.disableDirectBufferDraw = true;

    // A::Background().spawn();

    R3D.vertex[0] = {2_fp, 2_fp, 0};
    R3D.vertex[1] = {18_fp, 2_fp, 0};
    R3D.vertex[2] = {18_fp, 14_fp, 0};
    R3D.vertex[3] = {2_fp, 14_fp, 0};

    R3D.line[0] = {0, 1};
    R3D.line[1] = {1, 2};
    R3D.line[2] = {2, 3};
    R3D.line[3] = {3, 0};

    R3D.vertex[4] = {7_fp, 6_fp, 0};
    R3D.vertex[5] = {12_fp, 6_fp, 0};

    R3D.point[0] = 4;
    R3D.point[1] = 5;

    R3D.vertex[6] = {6_fp, 8_fp, 0};
    R3D.vertex[7] = {9_fp, 10_fp, 0};
    R3D.vertex[8] = {10_fp, 10_fp, 0};
    R3D.vertex[9] = {13_fp, 8_fp, 0};

    R3D.line[4] = {6, 7};
    R3D.line[5] = {7, 8};
    R3D.line[5] = {8, 9};

    R3D.lineCount = 7;
    R3D.pointCount = 2;

    R3D.projection = Mat4x4::Identity();

    R3D.prepare();

    auto renderActor = RWE.make()
                           .position(0, 0)
                           .control()
                           .speed(1, 0)
                           .text("")
                           .eachFrame(TIMER_FN {
                               auto &p = RWE.getPosition(receiver);
                               R3D.render();

                               for (int i = 0; i < 8; i++)
                                   RWE.drawContext.defineChar(i, CustomCharacter{R3D.buffer[i]});

                               RWE.drawContext.clearAll();

                               for (auto x = 0; x < 4; x++)
                                   for (auto y = 0; y < 4; y++) {
                                       RWE.drawContext.addChar(x + p.x, y, x + y * 4);
                                   }
                           })
                           .spawn();
}
