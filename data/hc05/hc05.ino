#include <SoftwareSerial.h>
SoftwareSerial bt(4, 5); // RX, TX

void setup() {
  bt.begin(9600);
}

void loop() {
  if (bt.available()) {
    char c = bt.read();
    bt.write(c);  // 受信した文字をそのまま返す
  }
}
