/*
 * Espresso Machine Mod Controller - SIMPLIFIED TEST VERSION
 * Arduino Nano with TFT Display and Rotary Encoder
 * 
 * This simplified version tests ONLY the display and encoder
 * Other sensors disabled for initial testing
 */

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// ===== PIN CONFIGURATION =====
// Display pins
#define TFT_CS_PIN              A5
#define TFT_DC_PIN              A6
#define TFT_RST_PIN             A7
#define TFT_MOSI_PIN            11
#define TFT_CLK_PIN             13

// Rotary Encoder
#define ENCODER_CLK_PIN         6
#define ENCODER_DT_PIN          7
#define ENCODER_SW_PIN          8

// ===== DISPLAY =====
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS_PIN, TFT_DC_PIN, TFT_MOSI_PIN, TFT_CLK_PIN, TFT_RST_PIN);

// ===== ENCODER VARIABLES =====
volatile int encoderValue = 0;
volatile boolean encoderButtonPressed = false;
int lastEncoderValue = 0;

void encoderISR() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  
  // Debounce
  if (currentTime - lastTime < 5) {
    return;
  }
  lastTime = currentTime;
  
  if (digitalRead(ENCODER_CLK_PIN) == digitalRead(ENCODER_DT_PIN)) {
    encoderValue++;
  } else {
    encoderValue--;
  }
}

void buttonISR() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  
  // Debounce
  if (currentTime - lastTime < 50) {
    return;
  }
  lastTime = currentTime;
  encoderButtonPressed = true;
}

void initEncoder() {
  pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
  pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW_PIN), buttonISR, FALLING);
}

void initDisplay() {
  tft.initR(INITR_BLACKTAB);
  delay(100);
  
  // Try all 4 rotations to find correct one
  for(int rot = 0; rot < 4; rot++) {
    tft.setRotation(rot);
    tft.fillScreen(ST7735_BLACK);
    delay(100);
    
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(2);
    tft.setCursor(20, 20);
    tft.print("R");
    tft.println(rot);
    delay(500);
  }
  
  // Use rotation 1 by default
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(40, 50);
  tft.println("ESPRESSO");
  
  tft.setTextSize(1);
  tft.setCursor(20, 100);
  tft.println("Display: OK");
}

void updateDisplay() {
  // Draw encoder value
  tft.fillRect(20, 120, 200, 20, ST7735_BLACK);  // Clear previous text
  
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(20, 120);
  tft.print("Encoder: ");
  tft.println(encoderValue);
  
  // Draw button status
  tft.fillRect(20, 140, 200, 20, ST7735_BLACK);
  tft.setCursor(20, 140);
  tft.print("Button: ");
  if (encoderButtonPressed) {
    tft.println("PRESSED!");
    encoderButtonPressed = false;
  } else {
    tft.println("Ready");
  }
}

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println("Starting...");
  
  // Initialize components
  initDisplay();
  initEncoder();
  
  Serial.println("Display and Encoder initialized!");
}

void loop() {
  updateDisplay();
  delay(100);
}
