#include "r_lcd.hpp"
#include "horizontal_space.hpp"

RowguelikeLCD rlcd {8,9,4,5,6,7};

void setup() {
  rlcd.setup(16,2);
  setupHorizontalSpace();
}

void loop() {
  rlcd.loop(100);
}