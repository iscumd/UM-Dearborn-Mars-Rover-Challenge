#ifndef __NASAROBOTEQ_H__
#define __NASAROBOTEQ_H__
/*
 * This file is common to most development.
 * If you make any changes here, it will affect everyone.
 * Please avoid making changes to this file.
 */
extern int roboteqPort;
extern int rightChannelNum;
extern int leftChannelNum;

void initRoboteq();
void sendspeed(int leftspeed, int rightspeed);
#endif // __ROBOTEQ_H__
