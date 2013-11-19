
float curr_speed = 0.0;
float min_speed = 0.1;
float nominal_angle = 90.0;

boolean stop_at_end = true;
boolean first = true;
boolean moving  = false;

void motion_setup()
{
}

float motion_loop()
{
  

 if( first )
  {
    target = current_angle;
    nominal_angle = current_angle;
   
    first = false;
  }
  
  
   float distance;
  
  float sign = 1.0;
  
  distance = target - nominal_angle;

  if( distance < 0.0 )
    sign = -1.0;
 
 
  if( abs(distance) < 180.0/512.0 ) // arrived
  {
   #ifdef DO_LOGGING
    if( moving )
      Serial.println(" arrived");
   #endif
    
    moving = false;
    return target;
  }

 
 moving = true;
    
  if( stop_at_end && in_braking_zone(distance) )
  {
    curr_speed -= acceleration * dt * sign;
    #ifdef DO_LOGGING
    Serial.println(" braking: ");
    #endif
    /*if( abs(curr_speed) < min_speed )
    {
      curr_speed = min_speed * sign;
      Serial.println(" on min speed");
    }
    */
  }
  else if( abs(curr_speed) < max_speed )
  {
    #ifdef DO_LOGGING
    Serial.println(" accelerating: ");
    #endif
    curr_speed += acceleration * dt * sign;
    
    if( abs(curr_speed) > max_speed )
    {
      curr_speed = max_speed*sign;
      Serial.println(" on max speed");
    }
  }
  else
  {
    #ifdef DO_LOGGING
    Serial.println(" cruising");
    #endif
  }
  
  
  #ifdef DO_LOGGING
   Serial.print ("target" );
  Serial.print (target);
 
    Serial.print (" nominal angle: " );

  Serial.print (nominal_angle);
  
  Serial.print (" distance: ");
  
  Serial.print (distance );
  
  Serial.print (" dt: ");
  Serial.print (dt);
  
 Serial.print (" speed: ");
  Serial.print (curr_speed);
  
  Serial.print (" giving: ");
  #endif
  
  nominal_angle += dt * curr_speed;
  
#ifdef DO_LOGGING
  Serial.print (nominal_angle);
  Serial.println ("");
  #endif
  
  last_t = now;
  
  return nominal_angle;

}

boolean in_braking_zone( float distance )
{
  // s = 0.5 a t^2
  // s = 0.5 v t
  // => s = v^2 / 2a
  
  if( curr_speed * distance < 0.0 ) // going the wrong way, don't brake!
    return false;
    
  float s = curr_speed * curr_speed * 0.5 / acceleration;
  
  return abs(distance) < s;
}

