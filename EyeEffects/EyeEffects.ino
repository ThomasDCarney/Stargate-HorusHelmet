/* Horus will have two "moods" or modes, illustrated by the color of the eyes. 
 * This is handled by two LED's, white for normal running and red for angry or 
 * attack mode.  
 *
 * Changing moods/modes is handled via an ISR and each has a unique set of 
 * values to control their animations.
 * 
 * Debouncing is handled in software.
 */

// Using Adafruit NeoPixels, Libraries provided by AdaFruit.
#include <Adafruit_NeoPixel.h>

// Define which pins are used.
const int BUTTON_PIN = 2;
const int DATA_PIN = 6; // Sends instructions to "Data In" on strip.
const int NUM_PIXELS = 1; // number of pixels in the strip.

// For software debounce (momentary switch). 
const int DEBOUNCE_LENGTH = 250; // Tweak for the specific switch used.
volatile long oldDebounceTime = 0; // Keep volatile (modified by ISR)

// Keeps track of mood/mode (angry or passive).
volatile boolean isAngry = false; // Keep volatile (modified by ISR).
volatile boolean moodChanged = false; // Keep volatile (modified by ISR).

// Illumination values for individual colors, 0 = off, 255 = max.
int red = 115;
int green = 115;
int blue = 115;

// Timing variables for eye animations.
long lastAngryTime = 0;
int angrySettlingPeriod = 50;
int angryStandardPeriod = 100;

/* The NeoPixel Strip object is used to represent/control the pixels. 
 * Parameter 1 = Number of pixels in the strip.
 * Parameter 2 = Pin used for data input.
 * Parameter 3 = Pixel type flags... see documentation!
 */
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, DATA_PIN, NEO_GRB);


/**
 * Initial one-time setup.
 */
void setup() {

  // Init the button.
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), moodButtonISR, RISING);

  // Init the pixel strip.
  strip.begin();
  strip.setPixelColor(0, red, green, blue);
  strip.show(); // no values specified yet so inits all pixels to off.

} // end setup


/**
 * Main loop, all it does is tells a particular LED (depending on mood) to move along 
 * in it's animation. 
 */
void loop() {

  if(updateColorValues()) {

    strip.setPixelColor(0, red, green, blue);
    strip.show();
  
  }
  
  
} // end loop


/**
 * This method will handle updating pixel colors.
 * 
 * @return - A boolean true if colors were updated, false otherwise.
 */
boolean updateColorValues() {

  if(isAngry) {

    return updateAngryColors();
      
  } else {

    return updatePassiveColors();
    
  }
  
} // end updateColorValues


/**
 * This method is used to update colors while in the angry state.
 */
boolean updateAngryColors() {

  // Constants specific to the passive state.
  const int midRed = 225;
  const int midGreen = 20;
  const int midBlue = 0;
  const int colorRange = 25; // Will pulse this far above/below mid levels.
  const int overload = 255 - midRed; // Overload will be max red;
  const long angryFlashingPeriod = 5; // Dictates speed of the flash.
  const long angryCoolDownPeriod = 4; // Dictates speed of the cool down.
  const long angryStandardPeriod = 10; // Dictates normal pulse speed.

  // Triggers specific to this animation cycle, need to stick around.
  static boolean eyesOverloading = false;
  static boolean eyesCoolingDown = false;
  static boolean dimming = false;

  // Return value, must be updated if values are changed.
  boolean valuesChanged = false;
  
  /* This outer IF-Else block work kind of like Keyframes or "phases".
   * Phase 1: Reset the eyes to desired shade.
   * Phase 2: Overflash like Goa'uld eyes.
   * Phase 3: Settle back to normal high levels.
   * Phase 4: Standard creepy faster pulse.
   */
  if(moodChanged) { // Phase 1.

    red = midRed;
    green = midGreen;
    blue = midBlue;
    
    eyesOverloading = true; // Trigger the next phase.
    moodChanged = false; // Acknowledge the ISR did it's thing.
    valuesChanged = true;
    
  } else if(eyesOverloading) { // Phase 2.

    // Control animation speed without blocking.
    if(valuesChanged = hasEnoughTimePassed(angryFlashingPeriod)) {
      
      red++;
      
    }

    // Once settled, move to standard animation.
    if(red == midRed + overload) {

      eyesOverloading = false; // Trigger the next phase.
      eyesCoolingDown = true;
      
    }

  } else if(eyesCoolingDown) { // Phase 3.

    // Control animation speed without blocking.
    if(valuesChanged = hasEnoughTimePassed(angryCoolDownPeriod)) {

      red--;
      
    }

    // Once settled, move to standard animation.
    if(red == midRed) {

      eyesCoolingDown = false; // Trigger the next phase.
      dimming = false; // false to pulse up first.
      
    }

  } else { // Phase 4.

    // Standard animation after initial flash occured.
    if(valuesChanged = hasEnoughTimePassed(angryStandardPeriod)) {
      
      if(dimming) {

        red--;
        
      } else {

        red++;
        
      }

      if(red == midRed + colorRange || red == midRed - colorRange) {

          dimming = !dimming;
          
      }
      
    }
    
  }

  return valuesChanged;
  
} // end updateAngryColors


