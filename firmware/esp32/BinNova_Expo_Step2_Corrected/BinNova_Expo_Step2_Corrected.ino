#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// =====================================================
// TFT
// =====================================================

#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4

// =====================================================
// BIO ULTRASONIC SENSOR
// =====================================================

#define BIO_TRIG  27
#define BIO_ECHO  26

// =====================================================
// NON-BIO ULTRASONIC SENSOR
// =====================================================

#define NONBIO_TRIG  32
#define NONBIO_ECHO  33

// =====================================================
// LEDs + BUZZER
// =====================================================

#define GREEN_LED   14
#define YELLOW_LED  16
#define RED_LED     17
#define BUZZER      25

// =====================================================
// BIN SETTINGS
// =====================================================

#define BIN_HEIGHT 18.5 // change the hight of your bin 
#define NUM_READINGS 5


// =====================================================
// WIFI + WEB SERVER + sms api
// =====================================================

#include "secrets.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const String account_sid = SMS_ACCOUNT_SID;
const String auth_token = SMS_AUTH_TOKEN;
const String from_number = SMS_FROM_NUMBER;
const String to_number = SMS_TO_NUMBER;

WebServer server(80);

// =====================================================
// FIREBASE - STEP 2
// =====================================================

// Use the EXACT Realtime Database URL shown in Firebase Console.
// Do NOT add a trailing slash.
const char* firebaseURL = "https://binnova-expo-default-rtdb.firebaseio.com"; // Replace your firebase url

// Firebase upload interval (milliseconds)
const unsigned long FIREBASE_INTERVAL = 2000;
unsigned long lastFirebaseUpload = 0;

// Shared live values used by TFT and webpage
float currentBioDistance = -1;
float currentNonBioDistance = -1;
float currentBioFill = -1;
float currentNonBioFill = -1;

String statusText(float fill) {
  if (fill < 0) return "SENSOR ERROR";
  if (fill >= 80) return "FULL";
  if (fill >= 50) return "MEDIUM";
  return "NORMAL";
}

