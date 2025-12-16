#include <IRremote.h>

#define IR_RECEIVE_PIN 11  // IR受信機のOUTピン

uint32_t lastCode = 0;  // 前回のコード

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
      switch (code) {
        case 0xFFA25D: Serial.println("POWER"); break;
        case 0xFFE21D: Serial.println("FUNC / STOP"); break;
        case 0xFF629D: Serial.println("VOL +"); break;
        case 0xFFA857: Serial.println("VOL -"); break;
        case 0xFF906F: Serial.println("UP"); break;
        case 0xFFE01F: Serial.println("DOWN"); break;
        case 0xE916FF00: Serial.println("0"); break;
        case 0xF30CFF00: Serial.println("1"); break;
        case 0xE718FF00: Serial.println("2"); break;
        case 0xA15EFF00: Serial.println("3"); break;
        case 0xF708FF00: Serial.println("4"); break;
        case 0xE31CFF00: Serial.println("5"); break;
        case 0xA55AFF00: Serial.println("6"); break;
        case 0xBD42FF00: Serial.println("7"); break;
        case 0xAD52FF00: Serial.println("8"); break;
        case 0xB54AFF00: Serial.println("9"); break;
        default:
          Serial.print("UNKNOWN: 0x");
          Serial.println(code, HEX);
          break;
      }
      // Serial.print("Received: 0x");
      // Serial.println(code, HEX);

      lastCode = code;  // 今回のコードを保存
    }

    IrReceiver.resume();  // 次の受信へ
  }
}
