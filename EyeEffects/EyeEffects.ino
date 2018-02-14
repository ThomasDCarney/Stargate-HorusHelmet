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
volatile int isAngry = false; // Keep volatile (modified by ISR).

// Illumination values for individual colors, 0 = off, 255 = max.
int red = 200;
int green = 25;
int blue = 0;

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
  strip.show(); // no values specified yet so inits all pixels to off.

} // end setup


/**
 * Main loop, all it does is tells a particular LED (depending on mood) to move along 
 * in it's animation. 
 */
void loop() {

  updateColorValues();
  strip.setPixelColor(0, red, green, blue);
  strip.show();
  delay(50);
  
} // end loop


/**
 * This method determines the animation used for pixels in "angry" mode.
 */
void updateColorValues() {

  if(isAngry) {

    red = 200;
    green = 25;
    blue = 0;
      
  } else {

    red = 100;
    green = 100;
    blue = 100;
    
  }
  
} // end updateColorValues


/**
 * Interrupt Service Request for switching between angry/calm mood/mode
 */
void moodButtonISR() {

    // Since we're using software debouncing, wasButtonPressed() will tell the ISR whether 
    // or not to perform its routine.
    if(wasButtonPressed()) {

      // The button is used to change modes.
      isAngry = !isAngry;
    
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

