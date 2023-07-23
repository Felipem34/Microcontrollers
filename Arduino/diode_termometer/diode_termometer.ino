/*
superficial explanation: https://www.instructables.com/DIY-Temperature-Sensor-Using-One-Diode/
more to know in filter: https://blog.eletrogate.com/filtro-digital-no-arduino/
*/

const int sense_pin = A0;
float voltage_ref = 1.1;
int resolution = 1024;

// values obteined empirically
float voltage_temperature_ref = 0.5884;
float temperature_ref = 20.0;
float temperature_coeff = -0.0015;  // minus, because it`s a descending linear line


unsigned long timer;
int sample_interval = 1;
int sample_qtd = 100;

void setup() {
  Serial.begin(115200);
  pinMode(sense_pin, INPUT_PULLUP);
  analogReference(INTERNAL);
  analogRead(A0);
}

void loop() {
  float voltage_read = analogRead(sense_pin) * (voltage_ref / resolution);
  float temperature = temperature_ref + ((voltage_read - voltage_temperature_ref) / temperature_coeff);

  prettyPrint(recursiveFilter(temperature));
  // Serial.println(recursiveFilter(voltage_read), 6);
}


float recursiveFilter(float voltage_measured) {
  static float voltage_filtered;

  if (millis() - timer > sample_interval) {
    voltage_filtered += (float)(voltage_measured - voltage_filtered) / (float)sample_qtd;
    timer = millis();
  } else {
    return ((float)voltage_filtered);
  }
}


void prettyPrint(float message) {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();


  message = (int)(message * 10 + 0.5) / 10.0;
  static float previousMessage;


  if (currentMillis - previousMillis >= 250) {
    previousMillis = currentMillis;
    if (message != previousMessage) {
      Serial.print("Temp = ");
      Serial.print(message, 1);
      Serial.println(" °C");
    }
    previousMessage = message;
  }
}
