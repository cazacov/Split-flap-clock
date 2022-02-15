#pragma once

#include <Arduino.h>
#include "displayDescriptor.h"

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
#define TIMER_INTERVAL_MS        20
#include "ESP32TimerInterrupt.h"

enum DisplayType
{
  k40Flaps,
  k62Flaps
};

class SplitFlapDisplay
{
public:
  static void init(uint8_t start_pin, uint8_t adl_pin, uint8_t data0_pin, uint8_t data1_pin, uint8_t data2_pin, uint8_t data3_pin, uint8_t data4_pin, uint8_t data5_pin);
  SplitFlapDisplay(DisplayType display_type, uint8_t adc_pin);
  void gotoFlap(uint8_t flap_index);
  bool isCounting() { return displays_[display_index_].is_counting;};
private:
  static DisplayDescriptor displays_[16];
  static uint8_t display_count_;
  static uint8_t start_pin_;
  static uint8_t adl_pin_;
  static uint8_t data0_pin_;
  static uint8_t data1_pin_;
  static uint8_t data2_pin_;
  static uint8_t data3_pin_;
  static uint8_t data4_pin_;
  static uint8_t data5_pin_;
  static ESP32Timer itimer_;

  DisplayType display_type_;
  uint8_t adc_pin_;

  static void EnableModule(uint8_t adc_pin);
  static void DisableModule(uint8_t adc_pin);
  static void MotorStart(uint8_t adc_pin);
  static void MotorStop(uint8_t adc_pin);
  static uint8_t ReadEncoder();
  static uint8_t read(uint8_t adc_pin);

#if USING_ESP32_S2_TIMER_INTERRUPT
  static void IRAM_ATTR TimerHandler(void * timerNo);
#else
  static void IRAM_ATTR TimerHandler(void);
#endif

  uint8_t display_index_;

  void Lock();
  void Unlock();
  int8_t EncoderToPos(uint8_t encoder_value);
  uint8_t PosToEncoder(uint8_t flap_index);
};