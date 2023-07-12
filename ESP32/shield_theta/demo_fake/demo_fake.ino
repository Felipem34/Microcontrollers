// motor - only for simulating
int enB = 17;
int in4 = 16;
int in3 = 4;

int potPin = 15;
int state;
int potValue;


// control joystick
int Xchannel = 25;
int Ychannel = 26;
int Xvalue = 130;
int Yvalue = 130;


// hall banana
struct Hall {
  int pin;
  bool endPulse;
  unsigned long timeStart;
  unsigned long timeEnd;
};

Hall ALeftHall = { 19, true, 0, 0 };
Hall BLeftHall = { 21, true, 0, 0 };


// variables - change radius and stoppedTime accordingly
char* direction;
float velLinear = 0;
float radius = 2.5;  // in centimeters

int stoppedTime = 1000000;  // 1s = 1 000 000
unsigned long deltaTime = 0, previousTime = 0;


void IRAM_ATTR leftMotorISR(Hall* hall) {  //IRAM_ATTR to run on RAM
  ALeftHall.timeStart = BLeftHall.timeEnd;
  BLeftHall.timeEnd = ALeftHall.timeEnd;
  ALeftHall.timeEnd = micros();

  hall->endPulse = !hall->endPulse;
}

void setup() {
  // only for simulating
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  analogWrite(enB, 90);
  pinMode(potPin, INPUT);


  Serial.begin(9600);

  pinMode(Xchannel, OUTPUT);
  pinMode(Ychannel, OUTPUT);

  pinMode(ALeftHall.pin, INPUT_PULLUP);
  pinMode(BLeftHall.pin, INPUT_PULLUP);

  attachInterrupt(
    digitalPinToInterrupt(ALeftHall.pin), [] {
      leftMotorISR(&ALeftHall);
    },
    FALLING);
  attachInterrupt(
    digitalPinToInterrupt(BLeftHall.pin), [] {
      leftMotorISR(&BLeftHall);
    },
    FALLING);

  // dacWrite(Xchannel, 130);
  // dacWrite(Ychannel, 130);
}

void loop() {
  // writeToJoystick();
  // controlMotor();

  // speedWheel(&ALeftHall, &BLeftHall);
}

void writeToJoystick() {
  // only for simulating
  // potValue = analogRead(potPin) / 16;
  // int state = digitalRead(potPin);
  // Serial.println(state);

  while (Serial.available() > 0) {
  }
  potValue = Serial.parseInt();

  Serial.println(potValue);
  dacWrite(Xchannel, potValue);
  dacWrite(Ychannel, potValue);  

}

void controlMotor() {
  if (potValue <= 100) {
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } else if (potValue <= 160) {
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  } else {
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }
}

void speedWheel(Hall* Ahall, Hall* Bhall) {
  unsigned long currentTime = micros();

  // If the wheels have stopped for stoppedTime, reset hall values
  if (currentTime - previousTime >= stoppedTime) {
    Serial.println("paradito aqui");

    Ahall->endPulse = true;
    Ahall->timeStart = 0;
    Bhall->timeEnd = 0;
    Ahall->timeEnd = 0;
    previousTime = currentTime;
  }

  // If there is a turn complete from A Hall sensor
  if (Ahall->timeStart && Ahall->endPulse) {

    // Determine the direction of wheel rotation based on the readings of both Hall sensors
    bool directionAB = (Ahall->timeEnd - Bhall->timeEnd) > (Bhall->timeEnd - Ahall->timeStart);
    if (directionAB) {
      // Serial.print("ab");
      direction = "A -> B - ";
    } else {
      // Serial.print("ba");
      direction = "B -> A - ";
    }

    // ω = 2pi / Δt
    // v = ω * r    -> (m/s)
    deltaTime = (Ahall->timeEnd - Ahall->timeStart);
    velLinear = (radius / 100.0) * (2 * PI / (deltaTime / 1000000.0));
    Serial.println(direction + String(velLinear) + " m/s");

    // Reset the Hall sensor values and update the previousTime variable
    Ahall->timeStart = 0;
    Bhall->timeEnd = 0;
    Ahall->timeEnd = 0;
    previousTime = currentTime;
    ;
  }
}
