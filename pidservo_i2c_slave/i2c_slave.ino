#include "i2c_commands.h"

#include <Wire.h> 

char last_command = '\0';


float read_milli_float();
long read_4_bytes();

void i2c_slave_setup() 
{ 

#ifdef DO_LOGGING
  Serial.print ("Wire.begin\n");
#endif
 
   Wire.onRequest(i2cRequestEvent); // register a function which will be called when the master asks for data
  Wire.onReceive(i2CReceiveEvent); // register a function which will be called when the master send a command (which it will do before askigng for data)

 Wire.begin(SERVO_I2C_ADDRESS); // start i2c as slave
  // Arduino - uses analog input 5 for SCL, analog input 6 for SDA
  // ATTint45 - uses chip pin 7 for SCL, chip pin 5 for SDA


} 



void i2cRequestEvent() // called when master asks for bytes
{
  // NB - the request event handler is only allowed one call of write
  // so multiple bytes need to be sent in one array
  switch( last_command )
  {
    case SERVO_I2C_READ_ANGLE:
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
    case SERVO_I2C_SEND_TARGET:
      target = read_milli_float();
      
#ifdef DO_LOGGING
      //Serial.print (" target:");
      //Serial.println (target);
#endif

      break;
    
    
     case SERVO_I2C_SEND_MIN_ANGLE:
       min_angle =  read_milli_float();
       break;
     
     case SERVO_I2C_SEND_MAX_ANGLE:
       max_angle =  read_milli_float();
       break;
     
     case SERVO_I2C_SEND_MAX_SPEED:
       max_speed =  read_milli_float();
       break;
       
     case SERVO_I2C_SEND_ACCELERATION:
       acceleration =  read_milli_float();
       break;
       
     case SERVO_I2C_SEND_MAX_POWER:
       max_power =  read_milli_float();
       break;
       
     case SERVO_I2C_SEND_PID_KP:
       Kp =  read_milli_float();
       break;
  
     case SERVO_I2C_SEND_PID_KI:
       Ki =  read_milli_float();
       break;
  
     case SERVO_I2C_SEND_PID_KD:
       Kd =  read_milli_float();
       break;
       
 
    case SERVO_I2C_READ_ANGLE:
      //nothing to do, action continues in i2cRequestEvent
      break;
  }
  
  if( target < min_angle )
    target = min_angle;
    
  if( target > max_angle )
    target = max_angle;
}

float read_milli_float()
{
  return ((float) read_4_bytes()) / 1000.0;  // send params in millidegrees  
}

long read_4_bytes()
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
    
    return param;
}
