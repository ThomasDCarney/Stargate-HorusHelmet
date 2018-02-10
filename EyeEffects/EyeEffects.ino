/* Horus will have two "moods" or modes, illustrated by the color of the eyes. 
 * This is handled by two LED's, white for normal running and red for angry or 
 * attack mode.  
 *
 * No other frills in this version, simplest worst case backup. 
 */

// Define which pins will be used.
const int BUTTON_PIN = 2;
const int RED_PIN = 7;
const int WHITE_PIN = 8;

// Used in the software debounce on a momentary switch, tweak as needed. 
const int DEBOUNCE_LENGTH = 250;
long oldTime;

// Keeps tab on the mood/mode.
int isAngry;

void setup() {

  pinMode(BUTTON_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(WHITE_PIN, OUTPUT);
  
  // Default mood/mode is normal running.
  digitalWrite(WHITE_PIN, HIGH);
  digitalWrite(RED_PIN, LOW);
  isAngry = false;

  oldTime = 0;

} // end setup


void loop() {

  if(wasButtonPressed()) {

    isAngry = !isAngry;
    
    if(isAngry) {
      
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(WHITE_PIN, LOW);
    
    } else {
      
      digitalWrite(RED_PIN, LOW);
      digitalWrite(WHITE_PIN, HIGH);
    
    }
    
  }
  
} // end loop


boolean wasButtonPressed() {

  long currentTime = millis();
  boolean buttonPressed = false;

  // Only register the initial button press, assume the bounces die off after
  // a certain period... DEBOUNCE_LENGTH.
  if(currentTime - oldTime > DEBOUNCE_LENGTH){
    
    if(digitalRead(BUTTON_PIN) == HIGH) {

      // Record the button was pressed and when.
      buttonPressed = true;
      oldTime = currentTime;
  
    }
  
  }
  
  return buttonPressed;
  
} // end wasButtonPressed

