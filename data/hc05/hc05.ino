#include <SoftwareSerial.h>

SoftwareSerial bt(4, 5); // RX, TX
const int LED = 13;

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  bt.begin(9600);
}

void loop() {
  if (bt.available()) {
    char c = bt.read();

    if (c == '1') digitalWrite(LED, HIGH);
    if (c == '0') digitalWrite(LED, LOW);
  }
}
