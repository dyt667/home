// 定义引脚
const int touchPin = 4;  // 触摸传感器控制信号引脚 D4
const int ledPins[] = {2, 5, 19}; // 三个LED灯的引脚数组 D2 D5 D19
const int ledCount = 3;  // LED灯的数量

bool ledState = false;      // LED的当前状态（false为灭，true为亮）
bool lastTouchState = false; // 上一次读取到的触摸状态
const int threshold = 700;   // 触摸阈值 700

void setup() {
  // 初始化串口，波特率设置为 115200
  Serial.begin(115200);
  
  // 初始化LED引脚为输出模式
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // 初始状态全部熄灭
  }
  
  // 初始化触摸引脚为输入模式
  pinMode(touchPin, INPUT);
}

void loop() {
  // 1. 实时读取 D4 触摸原始数值
  int rawValue = touchRead(touchPin); 
  
  // 2. 实时输出原始数值
  Serial.print("D4触摸原始数值: ");
  Serial.println(rawValue);

  // --- 触摸自锁开关逻辑 ---
  bool currentTouchState = (rawValue < threshold); // 小于阈值判定为被触摸

  // 边缘检测：只有当“上一次未触摸”且“当前被触摸”时，才触发动作
  if (currentTouchState == true && lastTouchState == false) {
    ledState = !ledState; // 翻转LED的状态
    
    // 将新的状态同时写入三个LED灯
    for (int i = 0; i < ledCount; i++) {
      digitalWrite(ledPins[i], ledState);
    }
    
    delay(50); // 软件防抖延时
  }

  // 更新上一次的触摸状态
  lastTouchState = currentTouchState;
  
  delay(50);
}