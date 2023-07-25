/*
superficial explanation: https://www.instructables.com/DIY-Temperature-Sensor-Using-One-Diode/
more to know in filter: https://blog.eletrogate.com/filtro-digital-no-arduino/
*/

#include <LiquidCrystal.h>
LiquidCrystal lcd(9, 8, 5, 4, 3, 2);  // lcd(RS, enable, D4, D5, D6, D7)

const int sense_pin = A0;
float voltage_ref = 1.1;
int resolution = 1024;

// values empirically obteined, need to calibrate properly
float voltage_temperature_ref = 0.5793;
float temperature_ref = 15.0;
float temperature_coeff = 0.002;  // typical diode temperature coefficient


void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  pinMode(sense_pin, INPUT_PULLUP);
  analogReference(INTERNAL);  // for 1.1V regulator, if used a different voltage_ref, comment this line
}

void loop() {
  float voltage_read = analogRead(sense_pin) * (voltage_ref / resolution);
  voltage_read = (int)(voltage_read * 10000 + 0.5) / 10000.0;  // round to 4 decimal places
  float temperature = temperature_ref - ((voltage_read - voltage_temperature_ref) / temperature_coeff);

  temperature = recursiveFilter(temperature);
  prettyPrint(temperature, 1, 500);
  // Serial.println(voltage_read, 4);  // for calibration
  // printLCD(temperature);
}


float recursiveFilter(float voltage_measured) {
  static int sample_interval = 10;  // microseconds
  static int sample_qtd = 100;
  static float voltage_filtered;
  static unsigned long timer;

  if (millis() - timer > sample_interval) {
    voltage_filtered += (float)(voltage_measured - voltage_filtered) / (float)sample_qtd;
    timer = millis();
  } else {
    return ((float)voltage_filtered);
  }
}


void prettyPrint(float message, int decimal_place, int interval) {
  static unsigned long previousMillis = 0;
  static float previousMessage;
  unsigned long currentMillis = millis();

  message = (int)(message * 10 + 0.5) / 10.0;

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (message != previousMessage) {
      Serial.print(message, decimal_place);
      Serial.println(" °C");
    }
    previousMessage = message;
  }
}


void printLCD(float message) {
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("TEMP:");
  lcd.setCursor(10, 1);
  lcd.print(String(message, 1));
  lcd.setCursor(14, 1);
  lcd.print((char)223);  // "°" simbol
  lcd.setCursor(15, 1);
  lcd.print("C");
}
