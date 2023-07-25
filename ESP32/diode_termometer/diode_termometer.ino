const int sensPin = A5;  // A5 for arduino, D33 for ESP32
float VCC = 3.3;
int resolution = 4096;  // 1024 for arduino, 4096 for ESP32

float voltRef = 0.51;  // 0.4254 for arduino, 0.51 for ESP32
float tempRef = 20.0;
float tempCoeff = -0.015;  // -0.0015 for arduino, 4096 for ESP32 reta linear decrescente


void setup() {
  Serial.begin(115200);
}

void loop() {
  float voltRead = analogRead(sensPin) * (VCC / resolution);
  float temp = tempRef + ((voltRead - voltRef) / tempCoeff);

  // printStateChange(temp);
  Serial.println(temp);

  delay(500);
}

void printStateChange(float message) {

  message = (int)(message * 100 + 0.5) / 100.0;
  static float previousMessage;

  if (message != previousMessage) {
    Serial.print("Temp = ");
    Serial.print(message);
    Serial.println(" °C");
  }
  previousMessage = message;
}
