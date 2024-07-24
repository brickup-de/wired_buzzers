// STATES MACHINE -----------------------------------------------------------------------
const uint8_t STATE_NONE                     = 0;
                                                   // LIGHTS  | BUTTONS | WAITING FOR    -> NEXT STATE
const uint8_t STATE_BUZZERS                  = 10;
const uint8_t STATE_BUZZERS_LOCKED           = 11; // lights off, ignore buzzers
const uint8_t STATE_BUZZERS_LOCKED_INACTIVE  = 12; // set current buzzer inactive, move to LOCKED
const uint8_t STATE_BUZZERS_ACTIVE           = 13; // lights on for active buzzers, wait for first buzz, move to SINGLE
const uint8_t STATE_BUZZERS_ACTIVE_RESET     = 14; // set all buzzers active, move to ACTIVE
const uint8_t STATE_BUZZERS_SINGLE           = 15; // lights on for current buzzer
const uint8_t STATE_BUZZERS_SINGLE_BUZZED    = 16; // buzz effect for current buzzer, move to SINGLE_COUNTDOWN
const uint8_t STATE_BUZZERS_SINGLE_COUNTDOWN = 17; // countdown seconds, move to SINGLE
const uint8_t STATE_BUZZERS_SINGLE_RANDOM    = 18; // pick random buzzer from actives, set current, move to SINGLE_BUZZED

// const uint8_t STATE_SIMON             = 20;
// const uint8_t STATE_SIMON_LOCKED      = 21; // none    | ignore  | command        -> NEXT / PLAY
// const uint8_t STATE_SIMON_PLAY        = 22; // iterate | ignore  | sequence ends  -> LISTEN
// const uint8_t STATE_SIMON_LISTEN      = 23; // none    | first   | button pressed -> FALSE / NEXT
// const uint8_t STATE_SIMON_FALSE       = 24; // correct | ignore  | (animation)    -> LOCKED
// const uint8_t STATE_SIMON_NEXT        = 25; // none    | ignore  | (sequence + 1) -> PLAY
// const uint8_t STATE_SIMON_RESET       = 26; // none    | ignore  | (del sequence) -> LOCKED

// const uint8_t STATE_RANDOM            = 30;
// const uint8_t STATE_RANDOM_PICKING    = 31; // random  | ignore  | (animation)    -> ACTIVE
// const uint8_t STATE_RANDOM_ACTIVE     = 32; // picked  | ignore  | command        -> PICKING / BUZZERS_ACTIVE

const uint8_t  STATE_POSITION_NONE = 255;

// GLOBALS ------------------------------------------------------------------------------
uint8_t state;
uint8_t state_step;
long    state_step_millis;
bool    state_has_changed;
bool    state_has_changed_in_scope;
uint8_t state_position;

// STATE SETTER -------------------------------------------------------------------------
void state_set(uint8_t _state, uint8_t _position) {
  state      = _state;
  state_step = 0UL;
  debug_println("New state");

  if (state_position != _position) {
    state_position = _position;
    debug_println("New state position");
  }

  state_step_millis          = millis();
  state_has_changed          = true;
  state_has_changed_in_scope = true;
  buzzer_no_tone();
}

void state_set(uint8_t _state) {
  state_set(_state, state_position);
}

// STATE GETTER -------------------------------------------------------------------------
bool state_is_locked() {
  return (state == STATE_BUZZERS_LOCKED 
       || state == STATE_BUZZERS_LOCKED_INACTIVE);
}

bool state_is_single() {
  return (state == STATE_BUZZERS_SINGLE
       || state == STATE_BUZZERS_SINGLE_BUZZED
       || state == STATE_BUZZERS_SINGLE_COUNTDOWN
       || state == STATE_BUZZERS_SINGLE_RANDOM);
}

bool state_is_active() {
  return (state == STATE_BUZZERS_ACTIVE
       || state == STATE_BUZZERS_ACTIVE_RESET);
}

// TIMING HELPERS -----------------------------------------------------------------------
// replacements for "delay(...)" as main loop must always stay responsive
bool state_is_next_delay(unsigned long delay, uint8_t index, uint8_t repeat) {
  if (state_step < index || state_step >= index + repeat) {
    return false;
  }

  if (millis() < state_step_millis + delay) {
    return false;
  }

  state_step++;
  state_step_millis = millis();
  debug_println("Next state step");
  return true;
}

