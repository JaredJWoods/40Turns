#include "Mouse.h"

/*defines constant variables*/

#define CLK 2     //encoder outputA
#define DT 3      //encoder outputB
#define SW 7      //encoder switch
#define LEFT 9    //left horizontal toggle switch
#define RIGHT 8   //left horizontal toggle switch
#define LED 4     //event status LED
#define LEDSW 5   //latch status LED

//Variable for status report and debounce
int counter = 0;

bool latch;
bool status;
int currentStateCLK;
int lastStateCLK;
unsigned long lastButtonPress = 0;
int range = 10;




void setup() {
	/* Set designate encoder pins */
	pinMode(CLK, INPUT);          //rotary outputA
	pinMode(DT, INPUT);           //rotary outputB
	pinMode(SW, INPUT_PULLUP);    //rotary switch
  pinMode(LED, OUTPUT);         //mouse input status
  pinMode(LEDSW, OUTPUT);       //latch/left mouse click status
  pinMode(LEFT, INPUT_PULLUP);  //horizontal toggle switch left
  pinMode(RIGHT, INPUT_PULLUP); //horizontal toggle switch right

	// Setup Serial Monitor for display received Data; 9600 is the baud rate (think wordclock)
	Serial.begin(9600);           

  // Hands over mouse control can be dangerous if there are errors in the program
  Mouse.begin();

	// Read the initial state of CLK !!REDUNDANT!!
	lastStateCLK = digitalRead(CLK);

  // Sets a "string" variable for test staus read outs
  String currentDir ="";
  
  //initializes the "latch/left mouse click" as OFF
  latch = LOW;
}

void loop() {
  /*this is meant to create a "fine/coarse" rotational toggle when the encoder is held while turning...no luck
	bool btnState = digitalRead(SW);  
  if (btnState == LOW){
    int range = 2;                  
  }else {
    range = 10;
  }
  */

	// Read the current state of CLK this helps determine any rotary changes and in which direction
	currentStateCLK = digitalRead(CLK);

	/*If last and current state of CLK are different, then an expected pulse has occurred.  
  Also it shall react to only 1 state change to avoid double count.
  the next line looks for a change in the encoder's outputs to evaluate movement.
  If the DT state is different than the CLK state then
	the encoder is rotating CCW so decrement if it is the opossite we are moving in the other direction.
  Not exactly certain how this works it is from the arduino site*/
	
  
  //--------------------ROTARY ENCODER VERTICAL MOVEMENT--------------------
  
  //this says...what exactly?
  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
    digitalWrite(LED, HIGH);
		if (digitalRead(DT) != currentStateCLK) {
			counter --;
			currentDir ="CCW";
      Mouse.move(0,-range);
      
		} else {
			counter ++;
			currentDir ="CW";
      Mouse.move(0,range);
		}

    /*Displays the status of any rotary enocoder actions*/
		Serial.print("Direction: ");
		Serial.print(currentDir);
		Serial.print(" | Counter: ");
		Serial.println(counter);
    Serial.print("range");
    Serial.println(range);
      
	}
  
  //resets the rotary encoder's state
	lastStateCLK = currentStateCLK;
  //makes sure the "input LED" is off to begin with. (might be redundant)
  digitalWrite(LED, LOW);

	

  //----------------LATCH/LEFT MOUSE CLICK COMMANDS-------------------------

	// Checks the latch state, which order MOUSE_LEFT commands
  if (latch == HIGH) {
    digitalWrite(LEDSW, HIGH);
    Mouse.press(MOUSE_LEFT);
  }else{
    digitalWrite(LEDSW, LOW);
    Mouse.release(MOUSE_LEFT);
  } 
  //If we detect LOW signal, button is pressed
	if (btnState == LOW) {
    digitalWrite(LED, HIGH);
		//if 50ms have passed since last LOW pulse, it means that the
		//button has been pressed, released and pressed again
		if (millis() - lastButtonPress > 50) {
			Serial.println("Button pressed!");
      bool status = HIGH;
      latch = !latch;
		} else{
      bool status = LOW;
    }
    

		digitalWrite(LED, LOW);
    // Remember when the last button pressed event occured
		lastButtonPress = millis();
	}

	//A slight delay to help debounce the reading (DOES IT REALLY?)
	delay(1);

//-----------------------------HORIZONTAL TOGGLE SWITCH-------------------
  
  if (digitalRead(LEFT) == LOW){
    Mouse.move(-1,0);
    Serial.println("LEFT");
    digitalWrite(LED, HIGH);
    //delay(5);
    latch = LOW;
    }else{
    digitalWrite(LED, LOW);
  }
  if (digitalRead(RIGHT) == LOW){
    Mouse.move(1,0);
    Serial.println("RIGHT");
    digitalWrite(LED, HIGH);
    //delay(5);
    latch = LOW;
  }else{
    digitalWrite(LED, LOW);
  }
}
