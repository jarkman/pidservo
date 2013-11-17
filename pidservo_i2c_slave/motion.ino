
float curr_speed = 0.0;
float min_speed = 0.1;
float nominal_angle = 90.0;

boolean stop_at_end = true;
boolean first = true;

float last_t = 0.0;
void motion_setup()
{
}

float motion_loop()
{
  

 if( first )
  {
    target = current_angle;
    nominal_angle = current_angle;
    last_t = millis();
    first = false;
  }
  
  
   float distance, now;
  
  float sign = 1.0;
  
  distance = target - nominal_angle;

  if( distance < 0.0 )
    sign = -1.0;
 
  now = millis();
 
  if( abs(distance) < 180.0/512.0 ) // arrived
  {
    //Serial.println(" arrived");
    last_t = now;
    return target;
  }

 
  
  dt =  (now - last_t) / 1000.0;
  
  if( stop_at_end && in_braking_zone(distance) )
  {
    curr_speed -= acceleration * dt * sign;
    Serial.println(" braking: ");
    
    /*if( abs(curr_speed) < min_speed )
    {
      curr_speed = min_speed * sign;
      Serial.println(" on min speed");
    }
    */
  }
  else if( abs(curr_speed) < max_speed )
  {
    Serial.println(" accelerating: ");
    curr_speed += acceleration * dt * sign;
    
    if( abs(curr_speed) > max_speed )
    {
      curr_speed = max_speed*sign;
      Serial.println(" on max speed");
    }
  }
  else
  {
    Serial.println(" cruising");
  }
  
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
  
  nominal_angle += dt * curr_speed;
  

  Serial.print (nominal_angle);
  Serial.println ("");
  
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

