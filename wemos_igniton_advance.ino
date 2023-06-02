const int ignitionPin = D5;      // Pin for ignition points
const int mosfetPin = D0;        // Pin for NPN MOSFET

const int rpmThresholds[13] = {500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500};  // RPM thresholds
const int timingAdvanceMin = 5;    // Minimum timing advance in degrees
const int timingAdvanceMax = 32;   // Maximum timing advance in degrees
const int revLimitRPM = 6000;      // Maximum RPM limit

unsigned long previousTime = 0;  // Variable to store the previous time of ignition event
unsigned long rpmPeriod = 0;     // Variable to store the time period between ignition events
int rpm = 0;                     // Variable to store the calculated RPM value

void setup() {
  pinMode(ignitionPin, INPUT_PULLUP);
  pinMode(mosfetPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ignitionPin), ignitionEvent, RISING);
}

void loop() {
  if (rpm <= revLimitRPM) {
    int timingAdvance = getTimingAdvance(rpm);
  
    if (timingAdvance > 0) {
      digitalWrite(mosfetPin, HIGH);  // Turn on NPN MOSFET
      delayMicroseconds(timingAdvance);
      digitalWrite(mosfetPin, LOW);   // Turn off NPN MOSFET
      delayMicroseconds(1000 - timingAdvance);
    } else {
      digitalWrite(mosfetPin, LOW);   // Turn off NPN MOSFET
      delayMicroseconds(1000);
    }
  } else {
    // RPM exceeds maximum limit (rev limiter action)
    digitalWrite(mosfetPin, LOW);   // Turn off NPN MOSFET
    delayMicroseconds(1000);
  }
}

void ignitionEvent() {
  unsigned long currentTime = micros();
  rpmPeriod = currentTime - previousTime;
  previousTime = currentTime;
  
  if (rpmPeriod > 0) {
    rpm = 60000000 / rpmPeriod;  // Calculate RPM based on the time period between ignition events
  } else {
    rpm = 0;
  }
}

int getTimingAdvance(int rpm) {
  for (int i = 0; i < 13; i++) {
    if (rpm < rpmThresholds[i]) {
      int rpmRange = rpmThresholds[i] - rpmThresholds[i - 1];
      int advanceRange = timingAdvanceMax - timingAdvanceMin;
      int rpmDifference = rpm - rpmThresholds[i - 1];
      
      int timingAdvance = timingAdvanceMin + (advanceRange * rpmDifference) / rpmRange;
      return timingAdvance;
    }
  }
  
  // If RPM exceeds the highest threshold, return the maximum timing advance
  return timingAdvanceMax;
}
