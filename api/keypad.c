/*
 * keypad.c
 *
 *  Created on: Sep 22, 2019
 *      Author: simon
 */

#include <api/keypad.h>
#include <api/lcd.h>
#include "driverlib/driverlib.h"

void Init_KEYPAD() {
    GPIO_setAsOutputPin(GPIO_COL_1);
    GPIO_setAsOutputPin(GPIO_COL_2);
    GPIO_setAsOutputPin(GPIO_COL_3);

    GPIO_setAsInputPin(GPIO_ROW_1);
    GPIO_setAsInputPin(GPIO_ROW_2);
    GPIO_setAsInputPin(GPIO_ROW_3);
    GPIO_setAsInputPin(GPIO_ROW_4);
}

int scan_keypad() {
    keypad_state keypad = {0};

    while(1) {
        scan_column(0, &keypad);
        scan_column(1, &keypad);
        scan_column(2, &keypad);

        if (keypad.btn[0][0] >= SAMPLES_REQUIRED) print("B1 DWN");
        else if (keypad.btn[0][1] >= SAMPLES_REQUIRED) print("B4 DWN");
        else if (keypad.btn[0][2] >= SAMPLES_REQUIRED) print("B7 DWN");
        else if (keypad.btn[0][3] >= SAMPLES_REQUIRED) print("BA DWN");
        else if (keypad.btn[1][0] >= SAMPLES_REQUIRED) print("B2 DWN");
        else if (keypad.btn[1][1] >= SAMPLES_REQUIRED) print("B5 DWN");
        else if (keypad.btn[1][2] >= SAMPLES_REQUIRED) print("B8 DWN");
        else if (keypad.btn[1][3] >= SAMPLES_REQUIRED) print("B0 DWN");
        else if (keypad.btn[2][0] >= SAMPLES_REQUIRED) print("B3 DWN");
        else if (keypad.btn[2][1] >= SAMPLES_REQUIRED) print("B6 DWN");
        else if (keypad.btn[2][2] >= SAMPLES_REQUIRED) print("B9 DWN");
        else if (keypad.btn[2][3] >= SAMPLES_REQUIRED) break;
        else print("NO BTN");
    }

    return 0;
}

void scan_column(unsigned int col, keypad_state *keypad) {
    if (col == 0) GPIO_setOutputHighOnPin(GPIO_COL_1);
    if (col == 1) GPIO_setOutputHighOnPin(GPIO_COL_2);
    if (col == 2) GPIO_setOutputHighOnPin(GPIO_COL_3);

    (GPIO_getInputPinValue(GPIO_ROW_1) == GPIO_INPUT_PIN_HIGH) ? (keypad->btn[col][0]++) : (keypad->btn[col][0] = 0);
    (GPIO_getInputPinValue(GPIO_ROW_2) == GPIO_INPUT_PIN_HIGH) ? (keypad->btn[col][1]++) : (keypad->btn[col][1] = 0);
    (GPIO_getInputPinValue(GPIO_ROW_3) == GPIO_INPUT_PIN_HIGH) ? (keypad->btn[col][2]++) : (keypad->btn[col][2] = 0);
    (GPIO_getInputPinValue(GPIO_ROW_4) == GPIO_INPUT_PIN_HIGH) ? (keypad->btn[col][3]++) : (keypad->btn[col][3] = 0);

    if (col == 0) GPIO_setOutputLowOnPin(GPIO_COL_1);
    if (col == 1) GPIO_setOutputLowOnPin(GPIO_COL_2);
    if (col == 2) GPIO_setOutputLowOnPin(GPIO_COL_3);
    __delay_cycles(32);
}
