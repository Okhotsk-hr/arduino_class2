#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_PIN 7    // 外付けLED（D7）

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(9600);
  while (!Serial);   // UNOでは省略可

  Serial.println("=== RC522 Card Read Check ===");

  SPI.begin();
  mfrc522.PCD_Init();

  // RC522 接続チェック
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print("RC522 Version: 0x");
  Serial.println(v, HEX);

  if (v == 0x91 || v == 0x92) {
    Serial.println("RC522 detected");
  } else {
    Serial.println("RC522 NOT detected");
  }

  Serial.println("Put your card...");
}

void loop() {
  // カードがなければ何もしない
  if (!mfrc522.PICC_IsNewCardPresent()) {
    digitalWrite(LED_PIN, LOW);
    return;
  }

  // カードのUIDを読めなければ失敗
  if (!mfrc522.PICC_ReadCardSerial()) {
    digitalWrite(LED_PIN, LOW);
    return;
  }

  // === ここに来たら読み取り成功 ===
  digitalWrite(LED_PIN, HIGH);

  Serial.print("Card UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(" ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // 通信終了処理
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(1000);              // 連続読み取り防止
  digitalWrite(LED_PIN, LOW);
}
