#include "splitflapdisplay.h"

SplitFlapDisplay::SplitFlapDisplay(DisplayType display_type, uint8_t start_pin, uint8_t adl_pin, uint8_t adc_pin,
  uint8_t data0_pin, uint8_t data1_pin, uint8_t data2_pin, uint8_t data3_pin, uint8_t data4_pin, uint8_t data5_pin) :
    display_type_(display_type),
    start_pin_(start_pin),
    adl_pin_(adl_pin), 
    adc_pin_(adc_pin),
    data0_pin_(data0_pin),
    data1_pin_(data1_pin),
    data2_pin_(data2_pin),
    data3_pin_(data3_pin),
    data4_pin_(data4_pin),
    data5_pin_(data5_pin)
{
  pinMode(start_pin_, OUTPUT);
  pinMode(adl_pin_, OUTPUT);
  pinMode(adc_pin_, OUTPUT);

  pinMode(data0_pin_, INPUT_PULLUP);
  pinMode(data1_pin_, INPUT_PULLUP);
  pinMode(data2_pin_, INPUT_PULLUP);
  pinMode(data3_pin_, INPUT_PULLUP);
  pinMode(data4_pin_, INPUT_PULLUP);
  pinMode(data5_pin_, INPUT_PULLUP);

  digitalWrite(start_pin_, LOW);
  digitalWrite(adl_pin_, LOW);
  digitalWrite(adc_pin_, LOW);

  current_pos_ = -1;

  Serial.print("Initialized");
}

uint8_t SplitFlapDisplay::ReadEncoder() {
  uint8_t input = 0;

  input += (digitalRead(data0_pin_) == LOW) ? 1 << 0 : 0;
  input += (digitalRead(data1_pin_) == LOW) ? 1 << 1 : 0;
  input += (digitalRead(data2_pin_) == LOW) ? 1 << 2 : 0;
  input += (digitalRead(data3_pin_) == LOW) ? 1 << 3 : 0;
  input += (digitalRead(data4_pin_) == LOW) ? 1 << 4 : 0;
  input += (digitalRead(data5_pin_) == LOW) ? 1 << 5 : 0;
  return input;
}

uint8_t SplitFlapDisplay::read() {
  digitalWrite(adl_pin_, HIGH);
  digitalWrite(adc_pin_, HIGH);
  delayMicroseconds(35);
  uint8_t input = ReadEncoder();
  digitalWrite(adl_pin_, LOW);
  digitalWrite(adc_pin_, LOW);
  return input;
}

int8_t SplitFlapDisplay::init() {
  
  uint8_t encoder = read();
  if (encoder > 0) {
    current_pos_ = encoder;
  }
  else {
    current_pos_ = -1;
  }
}

int8_t SplitFlapDisplay::EncoderToIndex(uint8_t encoder_value) {
  
  switch (display_type_) {
    case k40Flaps:
      if (encoder_value == 0) {
        return -1;
      }
      if (encoder_value <= 24) {
        return encoder_value - 1;
      }
      switch (encoder_value) {
        case 0x19:
          return 24;
        case 0x1A:
          return 25;
        case 0x27:
          return 26;
        case 0x2D:
          return 27;
        case 0x31:
          return 28;
        case 0x32:
          return 29;
        case 0x33:
          return 30;
        case 0x34:
          return 31;
        case 0x35:
          return 32;
        case 0x36:
          return 33;
        case 0x37:
          return 34;
        case 0x38:
          return 35;
        case 0x39:
          return 36;
        case 0x30:
          return 37;
        case 0x2E:
          return 38;
        case 0x20:
          return 39;
        default:
          return -2;          
      }
    default:
      return -3;
  }
}

void SplitFlapDisplay::MotorStart() {
//  Serial.println("start motor");
  digitalWrite(adl_pin_, HIGH);
  digitalWrite(adc_pin_, HIGH);
  digitalWrite(start_pin_, HIGH);
  delayMicroseconds(35);
  digitalWrite(adl_pin_, LOW);
  digitalWrite(adc_pin_, LOW);
}

void SplitFlapDisplay::MotorStop() {
//  Serial.println("stop motor");
  digitalWrite(adl_pin_, HIGH);
  digitalWrite(adc_pin_, HIGH);
  digitalWrite(start_pin_, LOW);
  delayMicroseconds(35);
  digitalWrite(adl_pin_, LOW);
  digitalWrite(adc_pin_, LOW);
}

void SplitFlapDisplay::gotoFlap(uint8_t flap_index) {

  Serial.print("GOTO ");
  Serial.println(flap_index);

  if (current_pos_ > 0) {
    Serial.print("Current pos: ");
    Serial.println(current_pos_);
    Serial.print("Target pos: ");
    Serial.println(flap_index);
    int steps = flap_index - current_pos_;
    if (steps < 0) {
      steps += 40;
    }
    Serial.print("Steps: ");
    Serial.println(steps);

    int del = 0;
    if (steps > 0) {
      del = 4000L * steps / 30 - 20;
    }
    Serial.print("Delay: ");
    Serial.println(del);
    MotorStart();
    delay(del);
    MotorStop();
  }
  do {
    uint8_t encoder = read();
    if (encoder == flap_index) {
      current_pos_ = encoder;  
      Serial.println("DONE! ");
      Serial.println(encoder);
      break;
    } 
    Serial.println(encoder);
    MotorStart();
    delay(20);
    MotorStop();
  } while (true);
}