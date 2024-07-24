#include "pitches.h"

// CONFIGURATION ----------------------------------------------------------------------
const uint8_t  BUZZERS              = 5;
const uint8_t  BUZZER_BUTTON_PINS[] = {8, 4, 7, 12, 13};
const uint8_t  BUZZER_LIGHT_PINS[]  = {9, 5, 6, 10, 11};
const uint8_t  BUZZER_SOUND_PIN     = 3;
const uint16_t BUZZER_NOTES[]       = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_AS4, NOTE_C5};
const uint8_t  REMOTE_PIN_LOCKED    = A0;
const uint8_t  REMOTE_PIN_SINGLE    = A2;
const uint8_t  REMOTE_PIN_ACTIVE    = A1;
const uint16_t COUNTDOWN_TICK_NOTE  = NOTE_C6;
const uint16_t COUNTDOWN_END_NOTE   = NOTE_G6;
const uint8_t  COUNTDOWN_LENGTH     = 5;

// DEBUG HELPER -----------------------------------------------------------------------
String debug_string = "";
void debug_println(String line);

// CODING SECTIONS --------------------------------------------------------------------
// strict usage hierarchy: remote -> state -> buzzer 
#include "buzzer.h"
#include "state.h"
#include "remote.h"
#include "debug.h"

// MAIN LOGIC -------------------------------------------------------------------------
void setup() {
  setup_debug();
  setup_buzzer();
  setup_state();
  setup_remote();
}

void loop() {
  loop_debug();
  loop_buzzer();
  loop_remote();
  loop_state();
}