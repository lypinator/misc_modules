/*******************************************************************************
* Copyright (C) Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************
*/
/**
* @file             Coord_Asimov.h
* @brief            
* @version          1.0.0
* @notes            
*****************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef _COORD_ASIMOV_H_    
#define _COORD_ASIMOV_H_

#include "mxc_errors.h"
/***** Definitions *****/ 

//Setting Comments with an asterisks(*) refer to default conditions

struct RobotArm {

    //Need to be measured in terms of coordinate system
    uint16_t Len_BaseToElbow;
    uint16_t Len_ElbowToWrist;
    uint16_t Len_Wrist;
    
    //Calculated once on initilization of coords
    float Len_BaseToElbow_Sqrd;
    float Len_ElbowToWrist_Sqrd;

    //Remember the current Position just in case
    float Base_Angle;
    float Shoulder_Angle;
    float Elbow_Angle;
    float Wrist_Angle;
    float Wrist_Rotation;
    float Wrist_Grab;
};

/***** Global Variables *****/


/***** Function Prototypes *****/

/**
* @brief        Initialize the Coordinate System
*
* @return       
*/

int Init_Coords(uint16_t BaseToElbow, uint16_t ElbowToWrist, uint16_t Wrist);
int Update_Grabber_Position(uint16_t x, uint16_t y, uint16_t z, float Grabber_Angle, float *results);
int Update_WristAngles(uint16_t x,uint16_t y, uint16_t z, float Grabber_Angle, uint16_t *Updated_Coords);
int Update_BaseAngles(uint16_t x, uint16_t y, uint16_t z);



#endif  /* _COORD_ASIMOV_H_ */