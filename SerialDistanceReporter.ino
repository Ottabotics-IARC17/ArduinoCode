/*
 * This sketch uses an ultrasonic sensor to measure distance from the ground.
 * When the Arduino receives the four-byte command GHGT (followed by a newline)
 * via serial (USB cable), it will respond with the current distance measured
 * by the sensor in centimetres.
 */

#include <Wire.h>
#define COMMAND_LENGTH 4  //length of commands in bytes

char receivedChars[4];
int period = 100;
unsigned long time_now = 0;



void setup() {
  
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  Serial.begin(9600);

}

void loop() {
  
    receiveCommand();

    if(receivedChars[0] == 'G' && receivedChars[1] == 'H' && receivedChars[2] == 'G' && receivedChars[3] == 'T'){
      if(millis() > time_now + period){
          time_now = millis();
          
          Serial.println(sensorDistance());
  
      }
    }
}


/**
 * Gets and returns the distance of the object in
 * front of the ultrasonic sensor in centimetres.
 *
 * @return the distance of the closest object in centimetres.
 */
float sensorDistance(){
  digitalWrite(2, LOW);
  delayMicroseconds(2);

  digitalWrite(2, HIGH);
  delayMicroseconds(10);
  digitalWrite(2, LOW);

  long duration = pulseIn(3 ,HIGH);
  
  return duration*0.034/2;
}


/**
 * Gets a four-byte (four-character) from serial and places
 * it into the car array receivedChars.  The command being
 * "listened for" is defined above in loop().
 * Commands are delimited by a newline ('\n').
 */
void receiveCommand(){
  
  static byte index = 0;
  char endchar = '\n';
  char currchar;
  

  while(Serial.available() > 0){
    currchar = Serial.read();

    if(currchar != endchar){
      receivedChars[index] = currchar;
      index++;
      if(index >= COMMAND_LENGTH){
        index = COMMAND_LENGTH - 1;
      }
    }
    else{
      //newline character reached
      receivedChars[index] = '\0';
      index = 0;
    }
  }
}


