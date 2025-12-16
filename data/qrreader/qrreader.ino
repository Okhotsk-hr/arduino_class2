#include <SoftwareSerial.h>

SoftwareSerial extSerial(2, 3);  // RX, TX

void setup() {
  Serial.begin(9600);       // PC表示用
  extSerial.begin(115200);  // 外部部品の通信速度
  Serial.println("start\n");
}

void loop() {
  if (extSerial.available()) {
    //最初
    String msg = extSerial.readStringUntil('\n');
    Serial.println(msg);

    //char
    // char c = extSerial.read();
    // Serial.write(c);
  }
}