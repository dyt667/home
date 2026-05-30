// 定义两个LED引脚 (改为 19 和 5)
const int ledPinA = 19; // 灯A：D19
const int ledPinB = 5;  // 灯B：D5

// PWM 配置
const int freq = 1000;      // 降低频率到 1000Hz，通常能让LED更亮
const int resolution = 8;   // 8位分辨率 (0-255)

// 亮度控制变量
int brightness = 0;   // 当前亮度 (0-255)
int fadeStep = 5;     // 亮度变化步长

void setup() {
  // 初始化两个独立的PWM通道
  ledcAttach(ledPinA, freq, resolution); 
  ledcAttach(ledPinB, freq, resolution); 
}

void loop() {
  // 核心逻辑：反相输出
  // 灯A (D19) 的亮度 = 当前亮度值
  ledcWrite(ledPinA, brightness);
  
  // 灯B (D5) 的亮度 = 255 - 当前亮度值
  ledcWrite(ledPinB, 255 - brightness);

  // 改变下一次的亮度
  brightness = brightness + fadeStep;

  // 到达极值（最亮255或最暗0）时，反转变化方向
  if (brightness <= 0 || brightness >= 255) {
    fadeStep = -fadeStep; 
  }

  // 控制渐变的速度
  delay(20); 
}
