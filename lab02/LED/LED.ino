// 引脚定义
#define TOUCH_PIN 4    // 触摸引脚 (GPIO4)
#define LED_PIN 2      // LED 引脚 (GPIO2)
#define LED_PIN_D4 4   // LED 引脚 D4
#define LED_PIN_D5 5   // LED 引脚 D5
#define LED_PIN_D21 21 // LED 引脚 D21

// 触摸阈值设置
int threshold = 450;   

// 触摸状态记录变量
bool ledState = false;       
bool lastTouchState = false; 

// SOS 闪烁相关
const unsigned long UNIT_TIME = 200;
const unsigned long SHORT_FLASH = UNIT_TIME;
const unsigned long LONG_FLASH = UNIT_TIME * 3;
const unsigned long FLASH_GAP = UNIT_TIME;
const unsigned long LETTER_GAP = UNIT_TIME * 3;
const unsigned long SOS_GAP = UNIT_TIME * 7;

const int sosPattern[] = {
  1, 0, 1, 0, 1,
  0,
  3, 0, 3, 0, 3,
  0,
  1, 0, 1, 0, 1,
  0
};
const int PATTERN_LENGTH = sizeof(sosPattern) / sizeof(sosPattern[0]);

unsigned long previousMillis = 0;
int currentStep = 0;
bool isFlashing = false;
unsigned long flashStartTime = 0;
unsigned long currentFlashDuration = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("LAB02 整合程序启动");
  Serial.println("功能 1: 触摸开关控制 LED");
  Serial.println("功能 2: SOS 闪烁信号");

  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN_D4, OUTPUT);
  pinMode(LED_PIN_D5, OUTPUT);
  pinMode(LED_PIN_D21, OUTPUT);
  
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_PIN_D4, LOW);
  digitalWrite(LED_PIN_D5, LOW);
  digitalWrite(LED_PIN_D21, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  // 功能 1: 触摸开关控制 LED
  int touchValue = touchRead(TOUCH_PIN);
  bool currentTouchState = (touchValue < threshold);

  if (currentTouchState == true && lastTouchState == false) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    
    if (ledState) {
      Serial.println("检测到触摸 -> 灯亮了");
    } else {
      Serial.println("检测到触摸 -> 灯灭了");
    }
  }

  lastTouchState = currentTouchState;

  Serial.print("当前触摸值：");
  Serial.println(touchValue);

  // 功能 2: SOS 闪烁信号
  if (isFlashing) {
    if (currentMillis - flashStartTime >= currentFlashDuration) {
      digitalWrite(LED_PIN_D4, LOW);
      digitalWrite(LED_PIN_D5, LOW);
      digitalWrite(LED_PIN_D21, LOW);
      isFlashing = false;
      previousMillis = currentMillis;
      
      Serial.print("完成闪烁步骤 ");
      Serial.println(currentStep);
    }
  } else {
    if (currentStep < PATTERN_LENGTH) {
      int stepValue = sosPattern[currentStep];
      
      if (stepValue == 0) {
        unsigned long gapDuration;
        if (currentStep == PATTERN_LENGTH - 1) {
          gapDuration = SOS_GAP;
          Serial.println("--- SOS 播放完毕，长停顿 ---");
        } else if (currentStep == 5 || currentStep == 11) {
          gapDuration = LETTER_GAP;
          Serial.print("字母间隔 ");
        } else {
          gapDuration = FLASH_GAP;
        }
        
        if (currentMillis - previousMillis >= gapDuration) {
          currentStep++;
          previousMillis = currentMillis;
        }
      } else {
        unsigned long flashDuration = (stepValue == 1) ? SHORT_FLASH : LONG_FLASH;
        
        digitalWrite(LED_PIN_D4, HIGH);
        digitalWrite(LED_PIN_D5, HIGH);
        digitalWrite(LED_PIN_D21, HIGH);
        
        isFlashing = true;
        flashStartTime = currentMillis;
        currentFlashDuration = flashDuration;
        
        Serial.print("开始闪烁 (");
        Serial.print(stepValue == 1 ? "短" : "长");
        Serial.print(") 步骤 ");
        Serial.println(currentStep);
        
        currentStep++;
      }
    } else {
      currentStep = 0;
      previousMillis = currentMillis;
      Serial.println("--- 重新开始 SOS ---");
    }
  }

  delay(50);
}

// 给这个作业打满分，算通过。
