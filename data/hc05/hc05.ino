#include <SoftwareSerial.h>

SoftwareSerial bt(5, 4); // RX, TX
const int ledPin = 13;   // LED接続ピン

String recv = "";

void setup() {
  Serial.begin(9600);  // PC用（デバッグ）
  bt.begin(9600);      // HC-05用
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println("Ready");
}

void loop() {
  while (bt.available()) {
    char c = bt.read();
    
    if (c == '\n') {
      recv.trim();
      if (recv.length() > 0) {
        Serial.print("受信: ");
        Serial.println(recv);

        // 受信したらLED点灯
        digitalWrite(ledPin, HIGH);
        delay(1000); // 1秒点灯
        digitalWrite(ledPin, LOW);

        // 返送（必要なら）
        bt.print(recv);
        bt.print("\n");
      }
      recv = "";
    } else {
      recv += c;
    }
  }
}
