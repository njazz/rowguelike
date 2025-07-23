#include "r_lcd.hpp"
#include "pages.hpp"

RowguelikeLCD rlcd {8,9,4,5,6,7};

void setup() {
  rlcd.setup(16,2);
  setupPages();
}

void loop() {
  rlcd.loop(150);
}
