const int buttonPin = 2;
int buttonState;
int buttonLastState;
int buttonPushCounter;

unsigned long lastDebounceTime;
unsigned long debounceDelay = 50;

const int ledPin = 13;
int ledState;


const int voltPin = A0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  Serial.begin(115200);
}

void loop() {
  float voltValue = analogRead(voltPin) * (5. / 1023.);
  printStateChange(voltValue);
  // Serial.println(voltValue, 8);
  // printXSeconds(voltValue, 500)

  int reading = digitalRead(buttonPin);

  if (reading != buttonLastState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        ledState = !ledState;
        buttonPushCounter++;
        // Serial.println(buttonPushCounter);
      }
    }
  }

  digitalWrite(ledPin, ledState);
  buttonLastState = reading;
}

void printStateChange(float message) {
  // Serial.begin(9600);

  message = (int)(message * 100 + 0.5) / 100.0;

  static float previousMessage = 0;

  if (message != previousMessage) {
    Serial.println(message);
  }
  previousMessage = message;
}

void printXSeconds(float message, unsigned long interval) {
  // Serial.begin(9600);

  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Serial.println(message);
  }
}

// filtering
int sample_interval = 1;
int sample_qtd = 100;

float recursiveFilter(float voltage_measured) {
  static float voltage_filtered;
  static unsigned long timer;

  if (millis() - timer > sample_interval) {
    voltage_filtered += (float)(voltage_measured - voltage_filtered) / (float)sample_qtd;
    timer = millis();
  } else {
    return ((float)voltage_filtered);
  }
}


