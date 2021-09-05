#include <Arduino.h>
#define LGFX_AUTODETECT
#define LGFX_USE_V1
#include <M5Core2.h>
#include <LovyanGFX.hpp>

#define WEIGHT_PIN 36
#define KEY_DETECT_THRESH 2048
#define SWITCH_TIME_MS 5000

unsigned long switchingStartTime = 0;

static LGFX lcd;
static LGFX_Sprite sprite(&lcd);

enum status {
  Zaitaku,      // 在宅
  Gaisyutsu,    // 外出
  Itte,         // いってらっしゃい
  Okaeri        // おかえり
};

enum status currStatus = Zaitaku;

void setup() {
  // Comm to PC
  Serial.begin(115200);
  while(!Serial){
    ;
  }

  // LCD setting
  lcd.init();
  lcd.setRotation(1);
	lcd.setBrightness(128);
	lcd.setColorDepth(24);
	lcd.clear();
  M5.begin(true, true, true);

  Serial.println("[KeyStockun] Ready");
}

void loop() {
  if (currStatus == Zaitaku) {
    lcd.setFont(&fonts::lgfxJapanGothic_36);
    lcd.drawString("在宅", 120, 100);
    if (analogRead(WEIGHT_PIN) < KEY_DETECT_THRESH) {
      // 鍵が無いとき
      lcd.clear();
      switchingStartTime = millis();
      currStatus = Itte;
    }
  }
  if (currStatus == Gaisyutsu) {
    lcd.setFont(&fonts::lgfxJapanGothic_36);
    lcd.drawString("外出", 120, 100);
    if (analogRead(WEIGHT_PIN) > KEY_DETECT_THRESH) {
      // 鍵が無いとき
      lcd.clear();
      switchingStartTime = millis();
      currStatus = Okaeri;
    }
  }
  if (currStatus == Itte) {
    lcd.setFont(&fonts::lgfxJapanGothic_36);
    lcd.drawString("いってらっしゃい", 15, 100);
    if (analogRead(WEIGHT_PIN) > KEY_DETECT_THRESH) {
      // 鍵があるとき
      lcd.clear();
      currStatus = Zaitaku;
    }
    if (millis() - switchingStartTime > SWITCH_TIME_MS) {
      lcd.clear();
      currStatus = Gaisyutsu;
    }
  }
  if (currStatus == Okaeri) {
    lcd.setFont(&fonts::lgfxJapanGothic_36);
    lcd.drawString("おかえりなさい", 40, 100);
    if (analogRead(WEIGHT_PIN) < KEY_DETECT_THRESH) {
      // 鍵が無いとき
      lcd.clear();
      currStatus = Gaisyutsu;
    }
    if (millis() - switchingStartTime > SWITCH_TIME_MS) {
      lcd.clear();
      currStatus = Zaitaku;
    }
  }
  Serial.println("[KeyStockun] " + String(currStatus));
  delay(50);
}