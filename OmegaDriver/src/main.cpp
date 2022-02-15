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

FlapDisplay* hours;
FlapDisplay* minutes;

void setup() {
  Serial.begin(115200);
  FlapDisplayBoard.init(PIN_START, PIN_ADL,PIN_DATA0,PIN_DATA1,PIN_DATA2,PIN_DATA3,PIN_DATA4,PIN_DATA5);
  hours = FlapDisplayBoard.CreateDisplay(k40Flaps, PIN_HOURS_ADC);
//  minutes = SplitFlapDisplay::CreateDisplay(k40Flaps, PIN_HOURS_ADC);
  delay(5000);
}

int i = 0;

void loop() {
  if (hours->isCounting()) {
    Serial.println("Waiting for hours counter");
    while (hours->isCounting()) {
      delay(200);
    }
  }
  delay(3000);
  hours->gotoFlap(i);
  i++;
  if (i == 25) {
    i = 1;    
  }
}