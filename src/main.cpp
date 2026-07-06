#include <Arduino.h>
#include <USB.h>
#include <USBHIDKeyboard.h>
#include <USBHIDConsumerControl.h>
#include "encoder_direction.h"

USBHIDKeyboard keyboard;
USBHIDConsumerControl consumer;

// ---- wire your buttons/encoder to these GPIOs, then edit to match ----
const uint8_t buttonPins[] = {4, 5, 6, 7, 15};
const char *buttonWords[] = {"Str0ng", "dev3l", "pa$$", "apple"}; // text typed once per button press (buttons 0-3)
// button 4 (GPIO15) sends the Enter key instead, see pollButtons()

const uint8_t encA = 17, encB = 16, encSw = 18; // S2, S1, Key (swapped to reverse volume direction)
// ------------------------------------------------------------------

const size_t numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
bool buttonState[numButtons];
uint32_t buttonDebounceAt[numButtons];
const uint32_t debounceMs = 20;

uint8_t encPrevAB = 0;
bool encSwState = HIGH;
uint32_t encSwDebounceAt = 0;

void setupButtons() {
  for (size_t i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    buttonState[i] = HIGH;
  }
}

void pollButtons() {
  uint32_t now = millis();
  for (size_t i = 0; i < numButtons; i++) {
    bool reading = digitalRead(buttonPins[i]);
    if (reading != buttonState[i] && now - buttonDebounceAt[i] > debounceMs) {
      buttonDebounceAt[i] = now;
      buttonState[i] = reading;
      Serial.printf("button %u -> %s\n", i, reading == LOW ? "PRESSED" : "released");
      if (reading == LOW) {
        if (i == 4) keyboard.write(KEY_RETURN);
        else keyboard.print(buttonWords[i]);
      }
    }
  }
}

void setupEncoder() {
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);
  pinMode(encSw, INPUT_PULLUP);
}

// ponytail: gray-code polling in loop(), good enough for a slow volume knob;
// switch to attachInterrupt on encA/encB if fast turns start dropping steps
void pollEncoder() {
  uint8_t ab = (digitalRead(encA) << 1) | digitalRead(encB);
  if (ab != encPrevAB) {
    int8_t dir = encoderDirection(encPrevAB, ab);
    if (dir > 0) {
      consumer.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
      consumer.release();
    } else if (dir < 0) {
      consumer.press(CONSUMER_CONTROL_VOLUME_DECREMENT);
      consumer.release();
    }
    encPrevAB = ab;
  }

  uint32_t now = millis();
  bool sw = digitalRead(encSw);
  if (sw != encSwState && now - encSwDebounceAt > debounceMs) {
    encSwDebounceAt = now;
    encSwState = sw;
    if (sw == LOW) {
      consumer.press(CONSUMER_CONTROL_MUTE);
      consumer.release();
    }
  }
}

void setup() {
  Serial.begin(115200);
  setupButtons();
  setupEncoder();
  keyboard.begin();
  consumer.begin();
  USB.begin();
}

void loop() {
  static uint32_t lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 1000) {
    lastHeartbeat = millis();
    Serial.printf("alive, GPIO4 raw=%d\n", digitalRead(4));
  }
  pollButtons();
  pollEncoder();
  delay(1);
}
