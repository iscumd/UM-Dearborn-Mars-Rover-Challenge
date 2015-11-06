#ifndef __NASAROBOTEQ_C__
#define __NASAROBOTEQ_C__
/*
 * This file is common to most development.
 * If you make any changes here, it will affect everyone.
 * Please avoid making changes to this file.
 */

#include "nasannlib.h"
#include "nasaroboteq.h"

static HANDLE roboteqHandle = INVALID_HANDLE_VALUE;   /*  Handle to serial port */

void initRoboteq()
{
    roboteqHandle = SerialInit(roboteqPort, 115200, 8, 0, 0, 0, 0, 0);
    SerialPuts(roboteqHandle, "!G 1 0000\r!G 2 0000\r");
}


#define LFWD ('b' ^ 3)
#define RFWD ('b')
#define LREV (LFWD ^ 32)
#define RREV (RFWD ^ 32)
#define SIGN(x) ((x) > 0 ? 1: -1)

#ifndef ABS
#define ABS(val) (val > 0 ? val: -val)
#endif


void sendspeed(int leftSpeed, int rightSpeed)
{
    char buffer[20];
    static int errorCount;

#define DEBUGROBOTEQ //Print Speed Commands
#ifdef DEBUGROBOTEQ
    DEBUG(leftSpeed); DEBUGN(rightSpeed);
#endif
    if (roboteqHandle == INVALID_HANDLE_VALUE) {
        // Either the port was not opened or there was a failure
        // Try and open the robotEq again hope it works
        initRoboteq();
    }
    if(ABS(leftSpeed) > 1000) leftSpeed = SIGN(leftSpeed) * 1000;
    if(ABS(rightSpeed) > 1000) rightSpeed = SIGN(rightSpeed) * 1000;

    sprintf(buffer, "!G %01d %04d\r!G %01d %04d\r",
                leftChannelNum, leftSpeed, rightChannelNum,rightSpeed);

    if (roboteqHandle != INVALID_HANDLE_VALUE) {
        SerialPuts(roboteqHandle, buffer);
        FlushRx(roboteqHandle);
        FlushTx(roboteqHandle);
    } else {
        if (++errorCount % 100 == 0)    // Print every 100 times this happens
            fprintf(stderr, "Invalid roboteqPort. Did you open the port?\n");
    }
}
#endif //__ROBOTEQ_C__
