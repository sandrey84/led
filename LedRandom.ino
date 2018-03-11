

//pins
const int LEFT_FRONT_LED_PIN = 32;
const int LEFT_HOOK_LED_PIN = 30;
const int RIGHT_FRONT_LED_PIN = 33;
const int RIGHT_HOOK_LED_PIN = 31;
const int LEFT_RIGHT_RESISTOR_PIN = A1;
const int SPEED_RESISTOR_PIN = A0;

//app constants
const int LED_ON = 255;
const int LED_OFF = 0;
const int LED_ON_MILLIS = 100;
const int LED_PAUSE_MILLIS = 100;
const int MIN_INTERVAL = 500;
const int MAX_INTERVAL = 4000;
const long INTERVAL_RANDOM_COEFF = 60;//% -> 0.6
const long SPURT_INTERVAL_COEFF = 70;//% -> 0.7
const long SPURT_PROBABILITY_COEFF = 5;//% -> 0.5
const int LEFT_FRONT = 0;
const int LEFT_HOOK = 1;
const int RIGHT_FRONT = 2;
const int RIGHT_HOOK = 3;
const int ANALOG_RANGE = 1024;
const int MAX_SPURT = 10;

//variables
long leftFrontTurnOnMillis;
long leftHookTurnOnMillis;
long rightFrontTurnOnMillis;
long rightHookTurnOnMillis;
long lastTurnOn;
long nextDelay;
boolean spurtTime;
int spurtCounter;
int currentSpurt;


void setup() { 
  pinMode(LEFT_FRONT_LED_PIN, OUTPUT);
  pinMode(LEFT_HOOK_LED_PIN, OUTPUT);
  pinMode(RIGHT_FRONT_LED_PIN, OUTPUT);
  pinMode(RIGHT_HOOK_LED_PIN, OUTPUT);
//  Serial.begin(9600); 
} 

void loop() {
  turnOffWithDelay();

  if (isTimeToTurnOn()) {
    turnOn(getWhichTurnOn());
    processSpurt();
    nextDelay = getNextDelay();
  }

  delay(25);
}

void turnOffWithDelay() {
  long now = millis();

  if(leftFrontTurnOnMillis + LED_ON_MILLIS < now) {
    analogWrite(LEFT_FRONT_LED_PIN, LED_OFF);
  }
  
  if(leftHookTurnOnMillis + LED_ON_MILLIS < now) {
    analogWrite(LEFT_HOOK_LED_PIN, LED_OFF);
  }
  
  if(rightFrontTurnOnMillis + LED_ON_MILLIS < now) {
    analogWrite(RIGHT_FRONT_LED_PIN, LED_OFF);
  }

  if(rightHookTurnOnMillis + LED_ON_MILLIS < now) {
    analogWrite(RIGHT_HOOK_LED_PIN, LED_OFF);
  }
}

void turnOn(int led) {
  long now = millis();
  lastTurnOn = now;
  
  switch (led) {
    case LEFT_FRONT:
      analogWrite(LEFT_FRONT_LED_PIN, LED_ON);
      leftFrontTurnOnMillis = now;
      break;
    case LEFT_HOOK:
      analogWrite(LEFT_HOOK_LED_PIN, LED_ON);
      leftHookTurnOnMillis = now;
      break;
    case RIGHT_FRONT:
      analogWrite(RIGHT_FRONT_LED_PIN, LED_ON);
       rightFrontTurnOnMillis = now;
      break;
    case RIGHT_HOOK:
      analogWrite(RIGHT_HOOK_LED_PIN, LED_ON);
      rightHookTurnOnMillis = now;
      break;
  }
}

int getNextDelay() {
  long determinedDelay = map(analogRead(SPEED_RESISTOR_PIN), 0, 1023, MIN_INTERVAL, MAX_INTERVAL);
  
  return spurtTime
      ? determinedDelay - (determinedDelay * SPURT_INTERVAL_COEFF / 100.0)
      : determinedDelay - (determinedDelay * random(INTERVAL_RANDOM_COEFF) / 100.0);
}

boolean isTimeToTurnOn() {
  return lastTurnOn + nextDelay < millis();
}

int getWhichTurnOn() {
  int rnd = random(2);
  return shouldTurnOnLeft()
   ? rnd
   : rnd + 2;
}

boolean shouldTurnOnLeft() {
  return random(ANALOG_RANGE) > analogRead(LEFT_RIGHT_RESISTOR_PIN);
}

void processSpurt() {
  if (spurtTime && spurtCounter < currentSpurt - 1) {
    spurtCounter++;
    return;
  } else if (spurtTime) {
    spurtCounter = 0;
    spurtTime = false;
    return;
  }

  spurtTime = random(100) < SPURT_PROBABILITY_COEFF;

  if (spurtTime) {
    currentSpurt = random(3, MAX_SPURT + 1);
  }
}



