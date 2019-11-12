/*
 * keypad.c
 *
 *  Created on: Sep 22, 2019
 *      Author: simon
 */

#include <api/keypad.h>

void Init_KEYPAD() {
    GPIO_setAsInputPin(GPIO_COL_1);
    GPIO_setAsInputPin(GPIO_COL_2);
    GPIO_setAsInputPin(GPIO_COL_3);

    GPIO_setAsOutputPin(GPIO_MUX_ADDR_0);
    GPIO_setAsOutputPin(GPIO_MUX_ADDR_1);
}

unsigned long int nextKeypadValue() {
    keypad_state keypad = { 0 };
    unsigned long int val = 0;
    int prev, curr = 0;
    char str[6];

    while(1) {
        prev = curr;
        curr = getKeyDown(&keypad);

        if (curr == BH_VAL) {
            break;
        } else if (curr == BA_VAL && curr != prev) {
            // TODO: Make number negative if nothing input yet, otherwise keep current backspace behavior
            val /= 10;
        } else if (curr && curr != prev) {
            val = (val * 10) + (curr % 10);
        }

        sprintf(str, "%d DWN", curr);
        print(str);
    }

    return val;
}

int anyKeyDown() {
    keypad_state keypad = { 0 };
    int i;

    for (i = SAMPLES_REQUIRED; i > 0; i--) {
        getKeyDown(&keypad);
    }

    return getKeyDown(&keypad) != NO_KEY_VAL;
}

int getKeyDown(keypad_state *keypad) {
    scanRow(0, keypad);
    scanRow(1, keypad);
    scanRow(2, keypad);
    scanRow(3, keypad);

    if (keypad->btn[0][0] >= SAMPLES_REQUIRED) return B1_VAL;
    else if (keypad->btn[1][0] >= SAMPLES_REQUIRED) return B2_VAL;
    else if (keypad->btn[2][0] >= SAMPLES_REQUIRED) return B3_VAL;
    else if (keypad->btn[0][1] >= SAMPLES_REQUIRED) return B4_VAL;
    else if (keypad->btn[1][1] >= SAMPLES_REQUIRED) return B5_VAL;
    else if (keypad->btn[2][1] >= SAMPLES_REQUIRED) return B6_VAL;
    else if (keypad->btn[0][2] >= SAMPLES_REQUIRED) return B7_VAL;
    else if (keypad->btn[1][2] >= SAMPLES_REQUIRED) return B8_VAL;
    else if (keypad->btn[2][2] >= SAMPLES_REQUIRED) return B9_VAL;
    else if (keypad->btn[1][3] >= SAMPLES_REQUIRED) return B0_VAL;
    else if (keypad->btn[0][3] >= SAMPLES_REQUIRED) return BA_VAL;
    else if (keypad->btn[2][3] >= SAMPLES_REQUIRED) return BH_VAL;
    else return NO_KEY_VAL;
}

void scanRow(unsigned int row, keypad_state *keypad) {
    (row & 0b01) ? GPIO_setOutputHighOnPin(GPIO_MUX_ADDR_0) : GPIO_setOutputLowOnPin(GPIO_MUX_ADDR_0);
    (row & 0b10) ? GPIO_setOutputHighOnPin(GPIO_MUX_ADDR_1) : GPIO_setOutputLowOnPin(GPIO_MUX_ADDR_1);

    (GPIO_getInputPinValue(GPIO_COL_1) == GPIO_INPUT_PIN_HIGH) ? (keypad->btn[0][row]++) : (keypad->btn[0][row] = 0);
    (GPIO_getInputPinValue(GPIO_COL_2) == GPIO_INPUT_PIN_HIGH) ? (keypad->btn[1][row]++) : (keypad->btn[1][row] = 0);
    (GPIO_getInputPinValue(GPIO_COL_3) == GPIO_INPUT_PIN_HIGH) ? (keypad->btn[2][row]++) : (keypad->btn[2][row] = 0);

    __delay_cycles(ROW_SCAN_DELAY);
}
