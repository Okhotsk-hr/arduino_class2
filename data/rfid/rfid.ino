#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Place your RFID card");
}

void loop() {
  // 新しいカードがあるか
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // カードのUIDを読み取る
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // UID表示
  Serial.print("UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // 通信終了
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
