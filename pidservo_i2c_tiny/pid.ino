
#define DO_SERVO // just so we can turn the servo code off for debugging purposes

#ifdef DO_SERVO
#include <SoftwareServo.h> 
#endif

#ifndef ATTINY
// must be on a real Arduino,
int servo_pot_pin = 0;  // analog pin used to connect the potentiometer
int servo_output_pin = 9; // drive signal to the servo


#else
// ATTiny45 doesn't have A4
int servo_pot_pin = 2;  // chip pin 3 - analog pin used to connect the potentiometer
int servo_output_pin = 1; // chip pin 6, PB1 drive signal to the servo
#endif


#ifdef DO_SERVO
SoftwareServo myservo; 
#endif

// loop state
float previous_error = 0;
float integral = 0;





void pid_setup() 
{ 
  #ifdef DO_SERVO
  myservo.attach(servo_output_pin);  // attaches the servo on pin 9 to the servo object 
  #endif
  
}

void pid_read_angle()
{
    long val = analogRead(servo_pot_pin);            // reads the value of the servo potentiometer (value between 0 and 1023) 
  // and convert to millidegree 
  current_angle = map(val, 0, 1023.0, 0, 180.0);   
}

void pid_loop( float pid_target ) 
{ 
  long derivative;
  long output; // nominal output angle in millidegrees, which will generally be about 90000

   
  
  

#ifdef DO_LOGGING
  //Serial.println (current_angle/1000);
  //Serial.println (target/1000);
#endif
 
  float error = current_angle - pid_target;
  error = -error; // fix to make it go the right way - alternatively, could swap the feedback pot wires over
  
  
  
  integral = integral + error*dt;
  derivative = (error - previous_error)/dt;
  
    //Serial.println (derivative);

  // i term tends to be huge so gets a bigger divisor
  output = (Kp*error) + (Ki*integral)/100.0 + (Kd*derivative);
  // output is (roughly) in the range -90 to 90, with 0 when here's no error
  
  previous_error = error;

// stop us driving into the endstops by cutting drive when near the ends of the available rotation
  if( current_angle < min_angle && output < 0.0 )
    output = 0.0;
    
  if( current_angle > max_angle && output > 0.0 )
    output = 0.0;

// constrain output signal to a rational range, so we don't generate irrationally long/short pulses
  if( output < - max_power )
    output = -max_power;
    
  if( output > max_power )
    output = max_power;
    

#ifdef DO_SERVO  
#ifdef ATTINY
    
    
    myservo.write( output  + 90.0 );
    
     SoftwareServo::refresh();
    
#else
// Driving the servo in uS rather than degrees gets round a resolution problem with Arduino Servo.write    
// Output of 0 (for no error) maps to a pulse of 1500 uS
// That matches the fixed feedback resistors we've soldered into the servo, which make it think 
// it's always at the middle of its range

  float us = map( output, -max_power, max_power, 1000.0, 2000.0 );

  myservo.writeMicroseconds( (int) us ); // 1500 us is about the neutral position
  
#endif
#endif
  

} 
