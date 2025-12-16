#include <IRremote.h>

#define IR_RECEIVE_PIN 11   // IR受信機のOUTピン

uint32_t lastCode = 0;      // 前回のコード

void setup() {
  Serial.begin(9600);
  Serial.println("IR Receiver Ready");

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (IrReceiver.decode()) {

    // REPEAT や押しっぱなしを除外
    uint32_t code = IrReceiver.decodedIRData.decodedRawData;

    if (code != 0 && code != lastCode) {
      Serial.print("Received: 0x");
      Serial.println(code, HEX);

      lastCode = code;   // 今回のコードを保存
    }

    IrReceiver.resume(); // 次の受信へ
  }
}
