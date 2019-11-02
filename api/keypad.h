/*
 * keypad.h
 *
 *  Created on: Sep 22, 2019
 *      Author: simon
 */

#ifndef API_KEYPAD_H_
#define API_KEYPAD_H_

#include <api/lcd.h>
#include "driverlib/driverlib.h"
#include <stdio.h>

#define GPIO_MUX_ADDR_0 GPIO_PORT_P5, GPIO_PIN3
#define GPIO_MUX_ADDR_1 GPIO_PORT_P5, GPIO_PIN2

#define GPIO_COL_1 GPIO_PORT_P5, GPIO_PIN0
#define GPIO_COL_2 GPIO_PORT_P1, GPIO_PIN6
#define GPIO_COL_3 GPIO_PORT_P1, GPIO_PIN7

#define SAMPLES_REQUIRED 16
#define ROW_SCAN_DELAY 16

#define B1_VAL 1
#define B2_VAL 2
#define B3_VAL 3
#define B4_VAL 4
#define B5_VAL 5
#define B6_VAL 6
#define B7_VAL 7
#define B8_VAL 8
#define B9_VAL 9
#define B0_VAL 10
#define BA_VAL 11
#define BH_VAL 12
#define NO_KEY_VAL 0

typedef struct {
    unsigned int btn[3][4];
} keypad_state;

void Init_KEYPAD(  );
unsigned long int nextKeypadValue(  );
int anyKeyDown(  );
int getKeyDown( keypad_state* );
void scanRow( unsigned int, keypad_state* );

#endif /* API_KEYPAD_H_ */
