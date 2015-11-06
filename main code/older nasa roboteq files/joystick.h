#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__
/*
 * structure to hold joystick data
 */

struct JoystickData {

    int num_of_axis;
    int num_of_buttons;
    char joystick_name[80];
    char button[20];
    int buttons;
    double axis[20];
    int hat;
};
#define AXISRANGE 32767.0

/*
 * There is no reason why we can't have more than one joy stick.
 * However, to keep the code simple we read only one joystick.
 *
 */

int joystick();//check if joystick is present and pool data.
extern struct JoystickData joy0;

#endif //__JOYSTICK_H__
