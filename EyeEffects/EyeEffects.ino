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
int red = 50;
int green = 50;
int blue = 75;

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

    red = 200;
    green = 25;
    blue = 0;

    return true;
      
  } else {

    return updatePassiveColors();
    
  }
  
} // end updateColorValues


/**
 * This method is used to update colors while in the passive state.
 */
boolean updatePassiveColors() {

  const int midRed = 115;
  const int midGreen = 115;
  const int midBlue = 115;
  const int colorRange = 50;
  const int overload = 255 - midRed;
  const long passiveFlashingPeriod = 4;
  const long passiveCoolDownPeriod = 6;
  const long passiveStandardPeriod = 50;

  // Specific to this animation cycle but need to stick around.
  static long previousPassiveTime = 0;
  static boolean eyesOverloading = false;
  static boolean eyesCoolingDown = false;
  static boolean dimming = false;

  boolean valuesChanged = false;
  
  long currentTime = millis();

  /* This outer IF-Else block work kind of like Keyframes or "phases".
   * Phase 1: Reset the eyes to desired shade.
   * Phase 2: Overflash like Goa'uld eyes.
   * Phase 3: Settle back to normal high levels.
   * Phase 4: Standard slow pulse.
   */
  if(moodChanged) { // Phase 1 (based on ISR).

    red = midRed;
    green = midGreen;
    blue = midBlue;
    
    eyesOverloading = true; // Trigger the next phase.
    moodChanged = false; // Acknowledge the ISR did it's thing.
    valuesChanged = true;
    
  } else if(eyesOverloading) { // Phase 2.

    // Control animation speed without blocking.
    if(currentTime - previousPassiveTime >= passiveFlashingPeriod) {

      red++;
      green++;
      blue++;

      previousPassiveTime = currentTime;
      valuesChanged = true;
      
    }

    // Once settled, move to standard animation.
    if(red == midRed + overload) {

      eyesOverloading = false; // Trigger the next phase.
      eyesCoolingDown = true;
      
    }

  } else if(eyesCoolingDown) { // Phase 3.

    // Control animation speed without blocking.
    if(currentTime - previousPassiveTime >= passiveCoolDownPeriod) {

      red--;
      green--;
      blue--;

      previousPassiveTime = currentTime;
      valuesChanged = true;
      
    }

    // Once settled, move to standard animation.
    if(red == midRed) {

      eyesCoolingDown = false; // Trigger the next phase.
      dimming = false;
      
    }

  } else { // Phase 4.

    // Standard animation after initial flash occured.
    if(currentTime - previousPassiveTime >= passiveStandardPeriod) {
      
      if(dimming) {

        red--;
        green--;
        blue--;

        if(red <= midRed - colorRange) {

          dimming = false;
          
        }
        
      } else {

        red++;
        green++;
        blue++;

        if(red >= midRed + colorRange) {

          dimming = true;
          
        }
        
      }

      previousPassiveTime = currentTime;
      valuesChanged = true;
      
    }
    
  }

  return valuesChanged;
  
} // end animatePassive


/**
 * Interrupt Service Request for switching between angry/calm mood/mode
 */
void moodButtonISR() {

    // Since we're using software debouncing, wasButtonPressed() will tell the ISR whether 
    // or not to perform its routine.
    if(wasButtonPressed()) {

      // The button is used to change modes.
      isAngry = !isAngry;
      moodChanged = true;
    
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

