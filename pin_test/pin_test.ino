// PIN CHECK - Compare these values to your actual wire connections

#define TFT_CS_PIN              11   // Display CS
#define TFT_DC_PIN              8    // Display DC
#define TFT_RST_PIN             7    // Display RES
#define TFT_MOSI_PIN            12   // Display SDA (data)
#define TFT_CLK_PIN             13   // Display SCL (clock)
#define MAX6675_CS_PIN          10   // Thermocouple CS

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println(F("\n=== VERIFY YOUR CONNECTIONS ===\n"));
  Serial.println(F("Display VCC → 3v3"));
  Serial.println(F("Display GND → GND"));
  Serial.println(F("Display BL (backlight) → 3v3\n"));
  
  Serial.println(F("Signal pins:"));
  Serial.print(F("  Display RES → D"));     Serial.println(TFT_RST_PIN);
  Serial.print(F("  Display DC → D"));      Serial.println(TFT_DC_PIN);
  Serial.print(F("  Display CS → D"));      Serial.println(TFT_CS_PIN);
  Serial.print(F("  Display SDA (data) → D")); Serial.println(TFT_MOSI_PIN);
  Serial.print(F("  Display SCL (clock) → D")); Serial.println(TFT_CLK_PIN);
  
  Serial.println(F("\nThermocouple:"));
  Serial.print(F("  MAX6675 CS → D"));      Serial.println(MAX6675_CS_PIN);
  Serial.println(F("  MAX6675 CLK → D13 (shared with display)"));
  Serial.println(F("  MAX6675 MISO → D12 (shared with display)\n"));
  
  Serial.println(F("Do all your wires match? Answer YES/NO\n"));
}

void loop() {
  delay(10000);
}
