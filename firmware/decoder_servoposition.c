
#include <lpc17xx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decoder_servoposition.h"
#include "servodriver.h"


bool bDecoder_updateServoPosition(char *pcBuffer)
{
    int i32ServoNumber = 0;
    int i32PulseWidth = 0;
    double fAngle = 0.0;
    
    // SPnn wwww
    // SAnn ffff

    if (pcBuffer[0] != 'S' || pcBuffer[4] != ' ')
        return false;

    i32ServoNumber = atoi(&pcBuffer[2]);
    if (i32ServoNumber < 1 || i32ServoNumber > 20) {
        // Servo index out of range
        printf("Servo index out of range\n");
        return false;
    }
    
    // Decrement number to fit array index
    i32ServoNumber--;

    // Servo index in range, get/check position
    if (pcBuffer[1] == 'P') {
        // Get position as pulse width (µs)
        i32PulseWidth = atoi(&pcBuffer[5]);

        if ((i32PulseWidth < 1000) || (i32PulseWidth > 2000)) {
            // Error, width out of range
            printf("Pulse width out of range\n");
            return false;
        }
        
        vServoDriver_setServoPosition((unsigned int)i32ServoNumber, (unsigned int)i32PulseWidth);
        return true;

    } else if (pcBuffer[1] == 'A') {
        // Get position as angle
        fAngle = atof(&pcBuffer[5]);

        if (fAngle < 0.0 || fAngle > 180.0) {
            // Error, angle out of range
            printf("Angle out of range\n");
            return false;
        }
        
        vServoDriver_setServoAngle((unsigned int)i32ServoNumber, (float)fAngle);
        return true;
    }
    
    printf("Unrecognized S cmd\n");

    return false;
}
