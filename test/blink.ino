// Simple LED blink test
void setup() {
  pinMode(13, OUTPUT);  // D13 LED pin
}

void loop() {
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
}
