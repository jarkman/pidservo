
/*

pidservo_i2c_slave
===================
This sketch receives angle instructions over i2c from pidservo_i2c_master, and controls the servo with a PID loop

Wiring fro a regular Arduino:
SCL (analog pin 5) to SCL (analog pin 5) on the master Arduino
SDA (analog pin 4) to SDA (analog pin 4) on the master Arduino
0v to 0v on the master Arduino 

Analog pin 0 to the wiper of the servo pot
+5v to one end of the servo pot track
0v to the other end of the servo pot track

Digital pin 9 to the signal input of the servo
0v to the ground wire of the servo
5v from a high-current supply to the servo power wire 


Wiring for an ATTiny45:
=======================
SCL (chip pin 7) to SCL (analog pin 5) on the master Arduino
SDA (chip pin 5) to SDA (analog pin 4) on the master Arduino
0v to 0v on the master Arduino 

Chip pin 3 (ADC2) to the wiper of the servo pot
+5v to one end of the servo pot track
0v to the other end of the servo pot track

Chip pin 6 (PB1) to the signal input of the servo
0v to the ground wire of the servo
5v from a high-current supply to the servo power wire 

*/

/*
#ifndef A4 
#define ATTINY45 // else must be on a real Arduino, which has an A4
#endif


#ifdef ATTINY45 

#include <zzServo8Bit.h> 
#include <zzTinyWireS.h> 

Servo8Bit myservo; 
#define WIRE TinyWireS
#define READ receive
#define WRITE send

#else
*/

#include <Servo.h> 
#include <Wire.h> 

Servo myservo; 
/*
#define WIRE Wire
#define READ read
#define WRITE write


#endif
*/

#include "i2c_commands.h"


  
 
#ifndef ATTINY45 
// must be on a real Arduino,
int servo_pot_pin = 0;  // analog pin used to connect the potentiometer
int servo_output_pin = 9; // drive signal to the servo
#define DO_LOGGING

#else
// ATTiny45 doesn't have A4
int servo_pot_pin = A2;  // chip pin 3 - analog pin used to connect the potentiometer
int servo_output_pin = 1; // chip pin 6, PB1 drive signal to the servo
#endif


long current_angle = 0; // most recent angle from servo pot

long target = 90000; // in thousanths of a degree

// loop state
long previous_error = 0;
long integral = 0;

long dt = 10; // ms 

// PID constants
long Kp = 1000; //1000;
long Ki = 0;
long Kd = 20000;

long max_output = 90000;


char last_command = '\0';

void setup() 
{ 
  myservo.attach(servo_output_pin);  // attaches the servo on pin 9 to the servo object 
  
#ifdef DO_LOGGING
  Serial.begin(9600);
#endif
  
#ifdef DO_LOGGING
  Serial.print ("Wire.begin\n");
#endif
 
   Wire.onRequest(i2cRequestEvent); // register a function which will be called when the master asks for data
  Wire.onReceive(i2CReceiveEvent); // register a function which will be called when the master send a command (which it will do before askigng for data)

 Wire.begin(SERVO_I2C_ADDRESS); // start i2c as slave
  // Arduino - uses analog input 5 for SCL, analog input 6 for SDA
  // ATTint45 - uses chip pin 7 for SCL, chip pin 5 for SDA


} 
 
void loop() 
{ 
  long derivative;
  long output; // nominal output angle in millidegrees, which will generally be about 90000

  long val = analogRead(servo_pot_pin);            // reads the value of the servo potentiometer (value between 0 and 1023) 
  // and convert to millidegree 
  current_angle = map(val, 0, 1023.0, 0, 179000);   
   
  
  

#ifdef DO_LOGGING
  //Serial.println (current_angle/1000);
  //Serial.println (target/1000);
#endif
 
  long error = current_angle - target;
  error = -error; // fix to make it go the right way - alternatively, could swap the feedback pot wires over
  
  
  
  integral = integral + error*dt;
  derivative = (error - previous_error)/dt;
  
    //Serial.println (derivative);

  // i term tends to be huge so gets a bigger divisor
  output = (Kp*error)/1000 + (Ki*integral)/100000 + (Kd*derivative)/1000;
  // output is (roughly) in the range -90,000 to 90,000, with 0 when here's no error
  
  previous_error = error;

// stop us driving into the endstops by cutting drive when near the ends of the available rotation
  if( current_angle < 1000 && output < 0 )
    output = 0;
    
  if( current_angle > 177000 && output > 0 )
    output = 0;

// constrain output signal to a rational range, so we don't generate irrationally long/short pulses
  if( output < - max_output )
    output = -max_output;
    
  if( output > max_output )
    output = max_output;
    
    
#ifdef ATTINY45 
    
    myservo.write( output / 1000 + 90 );
    
#else
// Driving the servo in uS rather than degrees gets round a resolution problem with Arduino Servo.write    
// Output of 0 (for no error) maps to a pulse of 1500 uS
// That matches the fixed feedback resistors we've soldered into the servo, which make it think 
// it's always at the middle of its range

  int us = map( output, -max_output, max_output, 1000, 2000 );

  myservo.writeMicroseconds( us ); // 1500 us is about the neutral position
  
#endif
  
  delay(dt);                           // waits for the servo to get there 
} 


void i2cRequestEvent() // called when master asks for bytes
{
  // NB - the request event handler is only allowed one call of write
  // so multiple bytes need to be sent in one array
  switch( last_command )
  {
    case SERVO_I2C_READ_ANGLE_COMMAND:
    #ifdef DO_LOGGING
    //Serial.print ("i2cRequestEvent - read\n");
    #endif
    Wire.write(63); //current_angle/1000);
    /*
    for( int i = 0; i < 4; i ++ )
    {
      unsigned long l = (current_angle >> (8 * i)) & 0xff;
      
      //Serial.print (l);
      //Serial.print (" ");
      unsigned char c = l;
      
      Wire.write(c); // respond with number of bytes as expected by master
    }
    */
    break;

    default:
     #ifdef DO_LOGGING
    //Serial.print ("i2cRequestEvent - default\n");
    #endif
  Wire.write(17);
  
    break;
  } 
  

}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void i2CReceiveEvent(int howMany)
{
  #ifdef DO_LOGGING
  //Serial.print ("i2CReceiveEvent\n");
  //Serial.println(howMany);
#endif

  if( Wire.available() < 1)
    return;
    
  
   last_command = Wire.read();

   switch( last_command )
  {
    case SERVO_I2C_SEND_TARGET_COMMAND:
    {
    long param = 0;
    int i = 0;
    
    while( Wire.available() > 0 && i < 4)
    {
      unsigned char c = Wire.read();
      long l = c;
      
#ifdef DO_LOGGING
  //Serial.print (l);
  //  Serial.print (" ");

#endif

      param = param | (l << (i*8)); // receive a byte as character
      i++;
    }
    
  
 
 
 
    
      target = param;
      
#ifdef DO_LOGGING
  //Serial.print (" giving ");
  //Serial.println (target);
#endif
    }
      break;
    
    case SERVO_I2C_READ_ANGLE_COMMAND:
      //nothing to do, action continues in i2cRequestEvent
      break;
  }
}
