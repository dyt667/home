// 定义LED引脚
const int ledPins[] = {2, 5, 19};
const int ledCount = 3;

// 时间参数（单位：毫秒）
const unsigned long SHORT_FLASH = 200;  // 短闪持续时间
const unsigned long LONG_FLASH = 600;   // 长闪持续时间
const unsigned long GAP_FLASH = 200;    // 闪与闪之间的熄灭间隔
const unsigned long GAP_LETTER = 400;   // 字母(S/O/S)之间的停顿
const unsigned long GAP_WORD = 1500;    // 完整SOS播放完后的长停顿

// 定义SOS的动作序列
// 1代表短闪(S), 2代表长闪(O), 0代表序列结束
const int sosSequence[] = {
  1, 1, 1, 0,         // S (3短) + 字母间隔
  2, 2, 2, 0,         // O (3长) + 字母间隔
  1, 1, 1, 0          // S (3短) + 字母间隔
};
const int seqLength = sizeof(sosSequence) / sizeof(sosSequence[0]);

int currentStep = 0;           // 当前执行到序列的第几步
unsigned long previousMillis = 0; // 记录上一次状态改变的时间
bool ledState = LOW;           // 当前LED是亮还是灭

void setup() {
  // 初始化所有LED引脚为输出模式
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // 如果当前步骤超出了序列长度，说明一轮SOS播放完毕，进入长停顿
  if (currentStep >= seqLength) {
    // 确保灯是灭的
    setLeds(LOW);
    
    // 等待长停顿时间
    if (currentMillis - previousMillis >= GAP_WORD) {
      currentStep = 0; // 重置，准备下一轮SOS
      previousMillis = currentMillis;
      ledState = HIGH; // 下一轮开始先点亮
    }
    return;
  }

  int action = sosSequence[currentStep];

  // 动作0代表当前字母结束，需要停顿一下进入下一个字母
  if (action == 0) {
    setLeds(LOW); // 保持熄灭
    if (currentMillis - previousMillis >= GAP_LETTER) {
      currentStep++; // 进入下一个字母
      ledState = HIGH; // 下一个字母开始，先点亮
      previousMillis = currentMillis;
    }
    return;
  }

  // 处理亮灯和灭灯的时间控制
  unsigned long flashDuration = (action == 1) ? SHORT_FLASH : LONG_FLASH;

  if (ledState == HIGH) {
    // 当前是点亮状态
    setLeds(HIGH);
    if (currentMillis - previousMillis >= flashDuration) {
      ledState = LOW; // 时间到了，转为灭灯状态
      previousMillis = currentMillis;
    }
  } else {
    // 当前是熄灭状态（闪烁之间的间隔）
    setLeds(LOW);
    if (currentMillis - previousMillis >= GAP_FLASH) {
      ledState = HIGH; // 间隔结束，转为点亮状态
      currentStep++;   // 准备执行序列中的下一个动作
      previousMillis = currentMillis;
    }
  }
}

// 辅助函数：同时控制所有LED
void setLeds(bool state) {
  for (int i = 0; i < ledCount; i++) {
    digitalWrite(ledPins[i], state);
  }
}
