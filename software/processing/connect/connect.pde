import processing.serial.*;

Serial arduino;
String received = "";

void setup() {
    size(400, 200);
    
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
    text("Received value:", 20, 60);
    text(received, 20, 110);
    
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

// Arduino → Processing（受信のみ）
void serialEvent(Serial p) {
    received = trim(p.readString());
}
