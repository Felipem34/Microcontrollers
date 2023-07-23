/*
insert summary here
*/

// control joystick
int Xchannel = 25;
int Ychannel = 26;
int Xvalue = 135;
int Yvalue = 135;


struct Hall {
  int pin;
  volatile bool endPulse;
  volatile unsigned long timeStart;
  volatile unsigned long timeEnd;
  unsigned long currentTime;
  unsigned long previousTime;
};

Hall ARightHall = { 21, true, 0, 0, 0, 0 };  // white - orange
Hall BRightHall = { 19, true, 0, 0, 0, 0 };  // brown - blue
Hall ALeftHall = { 18, true, 0, 0, 0, 0 };   // gray - orange
Hall BLeftHall = { 5, true, 0, 0, 0, 0 };    // blue - blue       // pin 5 must be HIGH during boot


// variables - change radius and stoppedTime accordingly
char* direction;
float velLinear = 0;
float velang = 0;
float radiusWheel = 0.165;  // in meters

int stoppedTime = 1000000;  // 1s = 1 000 000
unsigned long deltaTime;
float deltaTime2;
float f;
float rpmWheel;


// portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;


void IRAM_ATTR rightMotorISR(Hall* hall) {  //IRAM_ATTR to run on RAM

  // portENTER_CRITICAL(&mux);
  ARightHall.timeStart = BRightHall.timeEnd;
  BRightHall.timeEnd = ARightHall.timeEnd;
  ARightHall.timeEnd = micros();  // xTaskGetTickCount()

  hall->endPulse = !hall->endPulse;
  // portEXIT_CRITICAL(&mux);
}

void IRAM_ATTR leftMotorISR(Hall* hall) {

  // portENTER_CRITICAL(&mux);
  ALeftHall.timeStart = BLeftHall.timeEnd;
  BLeftHall.timeEnd = ALeftHall.timeEnd;
  ALeftHall.timeEnd = micros();

  hall->endPulse = !hall->endPulse;
  // portEXIT_CRITICAL(&mux);
}


void setup() {
  //Wifi.mode(WIFI_OFF);
  //btStop();

  Serial.begin(115200);

  pinMode(Xchannel, OUTPUT);
  pinMode(Ychannel, OUTPUT);

  pinMode(ARightHall.pin, INPUT_PULLUP);
  pinMode(BRightHall.pin, INPUT_PULLUP);
  pinMode(ALeftHall.pin, INPUT_PULLUP);
  pinMode(BLeftHall.pin, INPUT_PULLUP);

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
}

void loop() {
  // writeToJoystick();

  // String leftVel = speedWheel(&ALeftHall, &BLeftHall);
  // String rightVel = "A";  //speedWheel(&ARightHall, &BRightHall);
  // Serial.println(leftVel);


  // unsigned long currentMillis = millis();
  // if ((currentMillis - previousMillis) >= 500){
  //   previousMillis = previousMillis + 500;
  // }




  // Serial.print("RIGHT = ");
  // speedWheel(&ALeftHall, &BLeftHall);
  // Serial.print("                        LEFT  = ");
  speedWheel(&ARightHall, &BRightHall);
  // Serial.print("- - - - - - - - - - - - - - - - - - - - - - - - - -");
}

void writeToJoystick() {

  // try this
  /*
  if(Serial.available()){
    Xvalue = Serial.readStringUntil('\n');
    dacWrite(Xchannel, Xvalue);
    Serial.print("Xvalue: ");
    Serial.println(Xvalue);

    Yvalue = Serial.parseInt();
    dacWrite(Ychannel, Yvalue);
    Serial.print("Yvalue: ");
    Serial.println(Yvalue);
  }
  */


  while (Serial.available() == 0) {
  }
  Serial.println("------------------------------");

  Xvalue = Serial.parseInt();
  dacWrite(Xchannel, Xvalue);
  Serial.print("Xvalue: ");
  Serial.println(Xvalue);

  Yvalue = Serial.parseInt();
  dacWrite(Ychannel, Yvalue);
  Serial.print("Yvalue: ");
  Serial.println(Yvalue);
}

void speedWheel(Hall* Ahall, Hall* Bhall) {
  Ahall->currentTime = micros();

  // Serial.print(currentTime);
  // Serial.print(" " + String(previousTime));
  // Serial.println("  " + String(stoppedTime));

  // If the wheels have stopped for stoppedTime, reset hall values
  if (Ahall->currentTime - Ahall->previousTime >= stoppedTime) {

    Ahall->endPulse = true;
    Ahall->timeStart = 0;
    Bhall->timeEnd = 0;
    Ahall->timeEnd = 0;
    Ahall->previousTime = Ahall->currentTime;

    // direction = "STP - ";
    // velLinear = 0;

    Serial.println("paradito aqui");

    // String paradito = "paradito aqui";
    // return paradito;
  }

  // Serial.println("timeStart: " + Ahall->timeStart);

  // If there is a turn complete from A Hall sensor
  if (Ahall->timeStart && Ahall->endPulse) {

    // Determine the direction of wheel rotation based on the readings of both Hall sensors
    if ((Ahall->timeEnd - Bhall->timeEnd) > (Bhall->timeEnd - Ahall->timeStart)) {
      direction = "RVS - ";
    } else {
      direction = "FWD - ";
    }

    deltaTime = (Ahall->timeEnd - Ahall->timeStart);
    deltaTime2= deltaTime/1000000.0;
    f=1.0/deltaTime2;
    rpmWheel = f*(60.0 / 32.0);  // change name to motor rpm
    velang = ((rpmWheel * (2.0 * PI / 60.0)));
    velLinear= velang*radiusWheel*3.6;

    // Serial.println(velan);

    // ω = 2pi / Δt
    // v = ω * r    -> (m/s)
    // deltaTime = (Ahall->timeEnd - Ahall->timeStart);
    // velLinear = (radius / 100.0) * (2 * PI / (deltaTime / 1000000.0));

    // Reset the Hall sensor values and update the previousTime variable
    Ahall->timeStart = 0;
    Bhall->timeEnd = 0;
    Ahall->timeEnd = 0;
    Ahall->previousTime = Ahall->currentTime;


    Serial.println(direction + String(velLinear) + " km/h");

    // String return_string = (direction + String(velLinear) + " m/s");
    // return return_string;
  }

  // Serial.println(direction + String(velLinear) + " km/h");
}
