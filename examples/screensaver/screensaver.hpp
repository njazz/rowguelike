/// @file screensaver.hpp
/// draws all possible characters around the screen

#pragma once

#include "rowguelike.hpp"
#include <stdlib.h>

using namespace rwe ;

void setupScreensaver(){
    
    RWE.make()
      .text("-")
      .speed(1,0)
      .timer(1,TIMER_FN{
        auto v = RWE.sharedData.getElement(0);
        v.uint8[0]++;
        RWE.sharedData.setElement(0,v);

        auto& t = RWE.getText(receiver);
        static char buf[2];
        buf[0] = v.uint8[0];
        buf[1] = 0;

        t.line[0] = buf;

        auto& p = RWE.getPosition(receiver);
        auto& s = RWE.getSpeed(receiver);
        if (p.x==15 && p.y == 0)
          {s.vx = 0; s.vy=1;}
        if (p.x==15 && p.y == 1)
           {s.vx = -1; s.vy=0;}
        if (p.x==0 && p.y == 1)
           {s.vx = 0; s.vy=-1;}
        if (p.x==0 && p.y == 0)
           {s.vx = 1; s.vy=0;}

        
      })
      .spawn();
}
