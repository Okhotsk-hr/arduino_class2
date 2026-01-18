#include <LiquidCrystal.h>

// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(4, 5, 6, 7, 8, A0);

void setup() {
  lcd.begin(16, 2);  // 16x2 LCD
  lcd.print("Hello World!");
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
}
