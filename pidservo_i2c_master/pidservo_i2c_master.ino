
/*

pidservo_i2c_master

This sketch sends angle instructions over i2c to pidservo_i2c_slave.

Run this sketch on one arduino, wired to a pot with a knob, 
and run pidservo_i2c_slave on the other arduino, wired to a servo.

This knob will then control that servo.


Wiring:
SCL (analog pin 5) to SCL (analog pin 5) on the slave Arduino
SDA (analog pin 4) to SDA (analog pin 4) on the slave Arduino
(unless you're on a Leonardo, in which case it's digital pins 3 and 2)
0v to 0v on the slave Arduino 

Analog pin 1 to the wiper of our control pot (as a source of a test signal)
+5v to one end of the servo pot track
0v to the other end of the servo pot track

*/

#include <Servo.h> 
#include <Wire.h> 

#include "i2c_commands.h"


int servo_pot_pin = 0;  // analog pin used to connect the potentiometer
long current_angle = 0; // most recent angle from servo pot

long target = 90000; // in thousanths of a degree
int led = 13;

#define DO_LOGGING

void setup() 
{ 
  #ifdef DO_LOGGING
  Serial.begin(9600);
  #endif
  pinMode(led, OUTPUT); 
  
 Wire.begin(); // start i2c as master
  // Arduino analog input 5 - I2C SCL
  // Arduino analog input 4 - I2C SDA


} 
 
void loop() 
{ 

  // and read our command pot
  long val = analogRead(1);
  // convert to millidegrees
  target = map(val, 0, 1023.0, 0, 180000);
  
  //Serial.println (target/1000);
 
  send_target( target );
  
  //Serial.println ("sent target");

  
  //read_current_angle();
  
  //Serial.println (current_angle);
  
  digitalWrite(led, HIGH);  
  delay(200);              
  digitalWrite(led, LOW);
  
  delay(2000);   // Wait a while so we can see the operation of the servo when undisturbed
  
} 

void send_target( long target )
{
  
  Wire.beginTransmission(SERVO_I2C_ADDRESS); // transmit to device #44 (0x2c)
                              // device address is specified in datasheet
  Wire.write(SERVO_I2C_SEND_TARGET_COMMAND);            // sends instruction byte  
  for( int i = 0; i < 4; i ++ )
      Wire.write((char) ((target >> (8 * i)) & 0xff)); 

  Wire.endTransmission();     // stop transmitting
}


void read_current_angle()
{
    while(Wire.available())
    {
      Serial.println ("discard 1");
      Wire.read();
    }
    
   Wire.beginTransmission(SERVO_I2C_ADDRESS); 
                              
  Wire.write(SERVO_I2C_READ_ANGLE_COMMAND);        

  Wire.endTransmission();    

  int recd = Wire.requestFrom(SERVO_I2C_ADDRESS, 1);    // request 4 bytes from slave

Serial.print ("recd ");
Serial.println (recd);

  unsigned long param = 0;
   int c = Wire.read();
    current_angle = c;
    
 while(Wire.available())
    {
      Serial.println ("discard 2");
      Wire.read();
    }
  /*  
  int i = 0;
  while(Wire.available() )    // slave may send less than requested
  {
    int c = Wire.read();
    unsigned long l = c;
      
      Serial.print (c);
      Serial.print (" ");
      
     param = param | (l << (i*8));
    i++;
  }
*/

  current_angle = param;
}


