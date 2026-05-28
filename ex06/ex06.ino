#include <WiFi.h>
#include <WebServer.h>

// ================= 配置区域 =================
const char* ssid = "桃予柠";
const char* password = "00000000";

// LED 引脚 (D2, D5, D19)
const int LED_PIN_1 = 2;
const int LED_PIN_2 = 5;
const int LED_PIN_3 = 19;

WebServer server(80);
int currentBrightness = 0;

// ================= HTML 网页 =================
String index_html = R"EOF(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 无极调光</title>
    <style>
        body{font-family: Arial; text-align: center; background: #f4f4f4; padding: 30px;}
        .container{background: white; padding: 40px; border-radius: 15px; display: inline-block;}
        h1{color: #333;}
        .slider{width: 350px; height: 25px; margin: 20px 0;}
        #value{font-size: 28px; font-weight: bold; color: #007bff;}
    </style>
</head>
<body>
    <div class="container">
        <h1>LED 无极调光器</h1>
        <input type="range" min="0" max="255" value="0" class="slider" id="slider" oninput="update()">
        <p>当前亮度：<span id="value">0</span></p>
    </div>

    <script>
        function update() {
            var val = document.getElementById("slider").value;
            document.getElementById("value").innerHTML = val;
            fetch("/set?b=" + val);
        }
    </script>
</body>
</html>
)EOF";

// ================= 服务器函数 =================
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleSet() {
  if (server.hasArg("b")) {
    currentBrightness = server.arg("b").toInt();
    
    // 限制亮度 0-255
    if (currentBrightness < 0) currentBrightness = 0;
    if (currentBrightness > 255) currentBrightness = 255;

    // 通用 analogWrite 控制三路LED
    analogWrite(LED_PIN_1, currentBrightness);
    analogWrite(LED_PIN_2, currentBrightness);
    analogWrite(LED_PIN_3, currentBrightness);

    Serial.print("网页设置亮度：");
    Serial.println(currentBrightness);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  // 初始化LED引脚
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  
  // 初始关灯
  analogWrite(LED_PIN_1, 0);
  analogWrite(LED_PIN_2, 0);
  analogWrite(LED_PIN_3, 0);

  // 连接 WiFi
  Serial.print("正在连接 WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi 连接成功！");
  Serial.print("控制面板地址: ");
  Serial.println(WiFi.localIP());

  // 服务器路由
  server.on("/", handleRoot);
  server.on("/set", handleSet);

  // 启动服务器
  server.begin();
  Serial.println("Web 服务器已启动");
}

void loop() {
  server.handleClient();
}
