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

// Keeps track of mood/mode (angry or passive).
boolean isAngry = false;
boolean resetMood = false;
volatile boolean moodChanged = false; // Keep volatile (modified by ISR).

// Illumination values for individual colors, 0 = off, 255 = max.
int red = 100;
int green = 100;
int blue = 100;

/* The NeoPixel Strip object is used to represent/control the pixels.
 * Parameter 1 = Number of pixels in the strip.
 * Parameter 2 = Pin used for data input.
 * Parameter 3 = Pixel type flags... see documentation!
 */
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, DATA_PIN, NEO_GRB);


/**
 * Initial setup.
 */
void setup() {

  // Initialize buttons.
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), moodButtonISR, FALLING);

  // Init the NeoPixel strip.
  strip.begin();
  for(int i = 0; i < NUM_PIXELS; i++) {

    strip.setPixelColor(i, red, green, blue);
    strip.show();

  }

} // end setup


/**
 * Main loop.
 */
void loop() {

  // Acknowledge the mood button ISR (button was pressed).
  if(moodChanged) {

      resetMood = true;
      isAngry = !isAngry;
      moodChanged = false; // Reset button notification!

  }

  /* We are asking the program to update color values which returns a boolean
   * true if values were changed (false otherwise). If no changes occured then
   * we won't waste time pushing the same values to our pixels.
   */
  if(updateColorValues()) {

    for(int i = 0; i < NUM_PIXELS; i++) {

      strip.setPixelColor(i, red, green, blue);
      strip.show();

    }

  }

} // end loop


/**
 * This method handles updating each pixels color values based on the current
 * mood/mode (angry or passive).
 *
 * @return - A boolean true if colors were updated/changed, false otherwise.
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

  // Constants specific to the angry state.
  const int midRed = 100;
  const int overload = 100; // Will pulse this far above mid levels.

  // Timing variables for each phase.
  const long phase2Period = 10;
  const long phase3Period = 7;

  // Booleans to control phases of the animation.
  static boolean phase2 = false;
  static boolean phase3 = false;

  // Return value, must be updated if values are changed.
  boolean valuesChanged = false;

  /* This outer IF-Else block work kind of like Keyframes or "phases".
   * Phase 1: Reset the eyes to desired shade.
   * Phase 2: Raise to the normal maximum brightness.
   * Phase 3: Overload the eyes a bit.
   * Phase 4: Dim to normal mid brightness.
   * Phase 5: Normal fast pulse.
   */
  if(resetMood) {

    red = 25;
    green = 0;
    blue = 0;

    phase2 = true; // Trigger the next phase.
    resetMood = false;
    valuesChanged = true;

  } else if(phase2) {

    // Control animation speed without blocking.
    if(valuesChanged = hasEnoughTimePassed(phase2Period)) {

      red++;

    }

    // Once settled, move to standard animation.
    if(red == midRed + overload) {

      phase2 = false; // Trigger the next phase.
      phase3 = true;

    }

  } else if(phase3) {

    // Control animation speed without blocking.
    if(valuesChanged = hasEnoughTimePassed(phase3Period)) {

      red--;

    }

    if(red == midRed) {

      phase3 = false;

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
  if(resetMood) {

    red = 25;
    green = 25;
    blue = 25;

    resetMood = false;
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
 * This method is used to time the animations where we only want changes to
 * occure if the specified time has elapsed since the last change.
 *
 * @return - A boolean true if the provided period has expired, false otherwise.
 */
boolean hasEnoughTimePassed(long period) {

  static long previousTime = 0;
  long currentTime = millis();

  if(currentTime - previousTime >= period) {

    previousTime = currentTime;
    return true;

  }

  return false;

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
 * Interrupt Service Request for switching between angry/calm mood/mode. This
 * ISR will be called whenever the pins state goes LOW.
 */
void moodButtonISR() {

      // The button is used to trigger a mood/mode change.
      moodChanged = true; // Flag for outside methods to do something.

} // end moodButtonISR
