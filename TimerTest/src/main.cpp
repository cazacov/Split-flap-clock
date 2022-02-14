/*
Copyright (C) 2017  Arjen Stens

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>

#ifndef ESP32
  #error This code is designed to run on ESP32 platform, not Arduino nor ESP8266! Please check your Tools->Board setting.
#elif ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_ESP32S2_THING_PLUS || ARDUINO_MICROS2 || \
        ARDUINO_METRO_ESP32S2 || ARDUINO_MAGTAG29_ESP32S2 || ARDUINO_FUNHOUSE_ESP32S2 || \
        ARDUINO_ADAFRUIT_FEATHER_ESP32S2_NOPSRAM )
  #define USING_ESP32_S2_TIMER_INTERRUPT            true
#endif
// These define's must be placed at the beginning before #include "TimerInterrupt_Generic.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     4

#include "ESP32TimerInterrupt.h"

#include <MyClass.h>
#include <StensTimer.h>
StensTimer* stensTimer;

/* Define some custom Action code */
#define SHOW_NAME_ACTION 1

#if USING_ESP32_S2_TIMER_INTERRUPT
  void IRAM_ATTR TimerHandler0(void * timerNo)
#else
  void IRAM_ATTR TimerHandler0(void)
#endif
{
#if USING_ESP32_S2_TIMER_INTERRUPT
  /////////////////////////////////////////////////////////
  // Always call this for ESP32-S2 before processing ISR
  TIMER_ISR_START(timerNo);
  /////////////////////////////////////////////////////////
#endif

#if (TIMER_INTERRUPT_DEBUG > 0)
  Serial.print("ITimer0: millis() = "); Serial.println(millis());
#endif
  if (stensTimer) {
    stensTimer->run();
  }

#if USING_ESP32_S2_TIMER_INTERRUPT
  /////////////////////////////////////////////////////////
  // Always call this for ESP32-S2 after processing ISR
  TIMER_ISR_END(timerNo);
  /////////////////////////////////////////////////////////
#endif
}

#define TIMER0_INTERVAL_MS        20

// Init ESP32 timer 0
ESP32Timer ITimer0(0);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(100);

  Serial.println(ARDUINO_BOARD);

#if USING_ESP32_S2_TIMER_INTERRUPT
  Serial.println("S2");
  Serial.println(ESP32_S2_TIMER_INTERRUPT_VERSION);
#else
  Serial.println("not S2");
  Serial.println(ESP32_TIMER_INTERRUPT_VERSION);
#endif

  /* Save instance of StensTimer to the stensTimer variable */
  stensTimer = StensTimer::getInstance();

  Serial.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));

  // Using ESP32  => 80 / 160 / 240MHz CPU clock ,
  // For 64-bit timer counter
  // For 16-bit timer prescaler up to 1024

  // Interval in microsecs
  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, TimerHandler0))
  {
    Serial.print(F("Starting  ITimer0 OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));  


  /* Example of two different instances of a class and set their names */
  MyClass* myClassOne = new MyClass(stensTimer);
  MyClass* myClassTwo = new MyClass(stensTimer);
  myClassOne->setName("One");
  myClassTwo->setName("Two");

  /* Set timers for myClassOne and myClassTwo to show their name */
  stensTimer->setTimer(myClassOne, SHOW_NAME_ACTION, 1000);
  stensTimer->setTimer(myClassTwo, SHOW_NAME_ACTION, 2000);

  Serial.println("Setup completed...");

}

void loop() {
  /* Let StensTimer do it's magic every time loop() is executed */
  
}
