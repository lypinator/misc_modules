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
* @file             Coord_Asimov.c
* @brief            
* @version          1.0.0
* @notes            
*****************************************************************************/

#include <stdio.h>
#include <math.h>
#include "Coord_Asimov.h"
#include "mxc_delay.h"



/*****  Global Variables *****/  
float PI = 22.0/7.0;
float Distance_Flat = 0.0;
float Distance_Adjusted = 0.0;
float Distance_Adjusted_Sqrd = 0.0;

struct RobotArm Asimov = {

    //Need to be measured in terms of coordinate system
    .Len_BaseToElbow = 0,
    .Len_ElbowToWrist = 0,
    .Len_Wrist = 0,
    
    //Calculated once on initilization of coords
    .Len_BaseToElbow_Sqrd = 0.0,
    .Len_ElbowToWrist_Sqrd = 0.0,

    //Remember the current Position just in case
    .Base_Angle = 0.0,
    .Shoulder_Angle = 0.0,
    .Elbow_Angle = 0.0,
    .Wrist_Angle = 0.0,
    .Wrist_Rotation = 0.0,
    .Wrist_Grab = 0.0
};


/***** Function Prototypes *****/
int Update_Grabber_Position(uint16_t x, uint16_t y, uint16_t z, float Grabber_Angle, float *results);
int Update_WristAngle(uint16_t x,uint16_t y, uint16_t z, float Grabber_Angle, uint16_t *Updated_Coords);
int Update_BaseAngle(uint16_t x,uint16_t y, uint16_t z);
int Update_ElbowAngle(uint16_t *Updated_Coords);
int Update_ShoulderAngle(uint16_t *Updated_Coords);

float Base_Angle_Calc(uint16_t x);
float Shoulder_Angle_Calc(uint16_t z);
float Elbow_Angle_Calc(void);

float Distance_Calc(uint16_t x, uint16_t y, uint16_t z);
float RadToDeg(float Radians);
float DegToRad(float Degrees);
int Init_Coords(uint16_t BaseToElbow, uint16_t ElbowToWrist, uint16_t Wrist);


/***** Driver implementation *****/





int Update_Grabber_Position(uint16_t x, uint16_t y, uint16_t z, float Grabber_Angle, float *results){
    uint16_t Updated_Coords[3];
    
    //Find the base angle
    Update_BaseAngle(x,y,z);
    
    //Update coordinates based on grabbing angle
    Update_WristAngle(x, y,  z, Grabber_Angle, Updated_Coords);

    //Update the Distances
    Distance_Flat = Distance_Calc(Updated_Coords[0], Updated_Coords[1], 0.0);
    Distance_Adjusted = Distance_Calc(Updated_Coords[0], Updated_Coords[1], Updated_Coords[2]);
    Distance_Adjusted_Sqrd = Distance_Adjusted*Distance_Adjusted;

    Update_ShoulderAngle(Updated_Coords);

    Update_ElbowAngle(Updated_Coords);

    //Update arm structure
    results[0] = Asimov.Base_Angle;
    results[1] = Asimov.Shoulder_Angle;
    results[2] = Asimov.Elbow_Angle;
    results[3] = Asimov.Wrist_Angle;
    results[4] = Asimov.Wrist_Rotation;
    results[5] = Asimov.Wrist_Grab;

}

