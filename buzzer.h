#include <Wire.h>
#include "Adafruit_Debounce.h"

const uint8_t BUZZER_NONE = 255;

// GLOBALS ------------------------------------------------------------------------------
Adafruit_Debounce *buzzer_debouncers[BUZZERS];
bool               buzzer_active[BUZZERS];

// BUTTONS ------------------------------------------------------------------------------
uint8_t buzzer_get_first() {
  for (uint8_t i = 0; i < BUZZERS; i++) {
    if (buzzer_debouncers[i]->justPressed()) {
      return i;
    }
  }
  return BUZZER_NONE;
}

uint8_t buzzer_get_first_active() {
  for (uint8_t i = 0; i < BUZZERS; i++) {
    if (buzzer_active[i] && buzzer_debouncers[i]->justPressed()) {
      return i;
    }
  }
  return BUZZER_NONE;
}


// LIGHTS -------------------------------------------------------------------------------
void buzzer_set_light(uint8_t index, uint8_t state) {
  digitalWrite(BUZZER_LIGHT_PINS[index], state); 
}

void buzzer_all_lights_off() {
  for (uint8_t i = 0; i < BUZZERS; i++) {
    buzzer_set_light(i, LOW); 
  }
}

void buzzer_all_lights_on() {
  for (uint8_t i = 0; i < BUZZERS; i++) {
    buzzer_set_light(i, HIGH); 
  }
}

void buzzer_active_lights_on() {
  for (uint8_t i = 0; i < BUZZERS; i++) {
    buzzer_set_light(i, buzzer_active[i] ? HIGH : LOW); 
  }
}

void buzzer_single_light_on(uint8_t index) {
  for (uint8_t i = 0; i < BUZZERS; i++) {
    buzzer_set_light(i, i == index ? HIGH : LOW); 
  }
}

// ACTIVE -------------------------------------------------------------------------------
void buzzer_all_active() {
  for (uint8_t i = 0; i < BUZZERS; i++) {
    buzzer_active[i] = true;
  }
}

void buzzer_all_inactive() {
  for (uint8_t i = 0; i < BUZZERS; i++) {
    buzzer_active[i] = false;
  }
}

void buzzer_set_active(uint8_t index, bool active) {
  buzzer_active[index] = active;
}

uint8_t buzzer_get_active_count() {
  uint8_t result = 0;
  for (uint8_t i = 0; i < BUZZERS; i++) {
    if (buzzer_active[i]) {
      result++;
    }
  }
  return result;
}

uint8_t buzzer_get_random_active() {
  uint8_t active_count = buzzer_get_active_count();
  if (active_count == 0) {
    return BUZZER_NONE;
  }

  uint8_t random_index = random(active_count);
  for (uint8_t i = 0; i < BUZZERS; i++) {
    if (!buzzer_active[i]) {
      continue;
    }

    if (random_index-- == 0) {
      return i;
    }
  }
}

// SOUND --------------------------------------------------------------------------------
void buzzer_tone(uint16_t frequency, unsigned long duration) {
  tone(BUZZER_SOUND_PIN, frequency, duration);
}

void buzzer_no_tone() {
  noTone(BUZZER_SOUND_PIN);
}

// MAIN ---------------------------------------------------------------------------------
void setup_buzzer() {
  pinMode(BUZZER_SOUND_PIN, OUTPUT);
  for (uint8_t i = 0; i < BUZZERS; i++) {
    buzzer_debouncers[i] = new Adafruit_Debounce(BUZZER_BUTTON_PINS[i], LOW);
    buzzer_debouncers[i]->begin();
    pinMode(BUZZER_LIGHT_PINS[i], OUTPUT);
  }

  buzzer_all_lights_off();
  buzzer_all_active();
  buzzer_no_tone();
}

void loop_buzzer() {
  for (uint8_t i = 0; i < BUZZERS; i++) {
    buzzer_debouncers[i]->update();
  }
}
