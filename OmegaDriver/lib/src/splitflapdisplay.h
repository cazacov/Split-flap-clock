#pragma once

#include <Arduino.h>
#include "displayDescriptor.h"

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

  DisplayType display_type_;
  uint8_t adc_pin_;

  static void EnableModule(uint8_t adc_pin);
  static void DisableModule(uint8_t adc_pin);
  static void MotorStart(uint8_t adc_pin);
  static void MotorStop(uint8_t adc_pin);
  static uint8_t ReadEncoder();
  static uint8_t read(uint8_t adc_pin);

  int8_t current_pos_;
  uint8_t display_index_;

  void Lock();
  void Unlock();
  int8_t EncoderToPos(uint8_t encoder_value);
};