
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


/*
#define WIRE Wire
#define READ read
#define WRITE write


#endif
*/


#ifndef ATTINY45 
  //#define DO_LOGGING
#endif

float now = 0.0;
float last_t = 0.0;
float dt;

float loop_delay = 0.01;

float current_angle = 0; // most recent angle measured from servo pot

float target = 90; // Current target angle from i2c, used to drive motion-control

float max_power = 90; // max output signal, effectively a max-output-power setting

float min_angle = 1; // soft endstops
float max_angle = 179; 

float max_speed = 1000.0; 
float acceleration = 40.0; 

// PID constants
float Kp = 0.4;
float Ki = 0.0;
float Kd = 0.02;


void setup() 
{ 
 
#ifdef DO_LOGGING
  Serial.begin(9600);
#endif

pid_setup();  
i2c_slave_setup();
motion_setup();

} 
 
void loop() 
{ 

  now = millis();

  if( last_t == 0.0 )
    dt = loop_delay; // stop us genreating loopy numbers first time round
  else  
    dt = (now - last_t) / 1000.0;
  
  pid_read_angle();
  
  float pid_target = motion_loop(); // Motion control generates a desired position right now
  
  pid_loop( pid_target ); // and we ask the pid to get us to that position
  
  delay( (int) (loop_delay * 1000.0) );  // no point going round too often, since our output is a servo pulse every 20ms
  
  last_t = now;
} 



