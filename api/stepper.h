/*
Author: Ben Finch
Purpose: ECE298
Description: Driver logic for 4 wire stepper motors.
*/

#include "driverlib/driverlib.h"

typedef struct stepper {
    //Define the stepper motor object.
    uint8_t *ports;     //The 4 ports that control the stepper motor.
    uint16_t *pins;     //The 4 pins that control the stepper motor.
    int speed;          //The speed that the motor steps at.
    int currentDist;    //Current location on axis.
    int targetDist;     //Target location on axis.
    int stepState;      //Current state rotor for stepping.
    int maxDist;        //Max distance away from origin.
    int time;           //Time since last step
    int minport;        //min limit port
    int minpin;         //min limit pin
    int maxport;        //max limit port
    int maxpin;         //max limit pin
} stepper;

//Init and set functions.
stepper initStepper(uint8_t *ports, uint16_t *pins, uint8_t *limitports, uint16_t *limitpins);
void setSpeed(stepper *motor, int speed);
void setTarget(stepper *motor, int target);
void setDist(stepper *motor, int dist);

//Actuate and get functions.
int step(stepper *motor, int direction);
int drive(stepper *motor);
int getSpeed(stepper motor);
int getLocation(stepper motor);
int getTarget(stepper motor);
int getState(stepper motor);
int getMax(stepper motor);
