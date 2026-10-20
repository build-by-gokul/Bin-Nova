#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.setCursor(15, 20);
  tft.println("BINNOVA");

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(20, 55);
  tft.println("SMART WASTE BIN");

  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(25, 85);
  tft.println("SYSTEM ONLINE");

  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(35, 110);
  tft.println("TEST MODE");
}

void loop() {
}