void handleRoot() {

  const char page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="theme-color" content="#111111">
<title>BinNova Dashboard</title>

<style>
*{box-sizing:border-box}

body{
  margin:0;
  padding:18px;
  background:#0d0d0d;
  color:#fff;
  font-family:Arial,Helvetica,sans-serif;
}

.wrap{
  width:100%;
  max-width:560px;
  margin:auto;
}

.header{
  display:flex;
  justify-content:space-between;
  align-items:center;
  margin-bottom:4px;
}

h1{
  margin:0;
  color:#00e889;
  font-size:30px;
  letter-spacing:1px;
}

.live{
  color:#00e889;
  font-size:13px;
  font-weight:bold;
}

.subtitle{
  color:#8d8d8d;
  font-size:13px;
  margin-bottom:18px;
}

.card{
  background:#181818;
  border:1px solid #292929;
  border-radius:18px;
  padding:20px;
  margin:14px 0;
}

.row{
  display:flex;
  justify-content:space-between;
  align-items:center;
}

.name{
  font-size:18px;
  font-weight:bold;
}

.percent{
  font-size:48px;
  font-weight:800;
  line-height:1;
  margin:15px 0;
}

.bar{
  width:100%;
  height:14px;
  background:#303030;
  border-radius:20px;
  overflow:hidden;
}

.fill{
  height:100%;
  width:0%;
  background:#00e889;
  border-radius:20px;
  transition:width .4s,background .3s;
}

.status{
  display:inline-block;
  margin-top:14px;
  padding:7px 12px;
  border-radius:20px;
  background:#242424;
  font-size:13px;
  font-weight:bold;
}

.distance{
  color:#888;
  font-size:13px;
  margin-top:10px;
}

.footer{
  text-align:center;
  color:#666;
  font-size:12px;
  margin-top:20px;
}

.error{
  color:#ff5555 !important;
}

.warning{
  color:#ffd34d !important;
}
</style>
</head>

<body>

<div class="wrap">

  <div class="header">
    <h1>BINNOVA</h1>
    <div class="live">â— LIVE</div>
  </div>

  <div class="subtitle">SMART WASTE BIN â€¢ ESP32 IoT</div>

  <div class="card">

    <div class="row">
      <div class="name">BIO BIN</div>
      <div id="bioStatus" class="status">WAITING</div>
    </div>

    <div class="percent">
      <span id="bioFill">--</span>%
    </div>

    <div class="bar">
      <div id="bioBar" class="fill"></div>
    </div>

    <div class="distance">
      Distance: <span id="bioDistance">--</span> cm
    </div>

  </div>

  <div class="card">

    <div class="row">
      <div class="name">NON-BIO BIN</div>
      <div id="nonBioStatus" class="status">WAITING</div>
    </div>

    <div class="percent">
      <span id="nonBioFill">--</span>%
    </div>

    <div class="bar">
      <div id="nonBioBar" class="fill"></div>
    </div>

    <div class="distance">
      Distance: <span id="nonBioDistance">--</span> cm
    </div>

  </div>

  <div class="footer">
    Live data â€¢ Updates automatically
  </div>

</div>

<script>

function setBin(prefix, fill, status, distance){

  const fillText =
    document.getElementById(prefix + "Fill");

  const bar =
    document.getElementById(prefix + "Bar");

  const statusText =
    document.getElementById(prefix + "Status");

  const distanceText =
    document.getElementById(prefix + "Distance");

  if(fill < 0){

    fillText.textContent = "--";
    bar.style.width = "0%";
    bar.style.background = "#ff5555";

    statusText.textContent = "SENSOR ERROR";
    statusText.className = "status error";

    distanceText.textContent = "--";
    return;
  }

  fillText.textContent = Math.round(fill);
  bar.style.width = fill + "%";

  if(fill >= 80){

    bar.style.background = "#ff5555";
    statusText.className = "status error";

  }else if(fill >= 50){

    bar.style.background = "#ffd34d";
    statusText.className = "status warning";

  }else{

    bar.style.background = "#00e889";
    statusText.className = "status";
  }

  statusText.textContent = status;

  distanceText.textContent =
    distance.toFixed(1);
}

function updateDashboard(){

  fetch("/data")
    .then(response => response.json())
    .then(data => {

      setBin(
        "bio",
        data.bioFill,
        data.bioStatus,
        data.bioDistance
      );

      setBin(
        "nonBio",
        data.nonBioFill,
        data.nonBioStatus,
        data.nonBioDistance
      );

    })
    .catch(error => {

      console.log("Dashboard update failed:", error);

    });
}

updateDashboard();

setInterval(updateDashboard, 1000);

</script>

</body>
</html>
)rawliteral";

  server.send_P(200, "text/html", page);
}

