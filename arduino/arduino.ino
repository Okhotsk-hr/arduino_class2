#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

// ピン設定
#define SS_PIN 10
#define RST_PIN 9
#define LCD_RS 4
#define LCD_E 5
#define LCD_D4 6
#define LCD_D5 7
#define LCD_D6 8
#define LCD_D7 A0
#define BARCODE_RX 2
#define BARCODE_TX 3
#define BARCODE_BAUD 115200
#define SERIAL_BAUD 9600

// 通信インスタンス
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
MFRC522 rfid(SS_PIN, RST_PIN);
SoftwareSerial extSerial(BARCODE_RX, BARCODE_TX);

// データ変数
String data2d = "";     // バーコードデータ
String datarfid = "";   // RFID UIDデータ

// 関数プロトタイプ
void initSystems();
void readBarcode();
void readRFID();
void updateLCD();

void setup()
{
    initSystems();
}

void loop()
{

    if (extSerial.available())
    {
        // バーコードデータを読み込み
        String msg = extSerial.readStringUntil('\n');
        data2d = msg; // data2d変数に保存
        Serial.println("Barcode: " + data2d);
    }
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

    // LCD表示
    lcd.setCursor(0, 0);
    lcd.print(data2d);
    lcd.setCursor(0, 1);
    lcd.print(datarfid);

    // 通信終了
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}
