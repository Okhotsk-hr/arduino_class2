/* Initial Author: ryand1011 (https://github.com/ryand1011)
 *
 * Reads data written by a program such as "rfid_write_personal_data.ino"
 *
 * See: https://github.com/miguelbalboa/rfid/tree/master/examples/rfid_write_personal_data
 *
 * Uses MIFARE RFID card using RFID-RC522 reader
 * Uses MFRC522 - Library
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

// rfidとLCDが両立できるか
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(4, 5, 6, 7, 8, A0);

MFRC522 rfid(SS_PIN, RST_PIN);
String datarfid = ""; // RFID UID を保存する変数

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Place your RFID card");

  lcd.begin(16, 2); // 16x2 LCD
  lcd.print("Hello World!");
}

void loop()
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

  // LCD表示
  lcd.setCursor(0, 1);
  lcd.print(datarfid);

  // 通信終了
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
