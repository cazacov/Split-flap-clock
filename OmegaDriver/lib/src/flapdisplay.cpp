#include "flapdisplay.h"
#include <Arduino.h>

FlapDisplay::FlapDisplay(DisplayType display_type, uint8_t adc_pin, DisplayDescriptor* descriptor) :
    display_type_(display_type),
    adc_pin_(adc_pin),
    descriptor_(descriptor)
{
  pinMode(adc_pin_, OUTPUT);

  descriptor->adc_pin = adc_pin;
  descriptor->is_counting = false;
  descriptor->target_millis = 0;
  descriptor->target_encoder = 0;
  descriptor->must_be_started = false;
  descriptor->current_encoder = 0;
}

void FlapDisplay::gotoFlap(uint8_t flap_index) {

  if (isCounting()) {
    Serial.println("Is already counting. Skip gotoFlap");
    return;
  }

  uint8_t current_encoder = descriptor_->current_encoder;
  int8_t current_pos = EncoderToPos(current_encoder); 
  uint8_t target_encoder = PosToEncoder(flap_index);

  Serial.println("");
  Serial.print("GOTO position ");
  Serial.println(flap_index);
  Serial.print("Current pos: ");
  Serial.println(current_pos);
  Serial.print("Current encoder: ");
  Serial.println(current_encoder);
  Serial.print("Target encoder: ");
  Serial.println(target_encoder);

  if (current_encoder == target_encoder) {
    Serial.println("nothing to do");
    return;  
  }

  int steps = 0;

  if (current_encoder) {
    if (current_pos >= 0) {
      steps = flap_index - current_pos;
      if (steps < 0) {
        steps += 40;
      }
    }
  }
  Serial.print("Steps: ");
  Serial.println(steps);

  long del = 20;
  if (steps > 0) {
    del = 4000L * steps / 30 - 20;
  }
  Serial.print("Delay: ");
  Serial.println(del);

  descriptor_->target_encoder = target_encoder;
  descriptor_->target_millis = millis() + del;
  descriptor_->must_be_started = true;  
}

int8_t FlapDisplay::EncoderToPos(uint8_t encoder_value) {
  
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

uint8_t FlapDisplay::PosToEncoder(uint8_t flap_index) {
  switch (display_type_) {
    case k40Flaps:
      if (flap_index >= 40) {
        return 0;
      }
    case k62Flaps:
      if (flap_index >= 62) {
        return 0;
      }
  }
  for (uint8_t encoder = 1; encoder < 64; encoder++) {
    if (EncoderToPos(encoder) == flap_index) {
      return encoder;
    }
  }
  return 0;
}
