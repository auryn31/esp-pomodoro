#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Ticker.h>
#include <Wire.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define OLED_RESET -1
uint32_t workTime = 60 * 25 + 1;
uint32_t breakTime = 60 * 5 + 1;
Ticker tkSec;
uint32_t remainingTime = workTime;
bool work = true;
bool running = false;
bool startStopLastState = false;
const unsigned char playPauseBitmap[] PROGMEM = {
    0x00, 0x00, 0x41, 0x80, 0xf0, 0x00, 0x41, 0x80, 0xfc, 0x00, 0x41, 0x80,
    0xce, 0x00, 0x41, 0x80, 0xc3, 0x80, 0x41, 0x80, 0xc0, 0xe0, 0x41, 0x80,
    0xc0, 0x70, 0x41, 0x80, 0xc0, 0x1c, 0x41, 0x80, 0xc0, 0x0c, 0x41, 0x80,
    0xc0, 0x38, 0x41, 0x80, 0xc0, 0x70, 0x41, 0x80, 0xc1, 0xc0, 0x41, 0x80,
    0xc7, 0x80, 0x41, 0x80, 0xce, 0x00, 0x41, 0x80, 0xf8, 0x00, 0x41, 0x80,
    0xe0, 0x00, 0x41, 0x80, 0x00, 0x00, 0x41, 0x80};
const unsigned char resetBitmap[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x3e, 0x30, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x03, 0xe3, 0xf0, 0x00,
    0x07, 0x00, 0xf8, 0x00, 0x06, 0x00, 0xf8, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x0c, 0x00, 0x1c, 0x00, 0x0c, 0x00,
    0x0c, 0x00, 0x18, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x06, 0x00, 0x30, 0x00,
    0x07, 0x00, 0x70, 0x00, 0x03, 0xc1, 0xe0, 0x00, 0x00, 0xff, 0x80, 0x00,
    0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00};

void everySecond() {
  Serial.println("Tick");
  if (running) {
    Serial.println("Running");
    if (remainingTime && --remainingTime == 0) {
      if (work) {
        work = false;
        remainingTime = breakTime;
      } else {
        running = false;
      }
    } else {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 10);
      // Display left time in mm:ss
      if (work) {
        display.println("HUSSLE");
      } else {
        display.println("CHILL");
      }
      display.setCursor(0, 40);
      display.setTextSize(3);
      if (remainingTime < 600) {
        display.print("0");
        display.print(remainingTime / 60);
      } else {
        display.print(remainingTime / 60);
      }
      display.print(":");
      if (remainingTime % 60 < 10) {
        display.print("0");
      }
      display.println(remainingTime % 60);
      display.drawBitmap(100, 10, playPauseBitmap, 25, 17, WHITE);
      display.drawBitmap(100, 40, resetBitmap, 25, 25, WHITE);
      display.display();
    }
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Pomodoro Timer");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  tkSec.attach(1, everySecond);
  display.clearDisplay();

  Serial.println("Reset");
  running = true;
  work = true;
  remainingTime = workTime;
  everySecond();
  running = false;
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
}

void loop() {
  int startStop = digitalRead(1);
  int reset = digitalRead(2);

  // reset
  if (reset == LOW) {
    running = true;
    work = true;
    remainingTime = workTime;
    everySecond();
    delay(10);
  }

  // start/stop
  if (startStop == LOW && startStopLastState == HIGH) {
    Serial.println("Start/Stop");
    running = !running;
  }
  startStopLastState = startStop;
}