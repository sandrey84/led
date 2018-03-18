

//pins
const int HIGH_LEFT_UPPERCUT_LED_PIN = 48;
const int HIGH_LEFT_FRONT_LED_PIN = 50;
const int HIGH_LEFT_HOOK_LED_PIN = 52;

const int HIGH_RIGHT_UPPERCUT_LED_PIN = 49;
const int HIGH_RIGHT_FRONT_LED_PIN = 51;
const int HIGH_RIGHT_HOOK_LED_PIN = 53;

const int SPEED_RESISTOR_PIN = A0;
const int UPPERCUT_PROBABILITY_PIN = A1;
const int FRONT_PROBABILITY_PIN = A2;
const int HOOK_PROBABILITY_PIN = A3;
const int LEFT_RIGHT_RESISTOR_PIN = A4;
const int LED_ON_PERIOD_RESISTOR_PIN = A5;

//app constants
const int LED_ON = 255;
const int LED_OFF = 0;
const int MIN_HIT_INTERVAL_MS = 500;
const int MAX_HIT_INTERVAL_MS = 4000;
const long INTERVAL_RANDOM_COEFF = 60;//% -> 0.6
const long SPURT_INTERVAL_COEFF = 70;//% -> 0.7
const long SPURT_PROBABILITY_COEFF = 5;//% -> 0.5
const int HIGH_LEFT_UPPERCUT = 0;
const int HIGH_LEFT_FRONT = 1;
const int HIGH_LEFT_HOOK = 2;
const int HIGH_RIGHT_UPPERCUT = 3;
const int HIGH_RIGHT_FRONT = 4;
const int HIGH_RIGHT_HOOK = 5;
const int ANALOG_MIN = 0;
const int ANALOG_MAX = 1023;
const int ANALOG_RANGE = 1024;
const int ANALOG_MIN_THRESHOLD = 15;
const int MAX_HITS_IN_SPURT = 10;
const int LED_ON_MIN_PERIOD_MS = 50;
const int LED_ON_MAX_PERIOD_MS = 500;

//variables
long highLeftFrontTurnOnMillis;
long highLeftHookTurnOnMillis;
long highLeftUppercutTurnOnMillis;
long highRightFrontTurnOnMillis;
long highRightHookTurnOnMillis;
long highRightUppercutTurnOnMillis;
long lastTurnOn;
long nextDelay;
boolean spurtTime;
int currentSpurtHit;
int totalHitsInThisSpurt;


void setup() { 
  pinMode(HIGH_LEFT_UPPERCUT_LED_PIN, OUTPUT);
  pinMode(HIGH_LEFT_FRONT_LED_PIN, OUTPUT);
  pinMode(HIGH_LEFT_HOOK_LED_PIN, OUTPUT);
  pinMode(HIGH_RIGHT_UPPERCUT_LED_PIN, OUTPUT);
  pinMode(HIGH_RIGHT_FRONT_LED_PIN, OUTPUT);
  pinMode(HIGH_RIGHT_HOOK_LED_PIN, OUTPUT);
  Serial.begin(9600);
} 

void loop() {
  turnOffWithDelay();
 
  if (isTimeToTurnOn()) {
    turnOn(getWhichToTurnOn());
    processSpurt();
    nextDelay = getNextDelay();
  }

  delay(25);
}

void turnOffWithDelay() {
  long now = millis();
  int ledOnMillis = map(analogRead(LED_ON_PERIOD_RESISTOR_PIN), 
                        ANALOG_MIN, 
                        ANALOG_MAX, 
                        LED_ON_MIN_PERIOD_MS, 
                        LED_ON_MAX_PERIOD_MS);

  if(highLeftFrontTurnOnMillis + ledOnMillis < now) {
    analogWrite(HIGH_LEFT_FRONT_LED_PIN, LED_OFF);
  }
  
  if(highLeftHookTurnOnMillis + ledOnMillis < now) {
    analogWrite(HIGH_LEFT_HOOK_LED_PIN, LED_OFF);
  }

  if(highLeftUppercutTurnOnMillis + ledOnMillis < now) {
    analogWrite(HIGH_LEFT_UPPERCUT_LED_PIN, LED_OFF);
  }
  
  if(highRightFrontTurnOnMillis + ledOnMillis < now) {
    analogWrite(HIGH_RIGHT_FRONT_LED_PIN, LED_OFF);
  }

  if(highRightHookTurnOnMillis + ledOnMillis < now) {
    analogWrite(HIGH_RIGHT_HOOK_LED_PIN, LED_OFF);
  }

  if(highRightUppercutTurnOnMillis + ledOnMillis < now) {
    analogWrite(HIGH_RIGHT_UPPERCUT_LED_PIN, LED_OFF);
  }
}

