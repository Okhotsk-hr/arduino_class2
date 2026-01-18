# 情報処理演習 Ⅱ の授業の制作物
# ガチなレジのおもちゃ

## 使用部品
- バーコードリーダー
- RFID RC522
- ~~HC-05~~
- (LCD)
- ~~Android~~
- （フルカラーLED PL9823-F8）

## 手順
1. バーコード読み取り
2. 値段集計
3. タッチ決済




## 端子
- バーコードリーダー<br>
RX 2<br>
TX 3

- RC522<br>
RST          9<br>
SDA(SS)      10<br>
MOSI         11<br>
MISO         12<br>
SCK          13<br>

- ~~HC-05<br>~~
RX 4<br>
TX 5

- フルカラーLED（PL9823-F8）<br>

## 断念
バーコード/QRコードを読み取り、読み取り結果をidとして使用<br>
idをAndroidへ送信<br>
**Android**<br>
  受信したidでjsonから商品の値段を検索して画面に表示<br>
  全て読み取ったら決済ボタンを押す<br>
  Arduinoへ信号を送信/決済画面を出す<br>
信号を受信<br>
カードの読み取りモード<br>
カード(NFC)を読み取り<br>
idをAndroidへ送信<br>
  カードのidを受信<br>
  idでjsonからカード情報を取得<br>
  残高が足りれば決済/足りなければエラー<br>
  結果をArduinoへ送信<br>
結果を受信<br>
購入が成功したらLEDを青に/失敗したらLEDを赤に点灯
