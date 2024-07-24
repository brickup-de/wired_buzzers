#include <Wire.h>
#include "Adafruit_Debounce.h"

Adafruit_Debounce *remote_debounce_locked = new Adafruit_Debounce(REMOTE_PIN_LOCKED, LOW);;
Adafruit_Debounce *remote_debounce_single = new Adafruit_Debounce(REMOTE_PIN_SINGLE, LOW);;
Adafruit_Debounce *remote_debounce_active = new Adafruit_Debounce(REMOTE_PIN_ACTIVE, LOW);;

// HANDLER ------------------------------------------------------------------------------
void remote_handle_locked() {
  if (state_is_locked()) {
    return;
  }

  if (state_is_single()) {
    state_set(STATE_BUZZERS_LOCKED_INACTIVE);
    return;
  }

  if (state_is_active()) {
    state_set(STATE_BUZZERS_LOCKED, STATE_POSITION_NONE);
    return;
  }
}

void remote_handle_single() {
  if (state_is_locked()) {
    state_set(STATE_BUZZERS_SINGLE_RANDOM);
    return;
  }

  if (state_is_single()) {
    state_set(STATE_BUZZERS_SINGLE);
    return;
  }

  if (state_is_active()) {
    state_set(STATE_BUZZERS_SINGLE_RANDOM);
    return;
  }
}

void remote_handle_active() {
  if (state_is_locked()) {
    state_set(STATE_BUZZERS_ACTIVE, STATE_POSITION_NONE);
    return;
  }

  if (state_is_single()) {
    state_set(STATE_BUZZERS_ACTIVE_RESET);
    return;
  }

  if (state_is_active()) {
    state_set(STATE_BUZZERS_ACTIVE_RESET);
    return;
  }
}

// MAIN ---------------------------------------------------------------------------------
void setup_remote() {
  remote_debounce_locked->begin();
  remote_debounce_single->begin();
  remote_debounce_active->begin();
}

void loop_remote() {
  remote_debounce_locked->update();
  remote_debounce_single->update();
  remote_debounce_active->update();

  if (remote_debounce_locked->justPressed()) {
    debug_println("Pressed LOCKED");
    remote_handle_locked();
    return;
  }

  if (remote_debounce_single->justPressed()) {
    debug_println("Pressed SINGLE");
    remote_handle_single();
    return;
  }

  if (remote_debounce_active->justPressed()) {
    debug_println("Pressed ACTIVE");
    remote_handle_active();
    return;
  }
}