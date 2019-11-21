/*
Author: Ben Finch
Purpose: ECE298
Description: Implementation of stepper.h.
*/

#include "stepper.h"

stepper initStepper(uint8_t *ports, uint16_t *pins, uint8_t *limitports, uint16_t *limitpins) {
    volatile int i;
    for (i = 0; i < sizeof(ports)/sizeof(ports[0]); i++)
        GPIO_setAsOutputPin(ports[i], pins[i]);

    stepper newStepper;

    newStepper.ports = ports;
    newStepper.pins = pins;
    newStepper.speed = 1;
    newStepper.currentDist = 0;
    newStepper.targetDist = 0;
    newStepper.stepState = 0;
    newStepper.maxDist = 0;
    newStepper.time = 0;
    newStepper.maxport = limitports[0];
    newStepper.minport = limitports[1];
    newStepper.maxpin = limitpins[0];
    newStepper.minpin = limitpins[1];
    newStepper.swap = 1;

    return newStepper;
}

void setSpeed(stepper *motor, int speed) {
    motor->speed = speed;
}

void setTarget(stepper *motor, int target) {
    motor->targetDist = target;
}

void setDist(stepper *motor, int dist) {
    motor->currentDist = dist;
}

int step(stepper *motor, int direction) {
    GPIO_setAsOutputPin(motor->maxport, motor->maxpin);
    GPIO_setAsOutputPin(motor->minport, motor->minpin);
    GPIO_setOutputLowOnPin(motor->maxport, motor->maxpin);
    GPIO_setOutputLowOnPin(motor->minport, motor->minpin);
    GPIO_setAsInputPin(motor->maxport, motor->maxpin);
    GPIO_setAsInputPin(motor->minport, motor->minpin);

    if (GPIO_getInputPinValue(motor->maxport, motor->maxpin) && direction > 0)
        return -1;
    if (GPIO_getInputPinValue(motor->minport, motor->minpin) && direction < 0)
        return -1;

    if (motor->time < motor->speed) {
        motor->time += 1;
        return 0;
    }
    else
        motor->time = 0;

    if (motor->currentDist != motor->targetDist) {
        motor->stepState += direction*motor->swap;
        motor->currentDist += direction;

        if (motor->stepState < 0)
            motor->stepState = 3;
        if (motor->stepState > 3)
            motor->stepState = 0;

        switch (motor->stepState) {
            case 0:  // 1000
                GPIO_setOutputHighOnPin(motor->ports[0], motor->pins[0]);
                GPIO_setOutputLowOnPin(motor->ports[1], motor->pins[1]);
                GPIO_setOutputLowOnPin(motor->ports[2], motor->pins[2]);
                GPIO_setOutputLowOnPin(motor->ports[3], motor->pins[3]);
                break;
            case 1:  // 0100
                GPIO_setOutputLowOnPin(motor->ports[0], motor->pins[0]);
                GPIO_setOutputHighOnPin(motor->ports[1], motor->pins[1]);
                GPIO_setOutputLowOnPin(motor->ports[2], motor->pins[2]);
                GPIO_setOutputLowOnPin(motor->ports[3], motor->pins[3]);
                break;
            case 2:  //0010
                GPIO_setOutputLowOnPin(motor->ports[0], motor->pins[0]);
                GPIO_setOutputLowOnPin(motor->ports[1], motor->pins[1]);
                GPIO_setOutputHighOnPin(motor->ports[2], motor->pins[2]);
                GPIO_setOutputLowOnPin(motor->ports[3], motor->pins[3]);
                break;
            case 3:  //0001
                GPIO_setOutputLowOnPin(motor->ports[0], motor->pins[0]);
                GPIO_setOutputLowOnPin(motor->ports[1], motor->pins[1]);
                GPIO_setOutputLowOnPin(motor->ports[2], motor->pins[2]);
                GPIO_setOutputHighOnPin(motor->ports[3], motor->pins[3]);
                break;
        }
        return 0;
    }

    return 1;
}

int drive(stepper *motor) {
    int dir = -1;
    if (getLocation(*motor) < getTarget(*motor))
        dir = 1;
    if (getLocation(*motor) == getTarget(*motor))
            return 1;
    return step(motor, dir);
}

int getSpeed(stepper motor) {
    return motor.speed;
}

int getLocation(stepper motor) {
    return motor.currentDist;
}

int getTarget(stepper motor) {
    return motor.targetDist;
}

int getState(stepper motor) {
    return motor.stepState;
}

int getMax(stepper motor) {
    return motor.maxDist;
}
