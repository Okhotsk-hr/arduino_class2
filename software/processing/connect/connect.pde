import processing.serial.*;

Serial bt;

// テキストボックス用
String inputText = "";
boolean active = false;  // テキストボックスが選択されているか

void setup() {
    size(400, 200);
    
    println(Serial.list());
    
    bt = new Serial(this, "COM13", 9600);
    bt.bufferUntil('\n');
    
    println("接続しました");
}

void draw() {
    background(240);
    
    //テキストボックス描画
    fill(255);
    stroke(active ? color(0, 150, 255) : 0);
    rect(50, 80, 300, 30);
    
    //入力文字表示
    fill(0);
    textSize(16);
    text(inputText, 55, 102);
    
    //説明
    fill(0);
    textSize(12);
    text("クリックして入力 → Enterで送信（qで終了）", 50, 60);
}

/*
* マウスでテキストボックスを選択
*/
void mousePressed() {
    if (mouseX > 50 && mouseX < 350 && mouseY > 80 && mouseY < 110) {
        active = true;
    } else {
            active = false;
        }
        }
            
            /*
            * キーボード入力
            */
           void keyPressed() {
                if (!active) return;
                
                if (key == ENTER || key == RETURN) {
                    if (inputText.equals("q")) {
                        println("終了");
                        bt.stop();
                        exit();
                    }
                    
                    bt.write(inputText + "\n");
                    println("送信:" + inputText);
                    
                    inputText = "";
                }
                  else if (key == BACKSPACE) {
                        if (inputText.length() > 0) {
                            inputText = inputText.substring(0, inputText.length() - 1);
                        }
                    }
                      else if (key >= 32 && key <= 126) {
                           inputText += key;
                        }
                        }
                            
                            /*
                            * Arduino → Processing
                            */
                       void serialEvent(Serial p) {
                                String recv = p.readStringUntil('\n');
                           if(recv != null) {
                                    recv = trim(recv);
                                  println("受信: " + recv);
                                }
                                }
                                    