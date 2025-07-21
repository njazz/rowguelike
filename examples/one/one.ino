#include "r_lcd.hpp"
#include "one.hpp"

RowguelikeLCD rlcd {8,9,4,5,6,7};

void setup() {
  rlcd.setup(16,2);
  setupOne();
}

void loop() {
  rlcd.loop(150);
}
