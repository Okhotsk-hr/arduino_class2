#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9

// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(4, 5, 6, 7, 8, A0);

MFRC522 rfid(SS_PIN, RST_PIN);
String datarfid = ""; // RFID UID を保存する変数

// バーコードリーダー
SoftwareSerial extSerial(2, 3); // RX, TX
String data2d = "";

// 関数プロトタイプ
void read2d();
void readrfid();
void displaylcd();

void setup()
{
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("Place your RFID card");

    lcd.begin(16, 2); // 16x2 LCD
    lcd.print("Hello World!");

    extSerial.begin(115200);
}

void loop()
{
    read2d();
    readrfid();
    displaylcd();
}

// バーコード読み込み関数
void read2d()
{
    if (extSerial.available())
    {
        // バーコードデータを読み込み
        String msg = extSerial.readStringUntil('\n');
        data2d = msg; // data2d変数に保存
        Serial.println("Barcode: " + data2d);
    }
}

// RFID読み込み関数
void readrfid()
{
    // 新しいカードがあるか
    if (!rfid.PICC_IsNewCardPresent())
    {
        return;
    }

    // カードのUIDを読み取る
    if (!rfid.PICC_ReadCardSerial())
    {
        return;
    }

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

    // 通信終了
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

// LCD表示関数
void displaylcd()
{
    lcd.setCursor(0, 0);
    lcd.print(data2d);
    lcd.setCursor(0, 1);
    lcd.print(datarfid);
}
