import processing.serial.*;

Serial arduino;
String received = "";
PFont japaneseFont;
Table priceTable; // price.csvを保存するテーブル
Table cardTable;  // card.csvを保存するテーブル
String foundPrice = ""; // 検索結果のプライス
String current2d = ""; // 現在のバーコードID
String previous2d = ""; // 1つ前のバーコードID
int receivemode = 0;
int totalPrice = 0; // 合計料金

void setup() {
    size(400, 200);
    
    // 日本語フォント読み込み
    japaneseFont = createFont("MS Gothic", 24);
    textFont(japaneseFont);
    
    // price.csvを読み込み
    priceTable = loadTable("price.csv", "header");
    if (priceTable != null) {
        println("price.csv loaded successfully. Rows: " + priceTable.getRowCount());
        
        // CSV内容をデバッグ出力
        println("=== CSV内容 ===");
        for (TableRow row : priceTable.rows()) {
            String id = row.getString("id");
            String price = row.getString("price");
            println("ID: [" + id + "], Price: [" + price + "]");
        }
        println("===============");
    } else {
        println("ERROR: Could not load price.csv");
    }
    
    // card.csvを読み込み（RFID UID 用）
    cardTable = loadTable("card.csv", "header");
    if (cardTable != null) {
        println("card.csv loaded successfully. Rows: " + cardTable.getRowCount());
        
        // CSV内容をデバッグ出力
        println("=== card.csv 内容 ===");
        for (TableRow row : cardTable.rows()) {
            String id = row.getString("id");
            String balance = row.getString("balance");
            println("ID: [" + id + "], Balance: [" + balance + "]");
        }
        println("====================");
    } else {
        println("ERROR: Could not load card.csv");
    }
    
    //利用可能なシリアルポート表示
    println(Serial.list());
    
    //COM番号は環境に合わせて変更
    arduino = new Serial(this, "COM14", 9600);
    arduino.bufferUntil('\n');
}

void draw() {
    background(240);
    fill(0);
    textSize(24);
    textFont(japaneseFont);
    text("受信値:", 20, 60);
    text(received, 20, 110);
    
    // 検索結果を表示
    textSize(16);
    text("検索結果: " + foundPrice, 20, 140);
    
    // ボタン表示
    fill(0, 100, 255);
    rect(20, 150, 150, 30);
    fill(255);
    textSize(16);
    text("送信: 1,000", 40, 172);
}

// マウスクリック時にArduinoに送信
void mousePressed() {
    if (mouseX > 20 && mouseX < 170 && mouseY > 150 && mouseY < 180) {
        arduino.write("1000\n");
        println("送信: 1000");
    }
}

// Arduino → Processing（受信）
void serialEvent(Serial p) {
    received = trim(p.readString());
    
    println("=== シリアル受信 ===");
    println("受信データ: [" + received + "]");
    println("データ長: " + received.length());
    
    switch(receivemode) {
        case 0:
            searchAndSendPrice(received);
            break;
        default:
        println("不明な受信モード: " + receivemode);
        break;
    }
}

// CSVからIDで検索してpriceをArduinoに送信
void searchAndSendPrice(String id) {
    // 1つ前のバーコードIDを記録
    previous2d = current2d;
    current2d = id;
    
    // 余分な空白や改行を削除
    id = id.trim();
    
    boolean isRfid = isRfidUid(id);
    Table targetTable = isRfid ? cardTable : priceTable;
    String valueColumn = isRfid ? "balance" : "price";
    if (targetTable == null) {
        println("ERROR: " + (isRfid ? "card.csv" : "price.csv") + " not loaded");
        return;
    }
    
    println("=== 検索開始 ===");
    println("検索ID: [" + id + "]");
    println("ID長: " + id.length());
    println("検索対象CSV: " + (isRfid ? "card.csv" : "price.csv"));
    println("CSV行数: " + targetTable.getRowCount());
    
    foundPrice = "Not Found";
    
    // CSVの全行を検索
    for (TableRow row : targetTable.rows()) {
        String csvId = row.getString("id");
        csvId = csvId.trim(); // CSVのIDも trim
        
        println("比較: CSV[" + csvId + "] == 受信[" + id + "] -> " + csvId.equals(id));
        
        if (csvId.equals(id)) {
            String value = row.getString(valueColumn);
            foundPrice = value;
            
            if (isRfid) {
                // RFIDは残高から合計を引いて送信
                int balance = parseIntSafe(value);
                int remaining = balance - totalPrice;
                println("=== RFID 決済 ===");
                println("UID: " + id);
                println("残高: " + balance);
                println("合計料金: " + totalPrice);
                println("差引後残高: " + remaining);
                println("================");
                
                // CSV上の残高を更新して保存
                row.setString(valueColumn, str(remaining));
                saveTable(cardTable, "card.csv");
                println("card.csv updated: " + id + " -> " + remaining);
                
                // Arduinoに「合計料金,差引後残高」を送信
                arduino.write(totalPrice + "," + remaining + "\n");
                foundPrice = str(remaining);
                
                // 決済完了後、総額をリセット
                totalPrice = 0;
                println("総額をリセット: " + totalPrice);
            } else {
                int currentPrice = Integer.parseInt(value);
                totalPrice += currentPrice;
                
                // コンソール出力
                println("=== 検索成功！ ===");
                println("バーコードID: " + id);
                println("現在の料金: " + currentPrice);
                println("合計料金: " + totalPrice);
                println("==================");
                
                // Arduinoに「現在の料金,合計料金」を送信
                arduino.write(currentPrice + "," + totalPrice + "\n");
            }
            return;
        }
    }
    
    println("=== 検索失敗 ===");
    println("ID not found in CSV: [" + id + "]");
}

// RFIDのUIDかどうかを判定（例: "64 1b 9c 04"）
boolean isRfidUid(String id) {
    String trimmed = id.trim();
    return trimmed.matches("(?i)^[0-9a-f]{2}( [0-9a-f]{2}){3}$");
}

// 数値変換の安全版（空文字やnullを0にする）
int parseIntSafe(String value) {
    try {
        return Integer.parseInt(value.trim());
    } catch(Exception e) {
        return 0;
    }
}