bool state_is_next_delay(unsigned long delay, uint8_t index) {
  return state_is_next_delay(delay, index, 1);
}

bool state_is_next_delay(unsigned long delay) {
  return state_is_next_delay(delay, 0, 255);
}

// STATE MACHINE: BUZZER MODE -----------------------------------------------------------
void state_handle_buzzers_locked() {
  if (state_has_changed) {
    buzzer_all_lights_off();
    return;
  }
}

void state_handle_buzzers_locked_inactive() {
  buzzer_set_active(state_position, false);
  state_set(STATE_BUZZERS_LOCKED, STATE_POSITION_NONE);
}


void state_handle_buzzers_active() {
  if (state_has_changed) {
    buzzer_active_lights_on();
    return;
  }

  uint8_t first = buzzer_get_first_active();
  if (first == BUZZER_NONE) {
    return;
  }

  state_set(STATE_BUZZERS_SINGLE_BUZZED, first);
}

void state_handle_buzzers_active_reset() {
  buzzer_all_active();
  state_set(STATE_BUZZERS_ACTIVE, STATE_POSITION_NONE);
}

void state_handle_buzzers_single() {
  if (state_has_changed) {
    buzzer_single_light_on(state_position);
    return;
  }
}

void state_handle_buzzers_single_buzzed() {
  static bool single_light_is_on;
  
  if (state_has_changed) {
    single_light_is_on = false;
  }

  if (state_is_next_delay(10UL, 0)) {
    buzzer_tone(BUZZER_NOTES[state_position], 990UL);
  }

  if (state_is_next_delay(198UL, 1, 5)) {
    if (single_light_is_on) {
      buzzer_single_light_on(state_position);
    } else {
      buzzer_all_lights_off();
    }
    single_light_is_on = !single_light_is_on;
    return;
  }

  if (state_is_next_delay(200UL, 6)) {
    state_set(STATE_BUZZERS_SINGLE_COUNTDOWN);
    return;
  }
}

void state_handle_buzzers_single_countdown() {
  static uint8_t repeats;
  if (state_has_changed) {
    buzzer_single_light_on(state_position);
    return;
  }

  if (state_is_next_delay(1000UL, 0, COUNTDOWN_LENGTH - 1)) {
    buzzer_tone(COUNTDOWN_TICK_NOTE, 250UL);
  }

  if (state_is_next_delay(1000UL, COUNTDOWN_LENGTH - 1)) {
    buzzer_tone(COUNTDOWN_END_NOTE, 500UL);
  }

  if (state_is_next_delay(1000UL, COUNTDOWN_LENGTH)) {
    state_set(STATE_BUZZERS_SINGLE);
  }
}

void state_handle_buzzers_single_random() {
  uint8_t random_state_position = buzzer_get_random_active();
  if (random_state_position == STATE_POSITION_NONE) {
    state_set(STATE_BUZZERS_LOCKED, STATE_POSITION_NONE);
  } else {
    state_set(STATE_BUZZERS_SINGLE_BUZZED, random_state_position);
  }
}

// MAIN ---------------------------------------------------------------------------------
void setup_state() {
  state_set(STATE_BUZZERS_LOCKED, STATE_POSITION_NONE);
}

void loop_state() {
  state_has_changed_in_scope = false;
  switch (state) {
    case STATE_NONE:
      break;
    case STATE_BUZZERS:
    case STATE_BUZZERS_LOCKED:
      state_handle_buzzers_locked();
      break;
    case STATE_BUZZERS_LOCKED_INACTIVE:
      state_handle_buzzers_locked_inactive();
      break;
    case STATE_BUZZERS_ACTIVE:
      state_handle_buzzers_active();
      break;
    case STATE_BUZZERS_ACTIVE_RESET:
      state_handle_buzzers_active_reset();
      break;
    case STATE_BUZZERS_SINGLE:
      state_handle_buzzers_single();
      break;
    case STATE_BUZZERS_SINGLE_BUZZED:
      state_handle_buzzers_single_buzzed();
      break;
    case STATE_BUZZERS_SINGLE_COUNTDOWN:
      state_handle_buzzers_single_countdown();
      break;
    case STATE_BUZZERS_SINGLE_RANDOM:
      state_handle_buzzers_single_random();
      break;
  }

  if (!state_has_changed_in_scope) {
    state_has_changed = false;
  }
}
