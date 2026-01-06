#include <SoftwareSerial.h>

// HC-05接続ピン
// HC-05 TXD → Arduino D4
// HC-05 RXD → Arduino D5
SoftwareSerial bt(4, 5);  // (RX, TX)

void setup() {
  Serial.begin(9600);   // USBシリアル（デバッグ用）
  bt.begin(9600);       // HC-05（Bluetooth）

  Serial.println("Arduino 起動");
}

void loop() {
  if (bt.available()) {
    String s = bt.readStringUntil('\n');
    s.trim();

    Serial.print("Bluetooth受信: ");
    Serial.println(s);

    // Bluetoothへ返事
    bt.print("受信しました: ");
    bt.println(s);
  }
}
