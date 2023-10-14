#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30105.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 // Reset pin (none)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);//screen size

MAX30105 particleSensor;
const byte RATE_SIZE = 4; // Number of samples to average
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_I2C_ADDRESS, OLED_RESET);
  display.display();
  delay(2000);
  display.clearDisplay();
  
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check your connections.");
    while (1);
  }
  particleSensor.setup(); // Configure sensor with default settings
  
  // Initialize the buzzer
  pinMode(9, OUTPUT);
}

void loop() {
  long irValue = particleSensor.getIR();
  
  if (checkForBeat(irValue)) {
    // We sensed a beat!
    Serial.println("Heartbeat detected!");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Heartbeat detected!");
    display.display();
    
    // Turn on the buzzer for a short duration
    digitalWrite(9, HIGH);
    delay(100); // Adjust the delay as needed
    digitalWrite(9, LOW);
  }
  
  // Take some action based on your application requirements
  // For example, you can calculate and display heart rate and SpO2 here.
  // See the MAX30105 library for more details on how to do this.
}

boolean checkForBeat(long value) {
  // Keep a running total of the last RATE_SIZE IR values
  long sum = 0;
  for (byte i = 0; i < RATE_SIZE; i++) {
    rates[i] = value;
    sum += rates[i];
  }
  
  // Calculate the average IR value
  long average = sum / RATE_SIZE;
  
  // Calculate the beat value (difference between the current IR value and the average)
  long beat = value - average;
  
  // Update the lastBeat timestamp
  if (beat > 700 && millis() - lastBeat > 1000) {
    lastBeat = millis();
    return true;
  }
  
  return false;
}
