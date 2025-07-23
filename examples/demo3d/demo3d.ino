#define RW_WITH3D true
#define RW_SETUP_ACTORS 4 // reduce memory usage

#include "demo3d.hpp"

#include "r_lcd.hpp"

RowguelikeLCD rlcd {8,9,4,5,6,7};

void setup() {
  rlcd.setup(16,2);
  setupDemo3D();
}

void loop() {
  rlcd.loop(33);
}