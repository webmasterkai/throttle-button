// This is a simple button throttle sketch for a digispark.

// Connect this to the controller green wire.
int throttlePin = 1; // Also PCB LED.
// Connect this to one side of the brake output wire.
int brakePin = 2;
// Connect this to the button. The other side of the button should be ground.
int throttleButtonPin = 3; // Has 1.5k Ohm to 5v pullup resistor.

// The setup routine runs once when you press reset or the board starts.
void setup() {
  pinMode(throttlePin, OUTPUT);
  pinMode(brakePin, INPUT);
  pinMode(throttleButtonPin, INPUT);
}

// The loop routine runs over and over again forever.
void loop() {
  // I like to call loop functions on specific funtionality.
  throttleLoop();
}

// THROTTLE

int throttleOutput = 0; // off.
int throttleRamp = 10; // ms

// When does the controller start to respond to the throttle input voltage?
// Remember, analogWrite values go from 0 to 255.
byte throttleMin = 50; // around 1 volt.
byte throttleMax = 200; // around 4 volts.
// This stores the millisecond time of last throttle adjustment.
unsigned long throttleLastUp = 0;
// On the last throttle adjustment, was the throttle on or off.
boolean throttleWasOn = false;

void throttleLoop() {
  // When the brake is on turn the throttle off. End of story.
  if (digitalRead(brakePin == LOW)) {
    throttleOff();
  } else {
    // See if the throttle pin is activated.
    if (digitalRead(throttleButtonPin) == LOW) {
      throttleUp();
    }
    // Throttle pin off. Unless the ramp reached throttleMax turn the throttle off.
    else if (throttleOutput < throttleMax) {
      throttleOff();
    }
  }
}

void throttleUp() {
  // Only increase every so often.
  if (millis() - throttleLastUp >= throttleRamp) {
    // With a new button push reset throttle position to the minimum.
    if (!throttleWasOn) {
      throttleOutput = throttleMin; 
    }
    // On start begin where the controller will actually do something.
    else if (throttleOutput == 0) {
      throttleOutput = throttleMin;
    }
    // Do not go beyond max.
    if (throttleOutput != throttleMax) {
      // Increment one is an increase of +0.02v.
      throttleOutput++;
      analogWrite(throttlePin, throttleOutput);
      // Save that the throttle was on.
      throttleWasOn = true;
    }
    // Save time we updated throttle position.
    throttleLastUp = millis();
  }
}

// Turn the throttle off.
void throttleOff() {
  if (throttleWasOn) {
    // Save throttle output value.
    throttleOutput = 0;
    analogWrite(throttlePin, throttleOutput);
    // Save that the throttle was off.
    throttleWasOn = false;
  }
}

