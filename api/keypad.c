/*
 * keypad.c
 *
 *  Created on: Sep 22, 2019
 *      Author: simon
 */

#include <api/lcd.h>
#include "driverlib/driverlib.h"

int* scan_keypad() {
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN3);
//    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN5);
//    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN0);

    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3);
//    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN5);
//    GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN0);

    GPIO_setAsInputPin(GPIO_PORT_P1, GPIO_PIN4);

    while(1) {
        if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_HIGH) {
            print("PRESSED1");
        } else {
            print("NOOOOO");
        }
//        __delay_cycles(2000);
    }

    int *ret = {0};
    return ret;
}
