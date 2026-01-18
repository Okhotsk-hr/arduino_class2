import processing.serial.*;

Serial arduino;
String received = "";
PFont japaneseFont;
Table priceTable; // price.csvを保存するテーブル
String foundPrice = ""; // 検索結果のプライス
String current2d = ""; // 現在のバーコードID
String previous2d = ""; // 1つ前のバーコードID
int receivemode = 0;

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
    if (priceTable == null) {
        println("ERROR: price.csv not loaded");
        return;
    }
    
    // 1つ前のバーコードIDを記録
    previous2d = current2d;
    current2d = id;
    
    // 余分な空白や改行を削除
    id = id.trim();
    
    println("=== 検索開始 ===");
    println("検索ID: [" + id + "]");
    println("ID長: " + id.length());
    println("CSV行数: " + priceTable.getRowCount());
    
    foundPrice = "Not Found";
    
    // CSVの全行を検索
    for (TableRow row : priceTable.rows()) {
        String csvId = row.getString("id");
        csvId = csvId.trim(); // CSVのIDも trim
        
        println("比較: CSV[" + csvId + "] == 受信[" + id + "] -> " + csvId.equals(id));
        
        if (csvId.equals(id)) {
            String price = row.getString("price");
            foundPrice = price;
            
            // コンソール出力
            println("=== 検索成功！ ===");
            println("バーコードID: " + id);
            println("料金: " + price);
            println("==================");
            
            // Arduinoにpriceを送信
            arduino.write(price + "\n");
            return;
        }
    }
    
    println("=== 検索失敗 ===");
    println("ID not found in CSV: [" + id + "]");
}
