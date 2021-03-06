//LIBRARIES-----------------------------------------------------------------------------------------------------------------------------------------------------------

//Sensor:
#include <SoftwareSerial.h> //Library for Sensor.
#include <LiquidCrystal_I2C.h> //Library for LCD.

//Intializations------------------------------------------------------------------------------------------------------------------------------------------------------

//LCD:
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display.
//SDA (Yellow Wire) is Pin A4.
//SCL (Purple Wire) is Pin A5.

//Sensor:
SoftwareSerial Serial1(2,3); //Defines the pins used for the LIDAR sensor.
//SCL(or TXD) (Green Wire) is Pin 2.
//SDA(or RXD) (White Wire) is Pin 3.

int dist; // Initializes "dist" variable as integer.
int check; // Initializes "check" variable as integer.
int uart[9]; //Initializes "uart" protocol for sensor.
int j; //Initializes counter variable "j" for sensor as an integer.
float setPoint = 1; //Initializes "setPoint" value as a float.
float realPoint = 0; //Initializes "realPoint" value as a float.
const int HEADER = 0x59; //Initializes "HEADER" variable for sensor as an integer.

//Stepper Motor:
int i = 0; //Initializes counter variable "i" for stepper motor as an integer.

//Buttons:
const float  Up_buttonPin   = 6; // Initializes button on the right as the "UP" button.
const float  Down_buttonPin = 7; // Initializes button on the right as the "UP" button.
float buttonPushCounter = 0; //Counter for the number of button presses.
float up_buttonState = 0; //Current state of the up button.
float up_lastButtonState = 0; //Previous state of the up button.
float down_buttonState = 0; //Current state of the up button.
float down_lastButtonState = 0; //Previous state of the up button.
bool bPress = false; //Initializes boolean statement determining the state of the buttons.


//SETUP--------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  
 //LCD and Printing:
 Serial.begin(115200); // Initialize serial communication with computer at 115200 baud:
 Serial1.begin(115200); // Initialize serial communication with computer at 115200 baud:  
 lcd.init();  // Initialize the LCD.
 lcd.backlight();  // Initialize the backlight.

 //Buttons:
 pinMode( Up_buttonPin , INPUT_PULLUP); 
 pinMode( Down_buttonPin , INPUT_PULLUP);
}

//UP BUTTON-----------------------------------------------------------------------------------------------------------------------------------------------------------

void checkUp()
{
  up_buttonState = digitalRead(Up_buttonPin); 
  if (up_buttonState != up_lastButtonState) { //Compare the buttonState to its previous state.
    if (up_buttonState == LOW) { // If the state has changed, increment the counter.
        bPress = true;
      //If the current state is HIGH then the button went from off to on:
     buttonPushCounter = buttonPushCounter + 0.2; //Increase the number of the counter by 0.2 each button press.
    } else { 
      //If the current state is LOW then the button went from on to off:
    }
   delay(10); //Delay a little bit to avoid bouncing.
  }
  up_lastButtonState = up_buttonState; //Save the current state as the last state, for next time through the loop.
}

//DOWN BUTTON-----------------------------------------------------------------------------------------------------------------------------------------------------------

void checkDown()
{
  down_buttonState = digitalRead(Down_buttonPin); 
  if (down_buttonState != down_lastButtonState) { //Compare the buttonState to its previous state.
    if (down_buttonState == LOW) { //If the state has changed, increment the counter.
        bPress = true;
      //If the current state is HIGH then the button went from off to on:
      buttonPushCounter = buttonPushCounter - 0.2; //Increase the number of the counter by 0.2 each button press.
    } else {
      //If the current state is LOW then the button went from on to off:
    }
    delay(10); // Delay a little bit to avoid bouncing.
  }
  down_lastButtonState = down_buttonState; //Save the current state as the last state, for next time through the loop.
}

//LOOP-------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {

 //Buttons:
 checkUp(); //Implements up button code into the loop.
 checkDown(); //Implements down button code into the loop.

 if( bPress){ // If stament for when either button is pressed.
   bPress = false;
   setPoint = 1; //Includes the intial setPoint value in the loop so the push buttons work properly. 
   setPoint = setPoint + buttonPushCounter; //Adds or subtracts 0.2 from Set Point value based on which button is pressed.
   }
   
 //Sensor:
  if (Serial1.available())
   {
     if(Serial1.read()==HEADER)
     {
        uart[0]=HEADER;
        if(Serial1.read()==HEADER)
        {
         uart[1]=HEADER;
        }
        for(j=2;j<9;j++)
        {
         uart[j]=Serial1.read();
        }
        check=uart[0]+uart[1]+uart[2]+uart[3]+uart[4]+uart[5]+uart[6]+uart[7];
       
        if (uart[8]==(check&0xff)) 
        {
         dist=uart[2]+uart[3]*256;
         delayMicroseconds(25);
        }
     }
   }
 //LCD and Printing:
 Serial.print("Set MAF="); //Displays Set Point MAF in kg/s.
 Serial.print(setPoint);
 Serial.print('\t');
 Serial.print("Real MAF="); //Displays Real Point MAF in kg/s
 Serial.print(-0.033*(dist)+13.2);
 Serial.print('\t');
 Serial.print("Error="); //Displays Error between Set Point and Real Point
 Serial.print(setPoint-realPoint);
 Serial.print('\t');
 Serial.print("Distance");
 Serial.print('\t');
 Serial.print(dist);
 Serial.print('\n'); 
 delay(25); //Delay to slow down serial monitor.
        
 lcd.setCursor(1, 0); // Sets the cursor at the beginning of the first line on display.
 lcd.print("Set MAF: ");  // Print a message to the LCD.
 lcd.print(setPoint);  // Print a message to the LCD.
 lcd.setCursor(0, 1); // Sets the cursor at the beginning of the second line on display.
 lcd.print("Real MAF: ");  // Print a message to the LCD.
 lcd.print(realPoint, 1);  // Print a message to the LCD.


 //Stepper Motor:
 realPoint = -0.033*(dist)+13.2; /*Equation to convert sensor data (cm) to mass air flow
 rate data (kg/s) and sets it as the real point for calculation.*/

 /* The following if and for statements are used to control the motor based on the set point. The statements drive the motor towards the set point by trying 
 to get the error between the set point and real point lower than +/-0.2 kg/s.
 */ 
 
 if (setPoint-realPoint >= 0.2){
   digitalWrite(4, HIGH);
     for (i = 0; i <= 2; i++)
      {
      digitalWrite(5,HIGH);
      digitalWrite(5,LOW);
      delayMicroseconds(60);
      }
 }
  else if (setPoint-realPoint <= -0.2){
   digitalWrite(4, LOW);
     for (i = 0; i <= 2; i++)
      {
      digitalWrite(5,HIGH);
      digitalWrite(5,LOW);
      delayMicroseconds(60);
      }  
  }
}
