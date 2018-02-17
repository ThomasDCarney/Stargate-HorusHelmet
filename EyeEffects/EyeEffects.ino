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
const int NUM_PIXELS = 2; // number of pixels in the strip.

// For software debounce (momentary switch). 
const int DEBOUNCE_LENGTH = 250; // Tweak for the specific switch used.
volatile long oldDebounceTime = 0; // Keep volatile (modified by ISR)

// Keeps track of mood/mode (angry or passive).
volatile boolean isAngry = false; // Keep volatile (modified by ISR).
volatile boolean moodChanged = false; // Keep volatile (modified by ISR).

// Illumination values for individual colors, 0 = off, 255 = max.
int red = 100;
int green = 100;
int blue = 100;

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

  for(int i = 0; i < NUM_PIXELS; i++) {

    strip.setPixelColor(i, red, green, blue);
    strip.show(); // no values specified yet so inits all pixels to off.
    
  }
  

} // end setup


/**
 * Main loop, all it does is tells a particular LED (depending on mood) to move along 
 * in it's animation. 
 */
void loop() {

  if(updateColorValues()) {

    for(int i = 0; i < NUM_PIXELS; i++) {

      strip.setPixelColor(i, red, green, blue);
      strip.show();
      
    }
    
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
  const int midRed = 150;
  const int colorRange = 80; // Will pulse this far above/below mid levels.
  const int overload = 255; // Overload will be max red;
  
  // Timing variables for each phase.
  const long phase2Period = 6;
  const long phase3Period = 10;
  const long phase4Period = 6; 
  const long phase5Period = 1;

  // Booleans to control phases of the animation.
  static boolean phase2 = false;
  static boolean phase3 = false;
  static boolean phase4 = false;
  static boolean dimming = true;

  // Return value, must be updated if values are changed.
  boolean valuesChanged = false;
  
  /* This outer IF-Else block work kind of like Keyframes or "phases".
   * Phase 1: Reset the eyes to desired shade.
   * Phase 2: Raise to the normal maximum brightness.
   * Phase 3: Overload the eyes a bit.
   * Phase 4: Dim to normal mid brightness.
   * Phase 5: Normal fast pulse.
   */
  if(moodChanged) {

    red = 25;
    green = 15;
    blue = 0;
    
    phase2 = true; // Trigger the next phase.
    moodChanged = false; // Acknowledge the ISR did it's thing.
    valuesChanged = true;
    
  } else if(phase2) {

    // Control animation speed without blocking.
    if(valuesChanged = hasEnoughTimePassed(phase2Period)) {
      
      red++;
      
    }

    // Once settled, move to standard animation.
    if(red == midRed + colorRange) {

      phase2 = false; // Trigger the next phase.
      phase3 = true;
      
    }

  } else if(phase3) {

    // Control animation speed without blocking.
    if(valuesChanged = hasEnoughTimePassed(phase3Period)) {

      red++;
      
    }

    if(red == overload) {

      phase3 = false; 
      phase4 = true; // Trigger the next phase.
      
    }

  } else if(phase4) {

    // Control animation speed without blocking.
    if(valuesChanged = hasEnoughTimePassed(phase4Period)) {

      red--;
      
    }

    if(red == midRed) {

      phase4 = false; 
      dimming = false;
      
    }

    
  } else {

    // Standard animation after initial flash occured.
    if(valuesChanged = hasEnoughTimePassed(phase5Period)) {
      
      if(dimming) {

        red--;
        
      } else {

        red++;
        
      }

      if(red >= midRed + colorRange || red <= midRed - colorRange) {

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

  // Middle of level variation when passive mode progresses
  const int midRed = 100;
  const int midGreen = 100;
  const int midBlue = 100;

  // Values will pulse above/below mid levels by this much.
  const int colorRange = 75; // 

  // Dramatic effect max value for this animation.
  const int overloadRed = 255; // Overload will be max bright;

  // Timing variables for each phase.
  const long phase2Period = 3;
  const long phase3Period = 10;
  const long phase4Period = 5; 
  const long phase5Period = 25;

  // Booleans to control phases of the animation.
  static boolean phase2 = false;
  static boolean phase3 = false;
  static boolean phase4 = false;
  static boolean dimming = true;

  // Return value, update if values actually change.
  boolean valuesChanged = false;
  
  /* This outer IF-Else block work kind of like Keyframes or "phases".
   * Phase 1: Reset the eyes to desired shade.
   * Phase 2: Eyes go to normal max.
   * Phase 3: Eyes overload, goa'uld style.
   * Phase 4: Eyes recover from overload, dimming to normal max.
   * Phase 5: Normal slow pulse until some change occures.
   */
  if(moodChanged) {

    red = 25;
    green = 25;
    blue = 25;

    moodChanged = false; // End phase 1, ISR did it's thing.
    phase2 = true; // Trigger the next phase.
    valuesChanged = true;
    
  } else if(phase2) {

    // Control animation speed without blocking.
    if(valuesChanged = hasEnoughTimePassed(phase2Period)) {
      
      incrementPassiveColors();
      
    }

    // Once settled, move to standard animation.
    if(red == midRed + colorRange) {

      phase2 = false; // This phase is over.
      phase3 = true; // Next phase begins.
      
    }

  } else if(phase3) {

    // Control animation speed without blocking.
    if(valuesChanged = hasEnoughTimePassed(phase3Period)) {

      incrementPassiveColors();
      
    }

    // Once the eyes have reached max brightness.
    if(red >= overloadRed) {

      phase3 = false; // This phase is over.
      phase4 = true; // Next phase begins.
      
    }

  } else if(phase4) {

    if(valuesChanged = hasEnoughTimePassed(phase4Period)) {

      decrementPassiveColors();
      
    }

    // Once the overload is totally over.
    if(red <= midRed + colorRange) {

      phase4 = false; // This phase is over.
      dimming = true;
      
    }
    
    
  } else {

    // Standard animation after initial flash occured.
    if(valuesChanged = hasEnoughTimePassed(phase5Period)) {
      
      if(dimming) {

        decrementPassiveColors();
        
      } else {

        incrementPassiveColors();
        
      }

      if(red >= midRed + colorRange || red <= midRed - colorRange) {

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

