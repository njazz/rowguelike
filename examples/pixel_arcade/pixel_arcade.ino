#include "r_lcd.hpp"
#include "pixel_arcade.hpp"

RowguelikeLCD rlcd {8,9,4,5,6,7};

void setup() {
  rlcd.setup(16,2);
  setupPixelArcade();
}

void loop() {
  rlcd.loop(33);
}
