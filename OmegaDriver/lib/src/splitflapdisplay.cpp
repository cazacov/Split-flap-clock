#include "splitflapdisplay.h"

ESP32Timer SplitFlapDisplay::itimer_(0);

DisplayDescriptor SplitFlapDisplay::displays_[16];
uint8_t SplitFlapDisplay::display_count_ = 0;
uint8_t SplitFlapDisplay::start_pin_ = 0;
uint8_t SplitFlapDisplay::adl_pin_ = 0;
uint8_t SplitFlapDisplay::data0_pin_ = 0;
uint8_t SplitFlapDisplay::data1_pin_ = 0;
uint8_t SplitFlapDisplay::data2_pin_ = 0;
uint8_t SplitFlapDisplay::data3_pin_ = 0;
uint8_t SplitFlapDisplay::data4_pin_ = 0;
uint8_t SplitFlapDisplay::data5_pin_ = 0;



SplitFlapDisplay::SplitFlapDisplay(DisplayType display_type, uint8_t adc_pin) :
    display_type_(display_type),
    adc_pin_(adc_pin)
{
  pinMode(adc_pin_, OUTPUT);
  MotorStop(adc_pin);

  displays_[display_count_].adc_pin = adc_pin;
  displays_[display_count_].is_counting = false;
  displays_[display_count_].target_millis = 0;
  displays_[display_count_].target_encoder = 0;
  displays_[display_count_].must_be_started = false;
  displays_[display_count_].current_encoder = 0;
  display_index_ = display_count_;
  display_count_++;
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

uint8_t SplitFlapDisplay::read(uint8_t adc_pin) {
  EnableModule(adc_pin);
  uint8_t input = ReadEncoder();
  DisableModule(adc_pin);
  return input;
}

void SplitFlapDisplay::init(uint8_t start_pin, uint8_t adl_pin, uint8_t data0_pin, uint8_t data1_pin, uint8_t data2_pin, uint8_t data3_pin, uint8_t data4_pin, uint8_t data5_pin) {
  start_pin_ = start_pin;
  adl_pin_ = adl_pin;
  data0_pin_ = data0_pin;
  data1_pin_ = data1_pin;
  data2_pin_ = data2_pin;
  data3_pin_ = data3_pin;
  data4_pin_ = data4_pin;
  data5_pin_ = data5_pin;
  display_count_ = 0;

  pinMode(data0_pin_, INPUT_PULLUP);
  pinMode(data1_pin_, INPUT_PULLUP);
  pinMode(data2_pin_, INPUT_PULLUP);
  pinMode(data3_pin_, INPUT_PULLUP);
  pinMode(data4_pin_, INPUT_PULLUP);
  pinMode(data5_pin_, INPUT_PULLUP);

  pinMode(start_pin_, OUTPUT);
  pinMode(adl_pin_, OUTPUT);
  digitalWrite(start_pin_, LOW);
  digitalWrite(adl_pin_, LOW);

#if USING_ESP32_S2_TIMER_INTERRUPT
  Serial.println("S2");
  Serial.println(ESP32_S2_TIMER_INTERRUPT_VERSION);
#else
  Serial.println("not S2");
  Serial.println(ESP32_TIMER_INTERRUPT_VERSION);
#endif

  if (itimer_.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, SplitFlapDisplay::TimerHandler))
  {
    Serial.print(F("Starting  ITimer OK, millis() = ")); Serial.println(millis());
  }
  else {
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));  
  }
}

int8_t SplitFlapDisplay::EncoderToPos(uint8_t encoder_value) {
  
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

uint8_t SplitFlapDisplay::PosToEncoder(uint8_t flap_index) {
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

void SplitFlapDisplay::gotoFlap(uint8_t flap_index) {

  if (isCounting()) {
    Serial.println("Is already counting. Skip gotoFlap");
    return;
  }

  uint8_t current_encoder = displays_[display_index_].current_encoder;
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

  displays_[display_index_].target_encoder = target_encoder;
  displays_[display_index_].target_millis = millis() + del;
  displays_[display_index_].must_be_started = true;  
}

void SplitFlapDisplay::EnableModule(uint8_t adc_pin) {
  digitalWrite(adl_pin_, HIGH);
  digitalWrite(adc_pin, HIGH);
  delayMicroseconds(35);
}

void SplitFlapDisplay::DisableModule(uint8_t adc_pin) {
  digitalWrite(adl_pin_, LOW);
  digitalWrite(adc_pin, LOW);
}

void SplitFlapDisplay::MotorStart(uint8_t adc_pin) {
  EnableModule(adc_pin);
  digitalWrite(start_pin_, HIGH);
  delayMicroseconds(35);
  DisableModule(adc_pin);
  delayMicroseconds(35);
  digitalWrite(start_pin_, LOW);
}

void SplitFlapDisplay::MotorStop(uint8_t adc_pin) {
  EnableModule(adc_pin);
  delayMicroseconds(35);
  DisableModule(adc_pin);
}

#if USING_ESP32_S2_TIMER_INTERRUPT
  void IRAM_ATTR SplitFlapDisplay::TimerHandler(void * timerNo)
#else
  void IRAM_ATTR SplitFlapDisplay::TimerHandler(void)
#endif
{
#if USING_ESP32_S2_TIMER_INTERRUPT
/////////////////////////////////////////////////////////
// Always call this for ESP32-S2 before processing ISR
TIMER_ISR_START(timerNo);
/////////////////////////////////////////////////////////
#endif
  unsigned long now = millis();
  for (size_t i = 0; i < display_count_; i++) {
    if (displays_[i].must_be_started) {
      displays_[i].must_be_started = false;
      MotorStart(displays_[i].adc_pin);
      displays_[i].is_counting = true;
      continue;
    }
    if (displays_[i].is_counting && displays_[i].target_millis < now) {
      MotorStop(displays_[i].adc_pin);
      uint8_t current_encoder = read(displays_[i].adc_pin);
      if (current_encoder) {
        displays_[i].current_encoder = current_encoder;
      }
      if (current_encoder == displays_[i].target_encoder) {
        displays_[i].is_counting = false;
        Serial.print("Target encoder reached: ");
        Serial.println(displays_[i].target_encoder);
      }
      else {
        displays_[i].target_millis += 20;
        MotorStart(displays_[i].adc_pin);
      }
    }
  }
#if USING_ESP32_S2_TIMER_INTERRUPT
  /////////////////////////////////////////////////////////
  // Always call this for ESP32-S2 after processing ISR
  TIMER_ISR_END(timerNo);
  /////////////////////////////////////////////////////////
#endif  
}
