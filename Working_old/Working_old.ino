/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

int MAX_WRITE = 160;
int MIN_WRITE = 30;
int MID_WRITE = (MAX_WRITE+MIN_WRITE)/2;

Servo throttle;  // create servo object to control a servo
Servo pitch;
Servo roll; 
Servo yaw; 
Servo nothing; 
Servo alsoNothing; 

int serialThrottle = 512;    // variable to read the value from the serial port
int serialPitch = 512; 
int serialRoll = 512; 
int serialYaw = 512; 

int inputThrottle = 0;    // variable to read the value from the analog pin
int inputPitch = 90; 
int inputRoll = 90; 
int inputYaw = 90; 


// Throttle 11
// Yaw 12
// Pitch 9
// Roll 7


bool autonomous = true;

void setup() {
  throttle.attach(2);  // attaches the servo on pin 9 to the servo object
  pitch.attach(3);
  roll.attach(5);
  yaw.attach(4);
  nothing.attach(7);
  alsoNothing.attach(8);

  //initalization
  pitch.write(MID_WRITE); 
  roll.write(MIN_WRITE);
  throttle.write(MIN_WRITE);
  yaw.write(MID_WRITE); 
  nothing.write(MIN_WRITE);
  alsoNothing.write(MIN_WRITE);

  Serial.begin(2000000);
}

void loop() {
  if (!autonomous){
    
    digitalWrite(2,digitalRead(11));
    digitalWrite(3,digitalRead(9));
    digitalWrite(4,digitalRead(12));
    digitalWrite(5,digitalRead(10));
    
  } else {
    Serial.available();
    if (Serial.available() > 0) {
      int inByte = Serial.read();
      char* readString = "";
      while(Serial.available()){
        char c = Serial.read();
        readString += c;
      }
      
      switch (inByte) {
        case 'a':
          Serial.write("Arming\n");
          arm();
          Serial.write("Armed\n");
          break;
        case 'd':
          Serial.write("Disarming\n");
          disarm();
          Serial.write("Disarmed\n");
          break;
        case 'p':
          Serial.write("Pitching (forward or backward)\n");
          doPitch((int)(readString[1] + readString[2] + readString[3]));
          break;
        case 'r':
          Serial.write("Rolling (left or right)\n");
          doRoll((int)(readString[1] + readString[2] + readString[3]));
          break;
        case 'y':
          Serial.write("Yawing (counterclockwise or clockwise)\n");
          doYaw((int)(readString[1] + readString[2] + readString[3]));
          break;
        case 'c':
          sweep(false);
          downsweep(false);
          break;
        default:
          disarm();
          break;
      }
    }
  }
}

void arm() {
  throttle.write(MIN_WRITE);
  roll.write(MIN_WRITE);
  delay(2000);
}

void disarm() {
  throttle.write(MIN_WRITE);
  roll.write(MAX_WRITE); 
  delay(2000);
}

void hover() {
  throttle.write(MID_WRITE);
}


void sweep(bool all) {
  for(int i = MIN_WRITE; i < MAX_WRITE; i++){
    throttle.write(i);
    if(all){
      roll.write(i);
      pitch.write(i);
      yaw.write(i);
    }
    delay(100);
  }
}

void downsweep(bool all) {
  for(int i = MAX_WRITE; i > MIN_WRITE; i--){
    throttle.write(i);
    if(all){
      roll.write(i);
      pitch.write(i);
      yaw.write(i);
    }
    delay(100);
  }
}

void maxThrottle() {
  throttle.write(50);
}

/*
 * Causes the drone to pitch forward or backward.
 * 
 * @param power percentage value denoting the speed at which the drone moves. (Positive for forward, negative for backward)
 */
void doPitch(int power) {
  pitch.write(toServoValue(power));
}

/*
 * Causes the drone to roll left or right.
 * 
 * @param power percentage value denoting the speed at which the drone moves. (Positive for right, negative for left)
 */
void doRoll(int power) {
  roll.write(toServoValue(power));
}

/*
 * Causes the drone to yaw counterclockwise or clockwise.
 * 
 * @param power percentage value denoting the speed at which the drone moves. (Positive for clockwise, negative for counterclockwise)
 */
void doYaw(int power) {
  yaw.write(toServoValue(power));
}

/*
 * Converts a percentage value of domain (-100, 100) into a suitable servo power value of domain (-100, 160).
 * 
 * @param   percentage percentage value.
 * @return  converted value
 */
int toServoValue(int percentage) {
  return (percentage * 1.3) + 30;
}

/*
 * Casts a string as an int.
 * 
 * NOTE - ideally, this will use a Try/Catch type statement to catch exceptions,
 * but this is unimplemented as of yet.
 * 
 * @param   in the string to be converted.
 * @return  converted integer.
 */
int toInt(char* in) {
  return atoi(in);
}



