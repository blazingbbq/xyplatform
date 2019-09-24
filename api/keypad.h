/*
 * keypad.h
 *
 *  Created on: Sep 22, 2019
 *      Author: simon
 */

#ifndef API_KEYPAD_H_
#define API_KEYPAD_H_

#define GPIO_ROW_1 GPIO_PORT_P5, GPIO_PIN2
#define GPIO_ROW_2 GPIO_PORT_P1, GPIO_PIN7
#define GPIO_ROW_3 GPIO_PORT_P1, GPIO_PIN6
#define GPIO_ROW_4 GPIO_PORT_P5, GPIO_PIN3

#define GPIO_COL_1 GPIO_PORT_P1, GPIO_PIN3
#define GPIO_COL_2 GPIO_PORT_P1, GPIO_PIN5
#define GPIO_COL_3 GPIO_PORT_P5, GPIO_PIN0

#define SAMPLES_REQUIRED 16

typedef struct {
    unsigned int btn[3][4];
} keypad_state;

void Init_KEYPAD(  );
int scan_keypad(  );
void scan_column( unsigned int, keypad_state* );

#endif /* API_KEYPAD_H_ */
