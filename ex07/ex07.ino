#include <WiFi.h>
#include <WebServer.h>

// ===================== WiFi 配置（你指定的）=====================
const char* ssid = "桃予柠";       // 你的WiFi名称
const char* password = "00000000"; // 你的WiFi密码

// ===================== 引脚定义 =====================
const int LED_PIN_1 = 2;    // D2
const int LED_PIN_2 = 5;    // D5
const int LED_PIN_3 = 19;   // D19
const int touchPin = 4;     // D4 触摸引脚
const int threshold = 700;  // 触摸阈值

// ===================== 全局状态 =====================
bool isArmed = false;    // 是否布防
bool isAlarming = false; // 是否报警中

WebServer server(80);

// ===================== 网页 =====================
String index_html = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 安防主机</title>
<style>
  body{text-align:center; font-family:Arial; margin-top:50px;}
  .btn{
    width:200px; height:70px; font-size:22px;
    margin:20px; border:none; border-radius:10px; color:white;
  }
  .arm{background:#ff3333;}
  .disarm{background:#33cc33;}
  .status{font-size:24px; margin:20px;}
</style>
</head>
<body>
  <h1>ESP32 安防报警系统</h1>
  <div class="status" id="status">状态：未布防</div>
  
  <button class="btn arm" onclick="arm()">布防 (Arm)</button>
  <button class="btn disarm" onclick="disarm()">撤防 (Disarm)</button>

  <script>
    function arm(){
      fetch("/arm");
      document.getElementById("status").innerHTML = "状态：已布防";
    }
    function disarm(){
      fetch("/disarm");
      document.getElementById("status").innerHTML = "状态：未布防";
    }
  </script>
</body>
</html>
)HTML";

// ===================== 路由处理 =====================
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleArm() {
  isArmed = true;
  isAlarming = false;
  Serial.println("系统已布防");
  server.send(200, "text/plain", "Armed");
}

void handleDisarm() {
  isArmed = false;
  isAlarming = false;
  analogWrite(LED_PIN_1, 0);
  analogWrite(LED_PIN_2, 0);
  analogWrite(LED_PIN_3, 0);
  Serial.println("系统已撤防");
  server.send(200, "text/plain", "Disarmed");
}

// 控制所有LED
void setAllLED(int value) {
  analogWrite(LED_PIN_1, value);
  analogWrite(LED_PIN_2, value);
  analogWrite(LED_PIN_3, value);
}

// ===================== setup =====================
void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(touchPin, INPUT);
  
  setAllLED(0);

  // WiFi 连接
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_STA);
  
  Serial.print("连接 WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int timeout = 20;
  while (WiFi.status() != WL_CONNECTED && timeout-- > 0) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi 连接成功！");
    Serial.print("网页地址: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi 连接失败！");
  }

  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  
  server.begin();
  Serial.println("Web 服务器已启动");
}

// ===================== loop =====================
void loop() {
  server.handleClient();

  // 布防状态下检测触摸
  if (isArmed && !isAlarming) {
    int touchValue = touchRead(touchPin);
    if (touchValue < threshold) {
      isAlarming = true;
      Serial.println("!!! 触摸报警触发 !!!");
    }
  }

  // 报警闪烁（必须撤防才会停止）
  if (isAlarming) {
    setAllLED(255);
    delay(100);
    setAllLED(0);
    delay(100);
  }

  delay(50);
}