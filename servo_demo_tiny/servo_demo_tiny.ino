#include <SoftwareServo.h> 

// This is a demo for servo operation on ATTiny45

// Using the Tiny-compatible SoftwareServo from
// https://docs.google.com/file/d/0B6s_UhmkBLQfVTVQWEh6aS1yb0E/edit
// NB - the ATTiny clock speed must be set to 8MHz


SoftwareServo myservo;  // create servo object to control a servo 

int potpin = 2;  // ATTiny45 chip pin 3 (which is Arduino analog input 2) analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin 

int outputpin = 1; // ATTiny45 chip pin 6 (PB1) to the signal input of the servo

void setup() 
{ 
  myservo.attach(outputpin);   
} 

void loop() 
{ 
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023) 
  val = map(val, 0, 1023, 0, 179);     // scale it to use it with the servo (value between 0 and 180) 
  myservo.write(val);                  // sets the servo position according to the scaled value 
  delay(15);                           // waits for the servo to get there 

  SoftwareServo::refresh();
} 


