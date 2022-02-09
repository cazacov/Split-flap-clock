#pragma once

struct DisplayDescriptor {
  volatile int adc_pin;
  volatile bool must_be_started;
  volatile bool is_counting;
  volatile long target_millis;
  volatile int target_encoder;
  volatile uint8_t current_encoder;
};