void turnOn(int led) {
  long now = millis();
  lastTurnOn = now;
  
  switch (led) {
    case HIGH_LEFT_FRONT:
      analogWrite(HIGH_LEFT_FRONT_LED_PIN, LED_ON);
      highLeftFrontTurnOnMillis = now;
      break;
    case HIGH_LEFT_HOOK:
      analogWrite(HIGH_LEFT_HOOK_LED_PIN, LED_ON);
      highLeftHookTurnOnMillis = now;
      break;
    case HIGH_LEFT_UPPERCUT:
      analogWrite(HIGH_LEFT_UPPERCUT_LED_PIN, LED_ON);
      highLeftUppercutTurnOnMillis = now;
      break;  
    case HIGH_RIGHT_FRONT:
      analogWrite(HIGH_RIGHT_FRONT_LED_PIN, LED_ON);
       highRightFrontTurnOnMillis = now;
      break;
    case HIGH_RIGHT_HOOK:
      analogWrite(HIGH_RIGHT_HOOK_LED_PIN, LED_ON);
      highRightHookTurnOnMillis = now;
      break;
    case HIGH_RIGHT_UPPERCUT:
      analogWrite(HIGH_RIGHT_UPPERCUT_LED_PIN, LED_ON);
      highRightUppercutTurnOnMillis = now;
      break;
  }
}

int getNextDelay() {
  long determinedDelay = map(analogRead(SPEED_RESISTOR_PIN), 
                            ANALOG_MIN, 
                            ANALOG_MAX, 
                            MIN_HIT_INTERVAL_MS, 
                            MAX_HIT_INTERVAL_MS);
  
  return spurtTime
      ? determinedDelay - (determinedDelay * SPURT_INTERVAL_COEFF / 100.0)
      : determinedDelay - (determinedDelay * random(INTERVAL_RANDOM_COEFF) / 100.0);
}

boolean isTimeToTurnOn() {
  return lastTurnOn + nextDelay < millis();
}

int getWhichToTurnOn() {
  int frontResistorPosition = analogRead(FRONT_PROBABILITY_PIN);
  int uppercutResistorPosition = analogRead(UPPERCUT_PROBABILITY_PIN);
  int hookResistorPosition = analogRead(HOOK_PROBABILITY_PIN);
  
  int frontRandomValue = random(frontResistorPosition);
  int uppercutRandomValue = random(uppercutResistorPosition);
  int hookRandomValue = random(hookResistorPosition);

  Serial.println(frontResistorPosition);
  Serial.println(uppercutResistorPosition);
  Serial.println(hookResistorPosition);
  Serial.println(frontRandomValue);
  Serial.println(uppercutRandomValue);
  Serial.println(hookRandomValue);
  Serial.println("");

  if (frontResistorPosition <= ANALOG_MIN_THRESHOLD
        && uppercutResistorPosition <= ANALOG_MIN_THRESHOLD
        && hookResistorPosition <= ANALOG_MIN_THRESHOLD) {
          
    return -1; //not to turn on any led  
  }
  
  if (frontRandomValue > ANALOG_MIN_THRESHOLD 
        && frontRandomValue > uppercutRandomValue 
        && frontRandomValue > hookRandomValue) {
          
    return shouldTurnOnLeft() ? HIGH_LEFT_FRONT: HIGH_RIGHT_FRONT;
  }

  if (uppercutRandomValue > ANALOG_MIN_THRESHOLD
        && uppercutRandomValue > frontRandomValue 
        && uppercutRandomValue > hookRandomValue) {
          
    return shouldTurnOnLeft() ? HIGH_LEFT_UPPERCUT : HIGH_RIGHT_UPPERCUT;
  }

  return shouldTurnOnLeft() ? HIGH_LEFT_HOOK : HIGH_RIGHT_HOOK;
}

boolean shouldTurnOnLeft() {
  return random(ANALOG_RANGE) > analogRead(LEFT_RIGHT_RESISTOR_PIN);
}

void processSpurt() {
  if (spurtTime && currentSpurtHit < totalHitsInThisSpurt - 1) {
    currentSpurtHit++;
    return;
  } else if (spurtTime) {
    currentSpurtHit = 0;
    spurtTime = false;
    return;
  }

  spurtTime = random(100) < SPURT_PROBABILITY_COEFF;

  if (spurtTime) {
    totalHitsInThisSpurt = random(3, MAX_HITS_IN_SPURT + 1);
  }
}



