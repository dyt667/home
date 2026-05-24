// 定义引脚
const int touchPin = 4;  // 触摸传感器控制信号引脚 D4
const int ledPins[] = {2, 5, 19}; // LED引脚 D2 D5 D19
const int ledCount = 3;  // LED数量

// 触摸自锁 + 档位切换变量
bool lastTouchState = false; // 上一次触摸状态
const int threshold = 700;   // 触摸阈值
int speedGear = 1;           // 呼吸灯速度档位：1=慢 2=中 3=快

// 呼吸灯变量
int brightness = 0;    // LED亮度
int fadeStep = 5;      // 亮度变化步长
bool isFadeUp = true;  // 呼吸方向：true=变亮 false=变暗

void setup() {
  // 串口波特率 115200
  Serial.begin(115200);
  
  // 初始化LED引脚
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    analogWrite(ledPins[i], 0); // 初始熄灭
  }
  
  // 初始化触摸引脚
  pinMode(touchPin, INPUT);
  Serial.println("=== 3档触摸调速呼吸灯已启动 ===");
}

void loop() {
  // ===================== 1. 触摸检测 + 档位切换 =====================
  int rawValue = touchRead(touchPin); 
  Serial.print("触摸数值: ");
  Serial.print(rawValue);
  Serial.print("  | 当前档位: ");
  Serial.println(speedGear);

  bool currentTouchState = (rawValue < threshold);

  // 边缘检测：触摸瞬间切换档位
  if (currentTouchState == true && lastTouchState == false) {
    speedGear++;          // 档位+1
    if (speedGear > 3) {  // 3档循环：1→2→3→1
      speedGear = 1;
    }
    delay(50); // 防抖
  }
  lastTouchState = currentTouchState;

  // ===================== 2. 根据档位设置呼吸速度 =====================
  int delayTime;  // 延时时间（控制呼吸快慢）
  switch (speedGear) {
    case 1:
      delayTime = 30;   // 1档：慢速呼吸
      break;
    case 2:
      delayTime = 15;   // 2档：中速呼吸
      break;
    case 3:
      delayTime = 5;    // 3档：快速呼吸
      break;
    default:
      delayTime = 30;
  }

  // ===================== 3. 呼吸灯核心PWM逻辑 =====================
  if (isFadeUp) {
    brightness += fadeStep;   // 亮度递增
    if (brightness >= 255) {  // 达到最亮
      brightness = 255;
      isFadeUp = false;       // 切换为递减
    }
  } else {
    brightness -= fadeStep;   // 亮度递减
    if (brightness <= 0) {    // 达到最暗
      brightness = 0;
      isFadeUp = true;        // 切换为递增
    }
  }

  // 给所有LED设置亮度
  for (int i = 0; i < ledCount; i++) {
    analogWrite(ledPins[i], brightness);
  }

  delay(delayTime); // 档位控制延时，改变呼吸速度
}