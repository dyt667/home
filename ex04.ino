// 定义引脚
const int touchPin = 4;  // 触摸传感器控制信号引脚 D4
const int ledPins[] = {2, 5, 19}; // LED引脚 D2 D5 D19
const int ledCount = 3;  // LED数量

// 触摸自锁 + 边缘检测 + 防抖
bool lastTouchState = false;
const int threshold = 700;   // 触摸阈值
bool ledState = false;       // LED总开关：false=灭 true=亮

// 呼吸灯变量
int brightness = 0;
int fadeStep = 5;
bool isFadeUp = true;

// 防抖变量
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

void setup() {
  Serial.begin(115200);
  
  // 初始化LED
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    setAllLED(LOW);
  }

  Serial.println("=== 触摸开关：摸一下亮，再摸一下灭 ===");
}

void loop() {
  // ===================== 1. 触摸检测 =====================
  int rawValue = touchRead(touchPin);
  bool currentTouchState = (rawValue < threshold);

  Serial.print("触摸值: ");
  Serial.print(rawValue);
  Serial.print(" | LED: ");
  Serial.println(ledState ? "ON" : "OFF");

  // ===================== 边缘检测 + 防抖 =====================
  if (currentTouchState != lastTouchState) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    // 检测到：按下瞬间 → 翻转LED
    if (currentTouchState == true && lastTouchState == false) {
      ledState = !ledState;
      
      if (ledState == false) {
        setAllLED(LOW); // 关灯
        brightness = 0; // 重置亮度
      }
    }
  }

  lastTouchState = currentTouchState;

  // ===================== 2. 呼吸灯逻辑（只有开才运行） =====================
  if (ledState == true) {
    if (isFadeUp) {
      brightness += fadeStep;
      if (brightness >= 255) {
        brightness = 255;
        isFadeUp = false;
      }
    } else {
      brightness -= fadeStep;
      if (brightness <= 0) {
        brightness = 0;
        isFadeUp = true;
      }
    }

    for (int i = 0; i < ledCount; i++) {
      analogWrite(ledPins[i], brightness);
    }
  }

  delay(20);
}

// 批量设置所有LED
void setAllLED(int state) {
  for (int i = 0; i < ledCount; i++) {
    digitalWrite(ledPins[i], state);
  }
}