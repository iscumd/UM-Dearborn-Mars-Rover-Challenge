#ifndef __JOYSTICK_C__
#define __JOYSTICK_C__

/*
 * This file is common to most development.
 * If you make any changes here, it will affect everyone.
 * Please avoid making changes to this file.
 */
/*
 * Declared joy0 here.
 * Expect joy0 multiply defined errors!
 */

//#define DEBUGJOY // For debugging


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "joystick.h"

// The following line may have no effect on most compilers
// It is here as a reminder of what libraries are needed
//#pragma comment(lib,"WINMM.LIB")


// get/draw joystick data
#ifndef INT2DBL
#define INT2DBL(x) ((x-32767.0)/32767.0)
#endif


int joystick()
{
    DWORD id;
    memset(&joy0, 0, sizeof(joy0));
    for (id = 0; id < 16; id++) {
        JOYINFOEX joyInfoEx;
        ZeroMemory(&joyInfoEx, sizeof(joyInfoEx));
        joyInfoEx.dwSize = sizeof(joyInfoEx);
        BOOL JoyPresent = (joyGetPosEx(id, &joyInfoEx) == JOYERR_NOERROR);

        // fprintf(stderr, "\nLooking for joystick %d\n\n", id);
        if (!JoyPresent)
            continue;

        JOYCAPS joyCaps;
        ZeroMemory(&joyInfoEx, sizeof(joyInfoEx));
        joyInfoEx.dwSize = sizeof(joyInfoEx);

        joyGetDevCaps(id, &joyCaps, sizeof(joyCaps));

        //if (JoyPresent)
        {
            joyInfoEx.dwFlags = JOY_RETURNALL | JOY_RETURNCENTERED;
            joyInfoEx.dwSize = sizeof(joyInfoEx);

            joyGetPosEx(id, &joyInfoEx);
            joy0.axis[0] = INT2DBL(joyInfoEx.dwXpos);
            joy0.axis[1] = INT2DBL(joyInfoEx.dwYpos);
            joy0.axis[2] = INT2DBL(joyInfoEx.dwZpos);
            joy0.axis[3] = INT2DBL(joyInfoEx.dwRpos);
            joy0.axis[4] = INT2DBL(joyInfoEx.dwUpos);
            joy0.axis[5] = INT2DBL(joyInfoEx.dwVpos);


            joy0.buttons = joyInfoEx.dwButtons;


            joy0.hat = -1;
            if (joyCaps.wCaps & JOYCAPS_HASPOV) {
                int dir = (joyInfoEx.dwPOV / 100);
                if (dir > 360)
                    dir = -1;   // this will be centered

                joy0.hat = dir;
                //              joy0.hat = joyInfoEx.dwPOV;
            }

        }
        return 1;

    }
    return 0;
}

#ifdef DEBUGJOY
// Sample test code
struct JoystickData joy0;
int main()
{
    int k, n;
    printf("\n"
           " +====================================================+\n"
           " |                                                    |\n"
           " |   Joystick library test program                    |\n"
           " |   (copyleft) N. Natarajan                          |\n"
           " |                                                    |\n"
           " |   Press buttons 1 & 2 simultaneouly to quit        |\n"
           " |                                                    |\n"
           " +====================================================+\n");
    printf("\n\n%8s %8s %8s %8s  %3s %6s %8s %8s\n",
           "axis-0", "axis-1", "axis-2", "axis-3",
           "btn", "hat", "axis-4", "axis-5");
    while (1) {
        if (0 == joystick())
            break;

        for (k = 0; k < 4; k++)
            printf("%8.3f ", joy0.axis[k]);
        printf(" %03x ", joy0.buttons);
        printf("%6d ", joy0.hat);
        for (; k < 6; k++)
            printf("%8.3f ", joy0.axis[k]);
        printf("\r");
        if ((joy0.buttons & 3) == 3) {
            printf("\n\nBye\n");
            system("Pause");
            break;
        }
        //      sleep(1);
    }
    system("pause");
}
#endif //DEBUGJOY

#endif //__JOYSTICK_C__