/**
 * This method is used to update colors while in the passive state.
 */
boolean updatePassiveColors() {

  // Constants specific to the passive state.
  const int resetRed = 25;
  const int resetGreen = 25;
  const int resetBlue = 25;
  const int midRed = 135;
  const int midGreen = 135;
  const int midBlue = 135;
  const int colorRange = 50; // Will pulse this far above/below mid levels.
  const int overload = 255 - midRed; // Overload will be max bright;
  const long passiveFlashingPeriod = 5; // Dictates speed of the flash.
  const long passiveCoolDownPeriod = 4; // Dictates speed of the cool down.
  const long passiveStandardPeriod = 50; // Dictates normal pulse speed.

  // Triggers specific to this animation cycle, need to stick around.
  static boolean eyesOverloading = false;
  static boolean eyesCoolingDown = false;
  static boolean dimming = false;

  // Return value, must be updated if values are changed.
  boolean valuesChanged = false;
  
  /* This outer IF-Else block work kind of like Keyframes or "phases".
   * Phase 1: Reset the eyes to desired shade.
   * Phase 2: Overflash like Goa'uld eyes.
   * Phase 3: Settle back to normal high levels.
   * Phase 4: Standard slow pulse.
   */
  if(moodChanged) { // Phase 1.

    red = resetRed;
    green = resetGreen;
    blue = resetBlue;
    
    eyesOverloading = true; // Trigger the next phase.
    moodChanged = false; // Acknowledge the ISR did it's thing.
    valuesChanged = true;
    
  } else if(eyesOverloading) { // Phase 2.

    // Control animation speed without blocking.
    if(valuesChanged = hasEnoughTimePassed(passiveFlashingPeriod)) {
      
      incrementPassiveColors();
      
    }

    // Once settled, move to standard animation.
    if(red == midRed + overload) {

      eyesOverloading = false; // Trigger the next phase.
      eyesCoolingDown = true;
      
    }

  } else if(eyesCoolingDown) { // Phase 3.

    // Control animation speed without blocking.
    if(valuesChanged = hasEnoughTimePassed(passiveCoolDownPeriod)) {

      decrementPassiveColors();
      
    }

    // Once settled, move to standard animation.
    if(red == midRed) {

      eyesCoolingDown = false; // Trigger the next phase.
      dimming = false; // false to pulse up first.
      
    }

  } else { // Phase 4.

    // Standard animation after initial flash occured.
    if(valuesChanged = hasEnoughTimePassed(passiveStandardPeriod)) {
      
      if(dimming) {

        decrementPassiveColors();
        
      } else {

        incrementPassiveColors();
        
      }

      if(red == midRed + colorRange || red == midRed - colorRange) {

          dimming = !dimming;
          
      }
      
    }
    
  }

  return valuesChanged;
  
} // end updatePassiveColors


/**
 * This method is used to time the eye animations in which we don't want changes to 
 * occure as fast as possible but only after certain intervals.
 * 
 * @return - A boolean true if the provided period has been met since the last update, 
 * false otherwise.
 */
boolean hasEnoughTimePassed(long period) {

  static long previousTime = 0;
  long currentTime = millis();

  if(currentTime - previousTime >= period) {

    previousTime = currentTime;
    return true;
    
  } else {

    return false;
    
  }
  
} // end hasEnoughTimePassed


/**
 * This method is used to increment the colors while in the passive state.
 */
void incrementPassiveColors() {

  red++;
  green++;
  blue++;
  
} // end incrementPassiveColors


/**
 * This method is used to decrement the colors while in the passive state.
 */
void decrementPassiveColors() {

  red--;
  green--;
  blue--;
  
} // end decrementPassiveColors


/**
 * Interrupt Service Request for switching between angry/calm mood/mode
 */
void moodButtonISR() {

    // Since we're using software debouncing, wasButtonPressed() will tell the ISR whether 
    // or not to perform its routine.
    if(wasButtonPressed()) {

      // The button is used to change modes.
      isAngry = !isAngry; // Toggles the mode.
      moodChanged = true; // Flag for outside methods to do something.
    
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

