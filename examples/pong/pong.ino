#include "r_lcd.hpp"
#include "pong.hpp"

RowguelikeLCD rlcd {8,9,4,5,6,7};

void setup() {
  rlcd.setup(16,2);
  setupExamplePong();
}

void loop() {
  rlcd.loop(150);
}
