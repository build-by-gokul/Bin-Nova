#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// ---------- TFT ----------
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4

// ---------- Ultrasonic ----------
#define TRIG_PIN  27
#define ECHO_PIN  26

// ---------- Bin ----------
#define BIN_HEIGHT 35.0

Adafruit_ST7735 tft = Adafruit_ST7735(
  TFT_CS,
  TFT_DC,
  TFT_RST
);

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.setCursor(15, 10);
  tft.println("BINNOVA");

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(45, 35);
  tft.println("FILL LEVEL");
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    tft.fillRect(0, 50, 160, 85, ST77XX_BLACK);

    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(2);
    tft.setCursor(35, 75);
    tft.println("ERROR");

    Serial.println("Ultrasonic Error");

    delay(500);
    return;
  }

  float distance = duration * 0.0343 / 2.0;

  float wasteHeight = BIN_HEIGHT - distance;

  float fillPercent = (wasteHeight / BIN_HEIGHT) * 100.0;

  fillPercent = constrain(fillPercent, 0, 100);

  int barWidth = map((int)fillPercent, 0, 100, 0, 140);

  Serial.print("Distance: ");
  Serial.print(distance, 1);
  Serial.print(" cm | Fill: ");
  Serial.print(fillPercent, 0);
  Serial.println("%");

  tft.fillRect(0, 50, 160, 85, ST77XX_BLACK);

  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(45, 55);

  tft.print(fillPercent, 0);
  tft.println("%");

  tft.drawRect(10, 85, 140, 15, ST77XX_WHITE);

  if (barWidth > 2) {
    tft.fillRect(11, 86, barWidth - 2, 13, ST77XX_GREEN);
  }

  tft.setTextSize(1);
  tft.setCursor(45, 115);

  if (fillPercent < 50) {
    tft.setTextColor(ST77XX_GREEN);
    tft.println("STATUS: NORMAL");
  }
  else if (fillPercent < 80) {
    tft.setTextColor(ST77XX_YELLOW);
    tft.println("STATUS: MEDIUM");
  }
  else {
    tft.setTextColor(ST77XX_RED);
    tft.println("STATUS: FULL");
  }

  delay(500);
}
