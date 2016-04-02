// This is a simple button throttle sketch for a digispark.

// Connect this to the controller green wire.
// Also PCB LED. 5v max out. PWM ~1000 Hz.
// Attached to a 10k Ohm resistor then 2uf cap. Other side of cap to negative.
// digispark pin4 is also used by usb. Use pin 5 or 6 on the Uno.

// NOTE: using 1k Ohm resistor with 22uf on my controller. Seems to vary based
// on controller.
int throttlePin = 6;

// Connect this to the yellow wire coming out of the controller.
// This is pulled up by the controller. pin 2 on digispark. 11 on uno.
int brakePin = 11;
// Connect this to the button. The other side of the button should be ground.
// Has internal 1.5k Ohm to 5v pullup resistor.
int throttleButtonPin = 3;
// LED for debugging if the throttle is on
int throttleLED = 13;

// The setup routine runs once when you press reset or the board starts.
void setup() {
  Serial.begin(9600);
  pinMode(throttlePin, OUTPUT);
  pinMode(throttleLED, OUTPUT);
  pinMode(brakePin, INPUT_PULLUP);
  pinMode(throttleButtonPin, INPUT_PULLUP);
  Serial.println("SETUP");
}

// The loop routine runs over and over again forever.
void loop() {
  // I like to call loop functions on specific funtionality.
  throttleLoop();
}


// Store the last button state to debounce transitions
int lastButtonState = LOW;
// the last time the output pin was toggled
long lastDebounceTime = 0;
long debounceDelay = 50;

// THROTTLE
int throttleOutput = 0; // off.
int throttleRamp = 20; // ms

// When does the controller start to respond to the throttle input voltage?
// Remember, analogWrite values go from 0 to 255.
// Was having troubles with false positives, so introduce a slight buffer
// between the minimum throttle value and the minimum throttle action
byte throttleMin = 28; // where we start incrementing
byte throttleAction = 50; // around 1 volt. No action until we reach this value
byte throttleMax = 240; // around 4 volts.
// How much we add each increment. 1 increment = 0.02v.
byte throttleIncrement = 2;
// This stores the millisecond time of last throttle adjustment.
unsigned long throttleLastUp = 0;
// On the last throttle adjustment, was the throttle on or off.
boolean throttleWasOn = false;


void throttleLoop() {
  // When the brake is on turn the throttle off. End of story.
  if (digitalRead(brakePin)  == LOW) {
    throttleOff();
  } else {
    // See if the throttle pin is activated.
    int reading = digitalRead(throttleButtonPin);
    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
      // reset the debouncing timer
      lastDebounceTime = millis();
      lastButtonState = reading;
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (digitalRead(throttleButtonPin) == LOW) {
        throttleUp();
      }
      else {
        throttleWasOn = false;
        // Throttle pin off. Unless the ramp reached throttleMax turn the throttle off.
        if ((throttleOutput > throttleAction) && (throttleOutput < throttleMax)) {
          throttleOff();
        }
      }
    }
  }
  // Delay is for debug.
  // delay(30);
}

void throttleUp() {
  // Only increase every so often.
  if (millis() - throttleLastUp >= throttleRamp) {
    // With a new button push reset throttle position to the minimum.
    // On start begin where the controller will actually do something.
    if (!throttleWasOn || throttleOutput == 0) {
      throttleOutput = throttleMin;
      Serial.println("new throttle");
    }
    // Do not go beyond max.
    if (throttleOutput != throttleMax) {
      // Increment one is an increase of +0.02v.
      throttleOutput = throttleOutput + throttleIncrement;
      if (throttleOutput > throttleAction) {
        analogWrite(throttlePin, throttleOutput);
      }
      // Save that the throttle was on.
      throttleWasOn = true;
      Serial.println(throttleOutput);
    }
    // Save time we updated throttle position.
    throttleLastUp = millis();
    digitalWrite(throttleLED, HIGH);
  }
}

// Turn the throttle off.
void throttleOff() {
  if (throttleOutput > 0) {
    // Save throttle output value.
    throttleOutput = 0;
    analogWrite(throttlePin, throttleOutput);
    // Save that the throttle was off.
    throttleWasOn = false;
    Serial.println("BRAKE");
    digitalWrite(throttleLED, LOW);
  }
}