void handleData() {
  String json = "{";
  json += "\"bioFill\":" + String(currentBioFill, 0) + ",";
  json += "\"nonBioFill\":" + String(currentNonBioFill, 0) + ",";
  json += "\"bioDistance\":" + String(currentBioDistance, 1) + ",";
  json += "\"nonBioDistance\":" + String(currentNonBioDistance, 1) + ",";
  json += "\"bioStatus\":\"" + statusText(currentBioFill) + "\",";
  json += "\"nonBioStatus\":\"" + statusText(currentNonBioFill) + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

Adafruit_ST7735 tft = Adafruit_ST7735(
  TFT_CS,
  TFT_DC,
  TFT_RST
);

// =====================================================
// READ ONE ULTRASONIC SENSOR
// =====================================================

float readDistance(int trigPin, int echoPin) {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(
    echoPin,
    HIGH,
    30000
  );

  if (duration == 0) {
    return -1;
  }

  return duration * 0.0343 / 2.0;
}

// =====================================================
// MEDIAN FILTER
// Takes 5 readings and ignores values outside the bin
// =====================================================

float getMedianDistance(int trigPin, int echoPin) {

  float readings[NUM_READINGS];
  int validReadings = 0;

  for (int i = 0; i < NUM_READINGS; i++) {

    float distance = readDistance(trigPin, echoPin);

    if (distance > 0 && distance <= BIN_HEIGHT) {
      readings[validReadings] = distance;
      validReadings++;
    }

    delay(50);
  }

  // Need at least 3 valid readings
  if (validReadings < 3) {
    return -1;
  }

  // Sort readings
  for (int i = 0; i < validReadings - 1; i++) {

    for (int j = i + 1; j < validReadings; j++) {

      if (readings[j] < readings[i]) {

        float temp = readings[i];
        readings[i] = readings[j];
        readings[j] = temp;
      }
    }
  }

  // Middle value = median
  return readings[validReadings / 2];
}

// =====================================================
// CALCULATE FILL PERCENTAGE
// =====================================================

float calculateFill(float distance) {

  if (distance < 0) {
    return -1;
  }

  float wasteHeight =
    BIN_HEIGHT - distance;

  float fillPercent =
    (wasteHeight / BIN_HEIGHT) * 100.0;

  return constrain(fillPercent, 0, 100);
}

// =====================================================
// FIREBASE UPLOAD
// =====================================================

void sendToFirebase() {

  // Never stop the working bin if Wi-Fi drops.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Firebase: Wi-Fi not connected - skipped");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();  // Expo prototype: no certificate management yet

  HTTPClient http;

  String url = String(firebaseURL) + "/bins/BN-017.json";

  // Firebase Realtime Database REST payload
  String json = "{";
  json += "\"binId\":\"BN-017\",";
  json += "\"bioFill\":" + String(currentBioFill, 0) + ",";
  json += "\"nonBioFill\":" + String(currentNonBioFill, 0) + ",";
  json += "\"bioDistance\":" + String(currentBioDistance, 1) + ",";
  json += "\"nonBioDistance\":" + String(currentNonBioDistance, 1) + ",";
  json += "\"bioStatus\":\"" + statusText(currentBioFill) + "\",";
  json += "\"nonBioStatus\":\"" + statusText(currentNonBioFill) + "\",";
  json += "\"uptimeMs\":" + String(millis());
  json += "}";

  Serial.println();
  Serial.println("Firebase: uploading BN-017...");

  http.setConnectTimeout(1500);
  http.setTimeout(1500);

  if (!http.begin(client, url)) {
    Serial.println("Firebase: connection setup failed");
    return;
  }

  http.addHeader("Content-Type", "application/json");

  int httpCode = http.PUT(json);

  Serial.print("Firebase HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    if (httpCode >= 200 && httpCode < 300) {
      Serial.println("Firebase: upload OK");
    } else {
      Serial.print("Firebase response: ");
      Serial.println(http.getString());
    }
  } else {
    Serial.print("Firebase request error: ");
    Serial.println(http.errorToString(httpCode));
  }

  http.end();
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // Wi-Fi / web server
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.begin();

  Serial.print("Dashboard: http://");
  Serial.println(WiFi.localIP());

  // Ultrasonic sensors
  pinMode(BIO_TRIG, OUTPUT);
  pinMode(BIO_ECHO, INPUT);

  pinMode(NONBIO_TRIG, OUTPUT);
  pinMode(NONBIO_ECHO, INPUT);

  digitalWrite(BIO_TRIG, LOW);
  digitalWrite(NONBIO_TRIG, LOW);

  // LEDs + buzzer
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

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

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(38, 35);
  tft.println("DUAL BIN");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  server.handleClient();

  // ---------------------------------------------------
  // Read BIO bin
  // ---------------------------------------------------

  float bioDistance =
    getMedianDistance(
      BIO_TRIG,
      BIO_ECHO
    );

  // Small gap to reduce ultrasonic cross-talk
  delay(100);

  // ---------------------------------------------------
  // Read NON-BIO bin
  // ---------------------------------------------------

  float nonBioDistance =
    getMedianDistance(
      NONBIO_TRIG,
      NONBIO_ECHO
    );

  // ---------------------------------------------------
  // Calculate fill levels
  // ---------------------------------------------------

  float bioFill =
    calculateFill(bioDistance);

  float nonBioFill =
    calculateFill(nonBioDistance);

  // Share the exact same values with the web dashboard.
  currentBioDistance = bioDistance;
  currentNonBioDistance = nonBioDistance;
  currentBioFill = bioFill;
  currentNonBioFill = nonBioFill;

  // ---------------------------------------------------
  // Firebase: send live data every 2 seconds
  // ---------------------------------------------------
  if (millis() - lastFirebaseUpload >= FIREBASE_INTERVAL) {
    lastFirebaseUpload = millis();
    sendToFirebase();
  }

  // Keep web requests responsive.
  server.handleClient();

  // ---------------------------------------------------
  // Serial Monitor
  // ---------------------------------------------------

  Serial.print("BIO: ");

  if (bioFill < 0) {
    Serial.print("ERROR");
  }
  else {
    Serial.print(bioDistance, 1);
    Serial.print(" cm / ");
    Serial.print(bioFill, 0);
    Serial.print("%");
  }

  Serial.print("    |    NON-BIO: ");

  if (nonBioFill < 0) {
    Serial.println("ERROR");
  }
  else {
    Serial.print(nonBioDistance, 1);
    Serial.print(" cm / ");
    Serial.print(nonBioFill, 0);
    Serial.println("%");
  }

  // ---------------------------------------------------
  // Overall status
  // ---------------------------------------------------

  bool sensorError =
    (bioFill < 0 || nonBioFill < 0);

  bool anyBinFull =
    (bioFill >= 80 || nonBioFill >= 80);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  if (sensorError) {

    // No alert when a sensor is invalid
    // because the actual bin level is unknown.

  }
  else if (anyBinFull) {

    digitalWrite(RED_LED, HIGH);

    // Short alert
    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);

  }
  else if (bioFill >= 50 || nonBioFill >= 50) {

    digitalWrite(YELLOW_LED, HIGH);

  }
  else {

    digitalWrite(GREEN_LED, HIGH);
  }

  // ---------------------------------------------------
  // TFT
  // ---------------------------------------------------

  tft.fillRect(
    0, 50,
    160, 105,
    ST77XX_BLACK
  );

  // BIO
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(10, 55);
  tft.println("BIO");

  tft.setTextSize(2);
  tft.setCursor(65, 50);

  if (bioFill < 0) {
    tft.setTextColor(ST77XX_RED);
    tft.print("ERR");
  }
  else {
    tft.setTextColor(ST77XX_CYAN);
    tft.print(bioFill, 0);
    tft.print("%");
  }

  // NON-BIO
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(10, 85);
  tft.println("NON-BIO");

  tft.setTextSize(2);
  tft.setCursor(65, 80);

  if (nonBioFill < 0) {
    tft.setTextColor(ST77XX_RED);
    tft.print("ERR");
  }
  else {
    tft.setTextColor(ST77XX_YELLOW);
    tft.print(nonBioFill, 0);
    tft.print("%");
  }

  // Overall status
  tft.setTextSize(1);
  tft.setCursor(10, 120);

  if (sensorError) {

    tft.setTextColor(ST77XX_RED);
    tft.println("STATUS: SENSOR ERROR");

  }
  else if (anyBinFull) {

    tft.setTextColor(ST77XX_RED);
    tft.println("STATUS: BIN FULL");

  }
  else if (bioFill >= 50 || nonBioFill >= 50) {

    tft.setTextColor(ST77XX_YELLOW);
    tft.println("STATUS: MEDIUM");

  }
  else {

    tft.setTextColor(ST77XX_GREEN);
    tft.println("STATUS: SYSTEM OK");
  }

  delay(300);
}
