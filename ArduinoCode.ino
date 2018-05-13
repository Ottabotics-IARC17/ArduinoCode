/*
Ottabotics Ariel Robotics Team

Arduino acting as a junction and translator between Reciever, Serial, and Flight Controller.

Humam Shwaikh
Brennan Macdonald

*/

#include <Servo.h>

int MAX_WRITE = 160;
int MIN_WRITE = 20;
int MID_WRITE = 90;

Servo throttle;  // create servo object to control a servo
Servo pitch;
Servo roll; 
Servo yaw; 

const int MAX_PULSE = 1875;
const int MIN_PULSE = 1050;

int serialThrottle = 512;    // variable to read the value from the serial port
int serialPitch = 512; 
int serialRoll = 512; 
int serialYaw = 512; 

int inputThrottle = 0;    // variable to read the value from the analog pin
int inputPitch = 90; 
int inputRoll = 90; 
int inputYaw = 90; 

int throttlePinIn = 11;
int pitchPinIn = 9;
int yawPinIn = 12;
int rollPinIn = 10;
int autoPin = 8;

unsigned long throttlePulse;
unsigned long pitchPulse;
unsigned long rollPulse;
unsigned long yawPulse;


// Throttle 11
// Yaw 12
// Pitch 9
// Roll 7 or 10


bool autonomous = false;

void setup() {
  throttle.attach(2);  // attaches the servo on pin 9 to the servo object
  pitch.attach(3);
  roll.attach(5);
  yaw.attach(4);
  
  pinMode(throttlePinIn, INPUT);
  pinMode(pitchPinIn, INPUT);
  pinMode(yawPinIn, INPUT);
  pinMode(rollPinIn, INPUT);

  pinMode(autoPin, INPUT);
  pinMode(6, INPUT);
  pinMode(8, INPUT);

  //initalization
  pitch.write(MID_WRITE); 
  roll.write(MIN_WRITE);
  throttle.write(MIN_WRITE);
  yaw.write(MID_WRITE); 


  Serial.begin(9600);
  arm();
}

void loop() {


  //Tests the "gear" switch to see if it's activated. If it is, autonomous flight will start.
  if (pulseIn(autoPin, HIGH) > 1500 && !autonomous){ 
    autonomous = true;  
    Serial.write("Autopilot Engaged");
  } else if (pulseIn(autoPin, HIGH) < 1500 && autonomous){
    autonomous = false;
    Serial.write("Autopilot Disengaged");
  }
  
  if (!autonomous){

    /*    Passthrough RC to flight board
     *    reads the pulse length from the reciever, remaps it our write values, 
     *    then writes to the corresponding contol
     * 
     */

    //Serial.println(pulseIn(8, HIGH));
     
    throttlePulse = pulseIn(throttlePinIn,HIGH);
    pitchPulse = pulseIn(pitchPinIn,HIGH);
    rollPulse = pulseIn(rollPinIn,HIGH);
    yawPulse = pulseIn(yawPinIn,HIGH);
    
    throttlePulse = map(throttlePulse, MIN_PULSE, MAX_PULSE, 10, MAX_WRITE);
    pitchPulse = map(pitchPulse, MIN_PULSE, MAX_PULSE, MIN_WRITE, MAX_WRITE);
    rollPulse = map(rollPulse, MIN_PULSE, MAX_PULSE, MIN_WRITE, MAX_WRITE);
    yawPulse = map(yawPulse, MIN_PULSE, MAX_PULSE, MIN_WRITE, MAX_WRITE);
    
    throttle.write(throttlePulse); //throttle passthrough
    pitch.write(pitchPulse); //pitch passthrough
    roll.write(rollPulse); //roll passthrough
    yaw.write(yawPulse); //yaw passthrough
    
  } else {    

    //Serial.println("Destroy all humans.");
    
    Serial.available();
    
    if (Serial.available() > 0) {
      // Control byte - (a,d,p,r,y,c) 
      int inByte = Serial.read();
      
      char* readString = "";
      
      // future use, command arguments
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



