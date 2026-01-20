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
String receivedFromPC = ""; // PCから受信したデータ
String previousPrice = "";  // 前回受信した料金
int totalPrice = 0;         // 料金の累計
int currentPrice = 0;       // 現在の料金
int cardBalance = 0;        // カード残高（mode2表示用）

// A2端子のタクトスイッチ
#define SWITCH_PIN A2
int mode = 0;                     // モード変数
bool prevSwitchState = false;     // 前フレームのスイッチ状態
const int DEBOUNCE_DELAY = 50;    // デバウンス時間（ミリ秒）
unsigned long lastSwitchTime = 0; // 最後にスイッチが押された時刻

// 関数プロトタイプ
void read2d();
void readrfid();
void displaylcd();
void receiveFromPC();

void setup()
{
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("Place your RFID card");

    lcd.begin(16, 2); // 16x2 LCD
    lcd.print("Hello World!");

    extSerial.begin(115200);

    // A2端子をタクトスイッチの入力として設定
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    prevSwitchState = false;
}

void loop()
{
    switch (mode)
    {
    case 0:
        // 初期モード
        lcd.setCursor(10, 0);
        lcd.print("mode0");
        break;
    case 1:
        // 商品読み取りモード
        read2d();
        lcd.setCursor(10, 0);
        lcd.print("mode1");
        break;
    case 2:
        // 支払いモード
        readrfid();
        lcd.setCursor(10, 0);
        lcd.print("mode2");

        break;
    }
    displaylcd();
    detectTactSwitch(); // A2端子のスイッチを検知

    // Serial.println("fromArduino");
    receiveFromPC();
}

// バーコード読み込み関数
void read2d()
{
    if (extSerial.available())
    {
        // バーコードデータを読み込み
        String msg = extSerial.readStringUntil('\n');
        data2d = msg; // data2d変数に保存
        Serial.println(data2d);
        lcd.clear();
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
    Serial.println(datarfid);

    // 通信終了
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

// LCD表示関数
void displaylcd()
{
    // LCDをクリア（ちらつき防止のため条件付き）
    static unsigned long lastClear = 0;
    if (millis() - lastClear > 500)
    {
        lcd.clear();
        lastClear = millis();
    }

    // PC から受信したデータを表示
    if (receivedFromPC != "")
    {
        switch (mode)
        {
        case 1:
            lcd.setCursor(0, 0);
            lcd.print("P:");
            lcd.setCursor(2, 0);
            lcd.print(currentPrice);

            lcd.setCursor(0, 1);
            lcd.print("T:");
            lcd.setCursor(2, 1);
            lcd.print(totalPrice);
            break;
        case 2:
            lcd.setCursor(0, 0);
            lcd.print("T:");
            lcd.setCursor(2, 0);
            lcd.print(totalPrice);

            lcd.setCursor(0, 1);
            lcd.print("B:");
            lcd.setCursor(2, 1);
            lcd.print(cardBalance);
            break;
        }
    }
    else
    {
        // 受信がない場合はバーコードと RFID を表示
        lcd.setCursor(0, 0);
        lcd.print(data2d);
        lcd.setCursor(0, 1);
        lcd.print(datarfid);
    }
}

// A2端子のタクトスイッチを検知する関数
void detectTactSwitch()
{
    // A2端子の現在の状態を読み込み（LOW = 押された、HIGH = 押されていない）
    bool currentSwitchState = digitalRead(SWITCH_PIN);

    // デバウンス処理
    if (millis() - lastSwitchTime < DEBOUNCE_DELAY)
    {
        return;
    }

    // スイッチが離された瞬間を検出（前回LOW→現在HIGH）
    if (prevSwitchState == LOW && currentSwitchState == HIGH)
    {
        mode++; // modeを1つ進める

        // mode4になったら0に戻す
        if (mode == 4)
        {
            mode = 0;
        }

        // mode0になったときに料金関連をリセット
        if (mode == 0)
        {
            currentPrice = 0;
            totalPrice = 0;
            cardBalance = 0;
            Serial.println("Reset: currentPrice, totalPrice, cardBalance");
        }

        lastSwitchTime = millis();

        Serial.print("Switch detected! Mode changed to: ");
        Serial.println(mode);
    }

    prevSwitchState = currentSwitchState;
}

// PC（Processing）からの受信関数
void receiveFromPC()
{
    if (Serial.available())
    {
        receivedFromPC = Serial.readStringUntil('\n');
        Serial.println("Received from PC: " + receivedFromPC);

        // 「現在の料金,合計料金」の形式でパース
        int commaIndex = receivedFromPC.indexOf(',');
        if (commaIndex > 0)
        {
            String firstStr = receivedFromPC.substring(0, commaIndex);
            String secondStr = receivedFromPC.substring(commaIndex + 1);

            // mode2: 「総額,カード残高」を受信して表示
            if (mode == 2)
            {
                totalPrice = firstStr.toInt();
                cardBalance = secondStr.toInt();
            }
            else
            {
                // mode1等: 「現在の料金,合計料金」
                currentPrice = firstStr.toInt();
                totalPrice = secondStr.toInt();
            }
        }
        else
        {
            // カンマがない場合は従来通り処理
            if (receivedFromPC != previousPrice && receivedFromPC != "")
            {
                currentPrice = receivedFromPC.toInt();
                totalPrice += currentPrice;
                previousPrice = receivedFromPC;

                Serial.print("Current Price: ");
                Serial.print(currentPrice);
                Serial.print(", Total Price: ");
                Serial.println(totalPrice);
            }
        }
    }
}