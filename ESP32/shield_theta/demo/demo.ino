/*
insert summary here
*/

// control joystick
int Xchannel = 25;
int Ychannel = 26;
int Xvalue = 135;
int Yvalue = 135;


// hall banana
struct Hall {
  int pin;
  bool endPulse;
  unsigned long timeStart;
  unsigned long timeEnd;
};

Hall ALeftHall = { 22, true, 0, 0 };
Hall BLeftHall = { 23, true, 0, 0 };
Hall ARightHall = { 1, true, 0, 0 };
Hall BRightHall = { 3, true, 0, 0 };


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

void IRAM_ATTR rightMotorISR(Hall* hall) {
  ARightHall.timeStart = BRightHall.timeEnd;
  BRightHall.timeEnd = ARightHall.timeEnd;
  ARightHall.timeEnd = micros();

  hall->endPulse = !hall->endPulse;
}

void setup() {
  Serial.begin(250000);
  Serial.println("setup");

  pinMode(Xchannel, OUTPUT);
  pinMode(Ychannel, OUTPUT);

  pinMode(ALeftHall.pin, INPUT);
  pinMode(BLeftHall.pin, INPUT);
  pinMode(ARightHall.pin, INPUT_PULLUP);
  pinMode(BRightHall.pin, INPUT_PULLUP);

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

  attachInterrupt(
    digitalPinToInterrupt(ARightHall.pin), [] {
      rightMotorISR(&ARightHall);
    },
    FALLING);
  attachInterrupt(
    digitalPinToInterrupt(BRightHall.pin), [] {
      rightMotorISR(&BRightHall);
    },
    FALLING);
}

void loop() {
  Serial.println("1loop");
  // writeToJoystick();

  speedWheel(&ALeftHall, &BLeftHall);
  speedWheel(&ARightHall, &BRightHall);
  Serial.println("2loop");
}

void writeToJoystick() {
  dacWrite(Xchannel, Xvalue);
  dacWrite(Ychannel, Yvalue);
}

void speedWheel(Hall* Ahall, Hall* Bhall) {
  unsigned long currentTime = micros();

   Serial.println("speedwheel");

  // If the wheels have stopped for stoppedTime, reset hall values
  if (currentTime - previousTime >= stoppedTime) {
    // Serial.println("paradito aqui");

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
      direction = "A -> B - ";
    } else {
      direction = "B -> A - ";
    }

    // ω = 2pi / Δt
    // v = ω * r    -> (m/s)
    deltaTime = (Ahall->timeEnd - Ahall->timeStart);
    velLinear = (radius / 100.0) * (2 * PI / (deltaTime / 1000000.0));
    // Serial.println(direction);// + String(velLinear) + " m/s");
    

    // Reset the Hall sensor values and update the previousTime variable
    Ahall->timeStart = 0;
    Bhall->timeEnd = 0;
    Ahall->timeEnd = 0;
    previousTime = currentTime;
    ;
  }
}
