//The host should be configured for 9600 BAUD, 8 data bits and 1 Stop bit.
#include <stdio.h>
#include <math.h>
/*****************************************************************************
** Initialise touch screen controller
*****************************************************************************/
void Init_Touch(void)
{
 //Program serial port to communicate with touchscreen
 // send touchscreen controller an "enable touch" command
}
/*****************************************************************************
** test if screen touched
*****************************************************************************/
int ScreenTouched( void )
{
 // return TRUE if any data received from serial pory connected to
 // touchscreen or FALSE otherwise
    Char test = getchar();
    if(test == 0x80){
        return 1;
    }else{
        Flush();
        return 0;
    }
}
int ScreenReleased( void )
{
 // return TRUE if any data received from serial pory connected to
 // touchscreen or FALSE otherwise
    Char test = getchar();
    if(test == 0x81){
        return 1;
    }else{
        Flush();
        return 0;
    }
}
/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void WaitForReleased()
{
while(!ScreenReleased());
}
/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void WaitForTouch()
{
while(!ScreenTouched());
}
/* a data type to hold a point/coord */
typedef struct { int x, y; } Point ;
/*****************************************************************************
* This function waits for a touch screen press event and returns X,Y coord
*****************************************************************************/
Point GetPress(void)
{
 Point p1;
 // wait for a pen down command then return the X,Y coord of the point
 // calibrated correctly so that it maps to a pixel on screen
 WaitForTouch();
 Char x1 = getchar();
 Char x2 = getchar();
 Char y1 = getchar();
 Char y2 = getchar();
 float x = x1+x2<<7;
 x = x * 320/4096;
 float y = y1+y2<<7;
 y = y * 240/4096;
 p1 = new Point(round(x),round(y));
 return p1;
}
/*****************************************************************************
* This function waits for a touch screen release event and returns X,Y coord
*****************************************************************************/
Point GetRelease(void)
{
 Point p1;
 // wait for a pen up command then return the X,Y coord of the point
 // calibrated correctly so that it maps to a pixel on screen
 Char x1 = getchar();
 Char x2 = getchar();
 Char y1 = getchar();
 Char y2 = getchar();
 float x = x1+x2<<7;
 x = x * 320/4096;
 float y = y1+y2<<7;
 y = y * 240/4096;
 p1 = new Point(round(x),round(y));
 return p1;
}