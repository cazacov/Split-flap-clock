#pragma once

#include <Arduino.h>

enum DisplayType
{
  k40Flaps,
  k62Flaps
};

class SplitFlapDisplay
{
public:
  SplitFlapDisplay(DisplayType display_type, uint8_t start_pin, uint8_t adl_pin, uint8_t adc_pin,
    uint8_t data0_pin, uint8_t data1_pin, uint8_t data2_pin, uint8_t data3_pin, uint8_t data4_pin, uint8_t data5_pin);
  int8_t init();
  uint8_t read();
  void gotoFlap(uint8_t flap_index);
private:
  DisplayType display_type_;
  uint8_t start_pin_;
  uint8_t adl_pin_;
  uint8_t adc_pin_;
  uint8_t data0_pin_;
  uint8_t data1_pin_;
  uint8_t data2_pin_;
  uint8_t data3_pin_;
  uint8_t data4_pin_;
  uint8_t data5_pin_;

  int8_t current_pos_;
  
  uint8_t ReadEncoder();
  void MotorStart();
  void MotorStop();
  int8_t EncoderToIndex(uint8_t encoder_value);
};