int Update_WristAngle(uint16_t x,uint16_t  y, uint16_t z, float Grabber_Angle, uint16_t *Updated_Coords){

    //Variable to hold the adjustment measurment
    uint16_t Delta_X, Delta_Y, Delta_Z;
    float Delta_XY;

    //Have to figure this one out a litte...
    if (Grabber_Angle > 90){
        Asimov.Wrist_Angle = 0.0;
    }
    else{
        Asimov.Wrist_Angle = 90.0 - Grabber_Angle;

        Delta_Z = (uint16_t)round(Asimov.Len_Wrist*cosf(DegToRad(Asimov.Wrist_Angle)));
    
        Delta_XY = Asimov.Len_Wrist * sinf(DegToRad(Asimov.Wrist_Angle));

        Delta_X = (uint16_t)round(Delta_XY * sinf(DegToRad(90.0-Asimov.Base_Angle)));
        Delta_Y = (uint16_t)round(Delta_XY * cosf(DegToRad(90.0-Asimov.Base_Angle)));
    }    

    Updated_Coords[0] = x-Delta_X;
    Updated_Coords[1] = y-Delta_Y;
    Updated_Coords[2] = z+Delta_Z;

    return(E_NO_ERROR);
}


int Update_ElbowAngle(uint16_t *Updated_Coords){
    uint16_t x = Updated_Coords[0];
    uint16_t y = Updated_Coords[1]; 
    uint16_t z = Updated_Coords[2];

    
    //Update Elbow Angle
    Asimov.Elbow_Angle = Elbow_Angle_Calc();

    return(E_NO_ERROR);
}

int Update_ShoulderAngle(uint16_t *Updated_Coords){
    uint16_t x = Updated_Coords[0];
    uint16_t y = Updated_Coords[1]; 
    uint16_t z = Updated_Coords[2];
    //Update Shoulder Angle
    Asimov.Shoulder_Angle = Shoulder_Angle_Calc(z); 

    return(E_NO_ERROR);
}

int Update_BaseAngle(uint16_t x,uint16_t y, uint16_t z){
    Distance_Flat = Distance_Calc(x, y, 0.0);

    //Update Base Angle
    Asimov.Base_Angle = Base_Angle_Calc(x);
    return(E_NO_ERROR);
}



int Init_Coords(uint16_t BaseToElbow, uint16_t ElbowToWrist, uint16_t Wrist){
    
    //Initialize the arm dimensions
    Asimov.Len_BaseToElbow = BaseToElbow;
    Asimov.Len_ElbowToWrist = ElbowToWrist;
    Asimov.Len_Wrist = Wrist;

    //Calculate squared values for faster computation
    Asimov.Len_BaseToElbow_Sqrd = BaseToElbow*BaseToElbow;
    Asimov.Len_ElbowToWrist_Sqrd = ElbowToWrist*ElbowToWrist;

    return (E_NO_ERROR);
}

//Independent of Z coord
float Base_Angle_Calc(uint16_t x){
    float temp = (float)acosf(((float)x/Distance_Flat));

    return(RadToDeg(temp));
}

float Shoulder_Angle_Calc(uint16_t z){
    float Theta_Z = 0.0;
    float Theta_Shoulder = 0.0;

    //Calculate the Z component of theta
    if(z) Theta_Z = acosf((Distance_Flat/Distance_Adjusted));
    else Theta_Z = 0;

    //Calculate Shoulder component using SSS formula of triangle
    Theta_Shoulder = acosf(((Distance_Adjusted_Sqrd+Asimov.Len_BaseToElbow_Sqrd-Asimov.Len_ElbowToWrist_Sqrd)/(2*Asimov.Len_BaseToElbow*Distance_Adjusted)));

    return(RadToDeg(Theta_Z+Theta_Shoulder));
}


float Elbow_Angle_Calc(){
    return(RadToDeg(acosf(((Asimov.Len_BaseToElbow_Sqrd+Asimov.Len_ElbowToWrist_Sqrd-Distance_Adjusted_Sqrd)/(2*Asimov.Len_BaseToElbow*Asimov.Len_ElbowToWrist)))));
}


float Distance_Calc(uint16_t x, uint16_t y, uint16_t z){
    return((float)sqrt(((double)((x*x)+(y*y)+(z*z)))));
}

float RadToDeg(float Radians){
    return(Radians*180.0/PI);
}

float DegToRad(float Degrees){
    return(Degrees*PI/180.0);
}