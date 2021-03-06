/******************************************************************/
/********************Begin Simple Joystick*************************/
/*******Refer To Comments At Bottom For Reference Information******/
/******************************************************************/
#include <conio.h>
#include "nasannlib.h"
#include "nasaroboteq.h"
#include "nasajoystick.h"

#ifndef BIT
#define BIT(k) ((1)<<(k))
#endif

#define A_BTN BIT(0)    // 0000 0000 0000 0001
#define B_BTN BIT(1)    // 0000 0000 0000 0010
#define X_BTN BIT(2)    // 0000 0000 0000 0100
#define Y_BTN BIT(3)    // 0000 0000 0000 1000
#define LB_BTN BIT(4)   // 0000 0000 0001 0000
#define RB_BTN BIT(5)   // 0000 0000 0010 0000
#define BACK_BTN BIT(6) // 0000 0000 0100 0000
#define START_BTN BIT(7)// 0000 0000 1000 0000
#define LTS_BTN BIT(8)  // 0000 0001 0000 0000
#define RTS_BTN BIT(9)  // 0000 0010 0000 0000
#define MAX_SPEED (500)

/*roboteq.h Definitions*/
int roboteqPort = 6;
int leftChannelNum = 2;//Can be 1 or 2 check physical connection
int rightChannelNum = 1;//Can be 1 or 2 check physical connection
/*joystick.h Definitions*/
struct JoystickData joy0;

int main(){
    int count;
    initRoboteq();//Refer to roboteq.c
    count=0;
    while (1) {
        static double turnBoost = 0.85;//Multiplier for turn. Adjust to smooth jerky motions. Usually < 1.0
        static int lSpeed, rSpeed;//Wheel Speed Variables
        if (joystick() != 0) {
            double speed, turn;
            fprintf(stderr,"%5d ", count++);
            if (joy0.buttons & LB_BTN) {
                if (joy0.buttons & A_BTN) {
                    lSpeed=0;
                    rSpeed=MAX_SPEED/2;
                } else if (joy0.buttons&B_BTN){
                    lSpeed=0;
                    rSpeed=-MAX_SPEED/2;
                } else {
                    speed = -joy0.axis[1]; //Up is negative on joystick negate so positive when going forward
                    turn = joy0.axis[0];

/********************Add Boost Here**********************/

                    lSpeed = (int)((speed + turnBoost*turn)*MAX_SPEED);
                    rSpeed = (int)((speed - turnBoost*turn)*MAX_SPEED);
                }
            }else{
                     rSpeed=lSpeed=0;
            }
            sendspeed(lSpeed,rSpeed);//Refer to roboteq.c for how commands are sent
        } else{
            printf("No Joystick Found!\n");
            sendspeed(0,0);
        }
        Sleep(100);//Sleep so you don't flood roboteq with commands.
    }
    return 0;
}

#include "nasannlib.c"
#include "nasajoystick.c"
#include "nasaroboteq.c"
/******************************************************************/
/*********************Comment Section On Joystick******************/
/******************************************************************/
/*
 *
 *   Here is how we can control the robot using a joystick.
 *
 *
 *
 *   Notes:
 *       1) The controler has 6 axis labeled as folloC:\Program Files\Dassault Systemes\B19\win_b64\code\bin\CATSTART.exe"  -run "CNEXT.exe" -env CATIA.V5R19.B19 -direnv "C:\ProgramData\DassaultSystemes\CATEnvws
 *
 *                   ****Axis Map****
 *
 *               Axis 0 = X axis left stick
 *               Axis 1 = Y axis left stick inverted
 *               Axis 2 = triggers left positive (pressed), right negative (pressed), 0 otherwise
 *               Axis 3 = Y axis right stick inverted
 *               Axis 4 = X axis right stick
 *               Axis 5 =
 *
 *       2) The controler has 9 buttons labeled as follows
 *
 *
 *           ****Buton Map****
 *
 *               BIT|Button
 *                0 | A
 *                1 | B
 *                2 | X
 *                3 | Y
 *                4 | LB
 *                5 | RB
 *                6 | BACK
 *                7 | START
 *                8 | LTS CLICK
 *                9 | RTS CLICK
 *
 *       3) To reference an axis/button:
 *
 *               Axis: joy0.axis[i]; where i = the axis of you wish to select.
 *
 *               Buttons: The buttons produce a number of 2^k so inorder to reference the button we use a macro
 *
 *               #ifndef BIT
 *               #define BIT(k) ((1)<<(k))
 *               #endif
 *
 *               Ex. ==> BIT(3) = 2^3 = 8;
 *
 *               (joy0.buttons & BIT(k)); where k = the bit of the button you want
 *
 *       4) Good idea to use defines to represent the buttons purpose. For example this is what we used for the 2012 Lawnmower Competition
 *
 *               #define BOOST BIT(0)
 *               #define CRAWL BIT(1)
 *               #define PAUSE BIT(1)
 *               #define CANCEL BIT(2)
 *               #define HOME BIT(3)
 *               #define DEADMAN BIT(4)
 *               #define BLADE BIT(5)
 *               #define CLEAR BIT(6)
 *               #define GO2TARGETS BIT(7)
 *               #define SAVEHEADING BIT(9)
 *
 *
 *
 */
/******************************************************************/
/********************End Simple Joystick***************************/
/******************************************************************/
