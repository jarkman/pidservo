/*

pidservo

This sketch controls a servo with a PID loop

Wiring:
Analog pin 1 to the wiper of our control pot (as a source of a test signal)
+5v to one end of the servo pot track
0v to the other end of the servo pot track

Analog pin 0 to the wiper of the servo pot
+5v to one end of the servo pot track
0v to the other end of the servo pot track

Digital pin 9 to the signal input of the servo
0v to the ground wire of the servo
5v from a high-current supply to the servo power wire 

*/

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
 
int potpin = 0;  // analog pin used to connect the potentiometer
long val;    // variable to read the value from the analog pin 

long target = 90000; // in thousanths of a degree

// loop state
long previous_error = 0;
long integral = 0;

long dt = 10; // ms 

// PID constants
long Kp = 1000; //1000;
long Ki = 00;
long Kd = 20000; //20000;

long max_output = 90000;

void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  //Serial.begin(9600);
} 
 
void loop() 
{ 
  long derivative;
  long output; // nominal output angle in millidegrees, which will generally be about 90000

  val = analogRead(potpin);            // reads the value of the servo potentiometer (value between 0 and 1023) 
  // and convert to millidegree 
  long angle = map(val, 0, 1023.0, 0, 179000);   
   
  //Serial.println (angle/1000);
  
  // and read our command pot
  val = analogRead(1);
  // convert to millidegrees
  target = map(val, 0, 1023.0, 0, 180000);
  
  //Serial.println (target/1000);
 
  long error = angle - target;
  error = -error; // fix to make it go the right way - alternatively, could swap the feedback pot wires over
  
  
  
  integral = integral + error*dt;
  derivative = (error - previous_error)/dt;
  
    //Serial.println (derivative);

  // i term tends to be huge so gets a bigger divisor
  output = (Kp*error)/1000 + (Ki*integral)/100000 + (Kd*derivative)/1000;
  // output is (roughly) in the range -90,000 to 90,000, with 0 when here's no error
  
  previous_error = error;

// stop us driving into the endstops by cutting drive when near the ends of the available rotation
  if( angle < 1000 && output < 0 )
    output = 0;
    
  if( angle > 177000 && output > 0 )
    output = 0;

// constrain output signal to a rational range, so we don't generate irrationally long/short pulses
  if( output < - max_output )
    output = -max_output;
    
  if( output > max_output )
    output = max_output;
    
// Driving the servo in uS rather than degrees gets round a resolution problem with Arduino Servo.write    
// Output of 0 (for no error) maps to a pulse of 1500 uS
// That matches the fixed feedback resistors we've soldered into the servo, which make it think 
// it's always at the middle of its range

  int us = map( output, -max_output, max_output, 1000, 2000 );
  

  //  Serial.println (us);

  myservo.writeMicroseconds( us ); // 1500 us is about the neutral position
  
  delay(dt);                           // waits for the servo to get there 
} 
