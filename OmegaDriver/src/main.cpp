#include "flapdisplayboard.h"

const uint8_t PIN_START = 19;
const uint8_t PIN_ADL = 18;
const uint8_t PIN_HOURS_ADC = 5;
const uint8_t PIN_MINUTES_ADC = 17;

const uint8_t PIN_DATA0 = 32;
const uint8_t PIN_DATA1 = 33;
const uint8_t PIN_DATA2 = 25;
const uint8_t PIN_DATA3 = 26;
const uint8_t PIN_DATA4 = 27;
const uint8_t PIN_DATA5 = 14;

FlapDisplay* hours_display;
FlapDisplay* minutes_display;

void setup() {
  Serial.begin(115200);
  FlapDisplayBoard.init(PIN_START, PIN_ADL,PIN_DATA0,PIN_DATA1,PIN_DATA2,PIN_DATA3,PIN_DATA4,PIN_DATA5);
  hours_display = FlapDisplayBoard.CreateDisplay(k40Flaps, PIN_HOURS_ADC);
  minutes_display = FlapDisplayBoard.CreateDisplay(k62Flaps, PIN_MINUTES_ADC);
  delay(5000);
}

int hours = 0;
int minutes = 0;
int i;

void loop() {
  if (!hours_display->isCounting()) {
    hours_display->gotoFlap(hours);
  }
  if (!minutes_display->isCounting()) {
    minutes_display->gotoFlap(minutes);
  }
  delay(300);
  
  if (++i % 10 == 0) {
    hours = (hours + 1) % 24;
    minutes = (minutes + 1) % 60;
  }
}