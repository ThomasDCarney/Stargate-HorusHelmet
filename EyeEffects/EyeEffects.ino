/* Horus will have two "moods" or modes, illustrated by the color of the eyes. 
 * This is handled by two LED's, white for normal running and red for angry or 
 * attack mode.  
 *
 * Changing moods/modes is handled via an ISR and each has a unique set of 
 * values to control their animations.
 * 
 * Debouncing is handled in software.
 */

// Define which pins will be used.
const int BUTTON_PIN = 2;
const int RED_PIN = 5;
const int WHITE_PIN = 6;

// Used in the software debounce on a momentary switch, tweak as needed. 
const int DEBOUNCE_LENGTH = 250;
volatile long oldDebounceTime = 0;

// Keeps tab on the mood/mode.
volatile int isAngry = false;

// Illumination levels for both LED's.
const int maxRedLevel = 175;
const int minRedLevel = 40;
int redPeriod = 15;
volatile int redLevel = maxRedLevel;

const int maxWhiteLevel = 175;
const int minWhiteLevel = 40;
volatile int whiteLevel = maxWhiteLevel;
int whitePeriod = 15;

// Used by animation sequences.
boolean dimming = true;
long oldRedTime = 0;    // Not sure we need unique times for red and white
long oldWhiteTime = 0;  // but doesn't hurt so keeping it.


/**
 * Initial one-time setup.
 */
void setup() {

  // Init the pins
  pinMode(BUTTON_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(WHITE_PIN, OUTPUT);
  
  // Default mood/mode is normal running.
  analogWrite(WHITE_PIN, maxWhiteLevel);
  analogWrite(RED_PIN, 0); // Is off to start.
  attachInterrupt(digitalPinToInterrupt(2), moodButtonISR, RISING);

} // end setup


/**
 * Main loop, all it does is tells a particular LED (depending on mood) to move along 
 * in it's animation. 
 */
void loop() {

  if(isAngry) {

    changeRedLevel();
    
  } else {

    changeWhiteLevel();
    
  }
  
} // end loop


/**
 * This method determines the animation used for the red, "angry" LED.
 */
void changeRedLevel() {

  long currentTime = millis();

  // Change redPeriod to make the pulse faster/slower.
  if(currentTime - oldRedTime > redPeriod) {

    // This animation is a simple pulse that dims and brightens.
    if(dimming) {

      analogWrite(RED_PIN, --redLevel);
    
    }  else {

      analogWrite(RED_PIN, ++redLevel);
      
    }

    if(redLevel >= maxRedLevel || redLevel <= minRedLevel) {

      dimming = !dimming;
      
    }

    oldRedTime = currentTime;
  
  }
  
} // end changeRedLevel


/**
 * This method determines the animation used for the white, "calm" LED.
 */
void changeWhiteLevel() {

  long currentTime = millis();

  // Change whitePeriod to make the pulse faster/slower.
  if(currentTime - oldWhiteTime > whitePeriod) {

    // This animation is a simple pulse that dims and brightens.
    if(dimming) {

      analogWrite(WHITE_PIN, --whiteLevel);
    
    }  else {

      analogWrite(WHITE_PIN, ++whiteLevel);
      
    }

    if(whiteLevel >= maxWhiteLevel || whiteLevel <= minWhiteLevel) {

      dimming = !dimming;
      
    }

    oldWhiteTime = currentTime;
  
  }
  
} // end changeWhiteLevels


/**
 * Interrupt Service Request for switching between angry/calm mood/mode
 */
void moodButtonISR() {

    // Since we're using software debouncing, wasButtonPressed() will tell the ISR whether 
    // or not to perform its routine.
    if(wasButtonPressed()) {

      // The button is used to change modes.
      isAngry = !isAngry;

      // Init starting values for each state (starting on the high end).
      if(isAngry) {

        redLevel = maxRedLevel;
        analogWrite(RED_PIN, redLevel);
        analogWrite(WHITE_PIN, 0);
    
      } else {

        whiteLevel = maxWhiteLevel;
        analogWrite(RED_PIN, 0);
        analogWrite(WHITE_PIN, whiteLevel);
    
      }

      // Since we're starting high, we should be dimming.
      dimming = true;
    
  }
  
} // end moodButtonISR


/**
 * A debounce routine used for the mood switch.
 */
boolean wasButtonPressed() {

  long currentTime = millis();
  boolean buttonPressed = false;

  // Only register the initial button press, assume the bounces die off after
  // a certain period... tweak DEBOUNCE_LENGTH if needed.
  if(currentTime - oldDebounceTime > DEBOUNCE_LENGTH){
    
    if(digitalRead(BUTTON_PIN) == HIGH) {

      // Record the button was pressed and when.
      buttonPressed = true;
      oldDebounceTime = currentTime;
  
    }
  
  }
  
  return buttonPressed;
  
} // end wasButtonPressed

