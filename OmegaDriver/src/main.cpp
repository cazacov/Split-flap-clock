#include <Arduino.h>
#include "splitflapdisplay.h"

const uint8_t PIN_HOURS_START = 5;
const uint8_t PIN_HOURS_ADC = 18;
const uint8_t PIN_MINUTES_START = 16;
const uint8_t PIN_MINUTES_ADC = 17;
const uint8_t PIN_ADL = 19;

const uint8_t PIN_DATA0 = 32;
const uint8_t PIN_DATA1 = 33;
const uint8_t PIN_DATA2 = 25;
const uint8_t PIN_DATA3 = 26;
const uint8_t PIN_DATA4 = 27;
const uint8_t PIN_DATA5 = 14;

SplitFlapDisplay hours(k40Flaps, PIN_HOURS_START, PIN_ADL, PIN_HOURS_ADC,
  PIN_DATA0,PIN_DATA1,PIN_DATA2,PIN_DATA3,PIN_DATA4,PIN_DATA5);

void setup() {
  Serial.begin(115200);
  hours.init();
  delay(5000);
}

int i = 1;

void loop() {
  hours.gotoFlap(i);
  i++;
  if (i == 25) {
    i = 1;    
  }
  delay(3000);
}