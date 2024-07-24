
void debug_print_with_leading_zeros(uint8_t number) {
  if (number < 100) {
    Serial.print("0");
  }
  if (number < 10) {
    Serial.print("0");
  }

  Serial.print(number);
}

void debug_println(String line) {
  Serial.print("S-");
  debug_print_with_leading_zeros(state);

  Serial.print("/");
  debug_print_with_leading_zeros(state_step);

  Serial.print(" P-");
  debug_print_with_leading_zeros(state_position);

  Serial.print(" ");
  for (uint8_t i = 0; i < BUZZERS; i++) {
    Serial.print(buzzer_active[i] ? "1" : "0");
  }

  Serial.print(" | ");
  Serial.println(line);
}

// MAIN ---------------------------------------------------------------------------------
void setup_debug() {
  Serial.begin(9600);
  while(!Serial) {
    delay(10);
  }
}

void loop_debug() {

}