/*
 * lcd.c
 *
 *  Created on: Sep 21, 2019
 *      Author: simon
 */

#include <api/lcd.h>
#include "string.h"

void print(char *str) {
    int length = strlen(str);
    int pos[6] = {pos1, pos2, pos3, pos4, pos5, pos6};

    if (length > 6) {
        displayScrollText(str);
        return;
    }

    unsigned int i;
    for (i = length; i > 0; i--) {
        showChar(str[i - 1], pos[5 - length + i]);
    }
}
