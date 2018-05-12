#include <ArduinoSTL.h>

/* 
 * Yonder Dynamics 2018
 *
 * 
 */
//#include <StandardCplusplus.h>
#include "LinearActuator.h"
#include "ActuatedJoint.h"

#define MAX_SPEED 255

const int NUM_ACTUATORS = 2;
const unsigned long BAUDRATE = 19200;

const int BUFFER_LEN = 64;
char buffer[BUFFER_LEN];

float goals[NUM_ACTUATORS];

ActuatedJoint** drivers;

int parse_floats(char* input,float* dest,int num_vals,char delim,int len){
  int i = 0;
  int j = 0;
  int arg = 0;
  for (; input[i] != 0 && i < len; i++) {
    if (input[i] == delim) {
      input[i] = 0;
      dest[arg++] = atof(input+j);
      j = i + 1;
      if(arg == num_vals){
        break;
      }
    }
  }
  if(i==len){
      return -1;
  }
  return 0;
}

void make_driver(ActuatedJoint** driver,distance_t tol,int dir,int pwm,int trig,int echo,
                    float left_aligned, float left_perp, float right_aligned, float right_perp){
    mount_ptr mount = malloc(sizeof(mount_t));
    *mount = {
        left_aligned,left_perp,
        right_aligned,right_perp,
        0,0,0,0
    };
    *driver = new ActuatedJoint(new LinearActuatorDriver(tol,dir,pwm,trig,echo),mount);
}

void setup(){
    Serial.begin(BAUDRATE);
    drivers = malloc(sizeof(ActuatedJoint*) * NUM_ACTUATORS);
}

void loop(){
    if(Serial.available()){
        int read = Serial.readBytesUntil('\n', buffer, BUFFER_LEN);
        buffer[read] = 0; //null terminate the string just in case
        if(parse_floats(buffer,goals,NUM_ACTUATORS,',',read)){
            Serial.println("parse error");
        }
        int i;
        for(i = 0; i < NUM_ACTUATORS; i++){
            drivers[i]->setGoal(goals[i]);
            Serial.print("Actuator: ");
            Serial.print(i);
            Serial.print("Goal:");
            Serial.println(goals[i]);
        }
    }
}