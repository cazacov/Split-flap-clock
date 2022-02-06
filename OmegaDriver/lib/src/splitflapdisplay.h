#pragma once

#include <Arduino.h>
#include <StensTimer.h>

enum DisplayType
{
  k40Flaps,
  k62Flaps
};

class SplitFlapDisplay : public IStensTimerListener 
{
public:
  SplitFlapDisplay(DisplayType display_type, uint8_t start_pin, uint8_t adl_pin, uint8_t adc_pin,
    uint8_t data0_pin, uint8_t data1_pin, uint8_t data2_pin, uint8_t data3_pin, uint8_t data4_pin, uint8_t data5_pin);
  int8_t init(StensTimer* stens_timer);
  uint8_t read();
  void gotoFlap(uint8_t flap_index);
  void timerCallback(Timer* timer);
  bool isCounting() { return is_counting_;};
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
  int8_t target_flap_;
  StensTimer* stens_timer_;
  volatile bool is_counting_;
//  static volatile bool lock;
  
  void EnableModule();
  void DisableModule();
  uint8_t ReadEncoder();
  void MotorStart();
  void MotorStop();
  void Lock();
  void Unlock();
  int8_t EncoderToPos(uint8_t encoder_value);
};