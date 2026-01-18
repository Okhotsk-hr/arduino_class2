#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>

SoftwareSerial extSerial(2, 3); // RX, TX
const int speed2d = 115200;     // バーコードリーダーの通信速度

// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(4, 5, 6, 7, 8, A0); // lcd

// rfid
#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);

// 通信データを保存する文字列変数
String data2d = "";
String datarfid = "";

void setup()
{
    Serial.begin(9600);       // PC表示用
    extSerial.begin(speed2d); // 外部部品の通信速度
    delay(1000);              // 初期化待ち
    Serial.println("start\n");

    lcd.begin(16, 2); // 16x2 LCD

    SPI.begin();
    rfid.PCD_Init();

    // RFID初期化確認
    byte version = rfid.PCD_ReadRegister(rfid.VersionReg);
    Serial.print("MFRC522 Version: 0x");
    Serial.println(version, HEX);

    if (version == 0x00 || version == 0xFF)
    {
        Serial.println("ERROR: RFID module not detected!");
        lcd.print("RFID Error");
    }
    else
    {
        Serial.println("RFID module initialized successfully");
        lcd.print("RFID Ready");
    }
}

void loop()
{
    if (extSerial.available())
    {
        // 方法1: 改行文字までの文字列を読み込み
        String msg = extSerial.readStringUntil('\n');
        data2d = msg; // 文字列変数に保存
        Serial.println("Received: " + data2d);
    }

    lcd.setCursor(0, 0);
    lcd.print(data2d);

    // rfid
    //  新しいカードがあるか
    if (rfid.PICC_IsNewCardPresent())
    {
        // カードのUIDを読み取る
        if (rfid.PICC_ReadCardSerial())
        {
            // UID表示
            Serial.print("UID: ");
            datarfid = ""; // リセット
            for (byte i = 0; i < rfid.uid.size; i++)
            {
                if (rfid.uid.uidByte[i] < 0x10)
                {
                    Serial.print("0");
                    datarfid += "0";
                }
                Serial.print(rfid.uid.uidByte[i], HEX);
                datarfid += String(rfid.uid.uidByte[i], HEX);
                Serial.print(" ");
                datarfid += " ";
            }
            Serial.println();
            Serial.println("Saved UID: " + datarfid);

            lcd.setCursor(0, 1);
            lcd.print(datarfid);

            // 通信終了
            rfid.PICC_HaltA();
            rfid.PCD_StopCrypto1();
        }
    }

    delay(100); // CPU負荷軽減
}