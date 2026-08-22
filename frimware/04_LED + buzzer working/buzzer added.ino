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

// ---------- LEDs + Buzzer ----------
#define GREEN_LED   14
#define YELLOW_LED  16
#define RED_LED     17
#define BUZZER      25

// ---------- Bin ----------
#define BIN_HEIGHT 35.0

Adafruit_ST7735 tft = Adafruit_ST7735(
  TFT_CS,
  TFT_DC,
  TFT_RST
);

void setup() {

  Serial.begin(115200);

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // LEDs + buzzer
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Start everything OFF
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  // TFT
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

  // ---------- Ultrasonic trigger ----------
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // ---------- Read echo ----------
  long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    30000
  );

  // ---------- Sensor error ----------
  if (duration == 0) {

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    tft.fillRect(
      0, 50,
      160, 85,
      ST77XX_BLACK
    );

    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(2);
    tft.setCursor(35, 75);
    tft.println("ERROR");

    Serial.println("Ultrasonic Error");

    delay(500);
    return;
  }

  // ---------- Distance ----------
  float distance =
    duration * 0.0343 / 2.0;

  // ---------- Waste height ----------
  float wasteHeight =
    BIN_HEIGHT - distance;

  // ---------- Fill percentage ----------
  float fillPercent =
    (wasteHeight / BIN_HEIGHT) * 100.0;

  fillPercent =
    constrain(fillPercent, 0, 100);

  // ---------- Progress bar ----------
  int barWidth =
    map(
      (int)fillPercent,
      0,
      100,
      0,
      140
    );

  // ---------- Status ----------
  String status;

  // Turn all LEDs OFF first
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  if (fillPercent < 50) {

    digitalWrite(GREEN_LED, HIGH);
    status = "STATUS: NORMAL";

  }
  else if (fillPercent < 80) {

    digitalWrite(YELLOW_LED, HIGH);
    status = "STATUS: MEDIUM";

  }
  else {

    digitalWrite(RED_LED, HIGH);
    status = "STATUS: FULL";

    // Short alert beep
    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
  }

  // ---------- Serial Monitor ----------
  Serial.print("Distance: ");
  Serial.print(distance, 1);

  Serial.print(" cm | Fill: ");
  Serial.print(fillPercent, 0);

  Serial.print("% | ");
  Serial.println(status);

  // ---------- Clear TFT ----------
  tft.fillRect(
    0, 50,
    160, 85,
    ST77XX_BLACK
  );

  // ---------- Percentage ----------
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(45, 55);

  tft.print(fillPercent, 0);
  tft.println("%");

  // ---------- Progress bar ----------
  tft.drawRect(
    10, 85,
    140, 15,
    ST77XX_WHITE
  );

  if (barWidth > 2) {

    tft.fillRect(
      11,
      86,
      barWidth - 2,
      13,
      ST77XX_GREEN
    );
  }

  // ---------- Status ----------
  tft.setTextSize(1);
  tft.setCursor(45, 115);

  if (fillPercent < 50) {

    tft.setTextColor(ST77XX_GREEN);

  }
  else if (fillPercent < 80) {

    tft.setTextColor(ST77XX_YELLOW);

  }
  else {

    tft.setTextColor(ST77XX_RED);
  }

  tft.println(status);

  delay(500);
}