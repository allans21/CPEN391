/// /////
////
////
//The host should be configured for 9600 BAUD, 8 data bits and 1 Stop bit.
#include <stdio.h>
#include <math.h>
#define TouchScreen_ReceiverFifo        ((volatile unsigned char *)(0xFF210230))
#define TouchScreen_TransmitterFifo     ((volatile unsigned char *)(0xFF210230))
#define TouchScreen_InterruptEnableReg  ((volatile unsigned char *)(0xFF210232))
#define TouchScreen_InterruptIdentificationReg ((volatile unsigned char *)(0xFF210234))
#define TouchScreen_FifoControlReg ((volatile unsigned char *)(0xFF210234))
#define TouchScreen_LineControlReg ((volatile unsigned char *)(0xFF210236))
#define TouchScreen_ModemControlReg ((volatile unsigned char *)(0xFF210238))
#define TouchScreen_LineStatusReg ((volatile unsigned char *)(0xFF21023A))
#define TouchScreen_ModemStatusReg ((volatile unsigned char *)(0xFF21023C))
#define TouchScreen_ScratchReg ((volatile unsigned char *)(0xFF21023E))
#define TouchScreen_DivisorLatchLSB ((volatile unsigned char *)(0xFF210230))
#define TouchScreen_DivisorLatchMSB ((volatile unsigned char *)(0xFF210232))
#define TRUE 1
#define FALSE 0
/* a data type to hold a point/coord */
typedef struct { int x, y; } Point ;
void Init_BT(void);
int ScreenTouched( void );
int putchar_uart (int  , volatile unsigned char *  ,  volatile unsigned char * );
int getchar_uart( volatile unsigned char *  ,  volatile unsigned char *   );
int TestForReceivedData(volatile unsigned char *  );
void Flush( volatile unsigned char *  LineStatusReg , volatile unsigned char *  ReceiverFifo);
void Init_Touch(void);
void WaitForReleased(void);
int ScreenTouched( void );
int ScreenReleased( void );
Point GetRelease(void);
Point GetPress(void);
void WaitForTouch(void);
void WaitForReleased(void);
/*****************************************************************************
** Initialise touch screen controller
*****************************************************************************/
void main () {
    Init_Touch();
    Point touch;
    while(1) {

      WaitForTouch();
      WaitForReleased();
      touch=  GetRelease();
      printf("sucess\n");
      printf("x axis: %d", touch.x);
      printf("y axis: %d", touch.y);
      printf("\n");
      Flush( TouchScreen_LineStatusReg , TouchScreen_ReceiverFifo);




    }

}
void Init_Touch(void)
{
 //Program serial port to communicate with touchscreen
 // send touchscreen controller an "enable touch" command

 unsigned char line_control_register;
 line_control_register= *TouchScreen_LineControlReg;
 line_control_register = line_control_register |  0x80;
 *TouchScreen_LineControlReg= line_control_register;
  // set Divisor latch (LSB and MSB) with correct value for required baud rate
 // example 0x0145
  *TouchScreen_DivisorLatchLSB =0x45;
  *TouchScreen_DivisorLatchMSB =0x01;
  // set bit 7 of Line control register back to 0 and
  // program other bits in that reg for 8 bit data,
  // 1 stop bit, no parity etc
  //0000 0011
  *TouchScreen_LineControlReg= 0x03;
  // Reset the Fifosin the FiFo Control Reg by setting bits 1 & 2
 *TouchScreen_FifoControlReg = *TouchScreen_FifoControlReg | 0x06;
  // Now Clear all bits in the FiFo control registers
 *TouchScreen_FifoControlReg = *TouchScreen_FifoControlReg ^  0x06;
 // send char to intialize the Device1
 // synch, send size and command to enable
 putchar_uart(0x55, TouchScreen_LineStatusReg ,  TouchScreen_TransmitterFifo);
 putchar_uart(0x01, TouchScreen_LineStatusReg ,  TouchScreen_TransmitterFifo);
 putchar_uart(0x12, TouchScreen_LineStatusReg ,  TouchScreen_TransmitterFifo);

}
/*****************************************************************************
** test if screen touched
*****************************************************************************/
int ScreenTouched( void )
{
 // return TRUE if any data received from serial pory connected to
 // touchscreen or FALSE otherwise
    char test = getchar_uart( TouchScreen_LineStatusReg ,  TouchScreen_ReceiverFifo );
return ((test>>7)%2 == 1 && (test%2) == 1); //pen down

}
int ScreenReleased( void )
{
 // return TRUE if any data received from serial pory connected to
 // touchscreen or FALSE otherwise
  char test = getchar_uart( TouchScreen_LineStatusReg ,  TouchScreen_ReceiverFifo );
  return ((test>>7)%2 == 1 && (test%2) == 0); //pen down
}
/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void WaitForReleased(void)
{
while(!ScreenReleased());
}
/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void WaitForTouch(void)
{
while(!ScreenTouched());
}

/*****************************************************************************
* This function waits for a touch screen press event and returns X,Y coord
*****************************************************************************/
Point GetPress(void)
{
 Point p1;
  // wait for a pen down command then return the X,Y coord of the point
 // calibrated correctly so that it maps to a pixel on screen
 WaitForTouch();
 char x1 = getchar_uart( TouchScreen_LineStatusReg ,  TouchScreen_ReceiverFifo );
 char x2 = getchar_uart( TouchScreen_LineStatusReg ,  TouchScreen_ReceiverFifo );
 char y1 = getchar_uart( TouchScreen_LineStatusReg ,  TouchScreen_ReceiverFifo );
 char y2 =  getchar_uart( TouchScreen_LineStatusReg ,  TouchScreen_ReceiverFifo );
 float x = x1|(x2<<7);
 x = x * 800/4096;
 float y = y1|(y2<<7);
 y = y * 480/4096;
 p1.x=x;
 p1.y=y;
 return p1;
}
/*****************************************************************************
* This function waits for a touch screen release event and returns X,Y coord
*****************************************************************************/
Point GetRelease(void)
{
  WaitForReleased();
 Point p1;
 // wait for a pen up command then return the X,Y coord of the point
 // calibrated correctly so that it maps to a pixel on screen
 char x1 = getchar_uart( TouchScreen_LineStatusReg ,  TouchScreen_ReceiverFifo );
 char x2 = getchar_uart( TouchScreen_LineStatusReg ,  TouchScreen_ReceiverFifo );
 char y1 = getchar_uart( TouchScreen_LineStatusReg ,  TouchScreen_ReceiverFifo );
 char y2 =  getchar_uart( TouchScreen_LineStatusReg ,  TouchScreen_ReceiverFifo );
 float x = x1|(x2<<7);
 x = x * 800/4096;
 float y = y1| ( y2<<7 );
 y = y * 480/4096;
  p1.x=x;
 p1.y=y;
 return p1;
}

// send character

int putchar_uart(int c, volatile unsigned char *  LineStatusReg ,  volatile unsigned char *  TransmitterFifo)
{
 // wait for Transmitter Holding Register bit (5) of line status register to be '1'
 // indicating we can write to the device
 while ( (*LineStatusReg & 0x20)!= 0x20){
;
 }
 *TransmitterFifo = (unsigned char)c;
// write character to Transmitter fifo register
return c;
// return the character we printed

}


int getchar_uart( volatile unsigned char *  LineStatusReg ,  volatile unsigned char *  ReceiverFifo )
{

  while ( (*LineStatusReg & 0x01)!= 0x01){

 ;
  }
 // wait for Data Ready bit (0) of line status register to be '1'
 // read new character from ReceiverFiFo register
 return (int) *ReceiverFifo;
 // return new character
}
// the following function polls the UART to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read from the FIFO
int TestForReceivedData(volatile unsigned char *  LineStatusReg)
{
 // if RS232_LineStatusReg bit 0 is set to 1
 if((*LineStatusReg & 0x01)== 0x01)
 return TRUE;
 else
 return FALSE;
 //return TRUE, otherwise return FALSE
}
//
// Remove/flush the UART receiver buffer by removing any unread characters
//
void Flush( volatile unsigned char *  LineStatusReg , volatile unsigned char *  ReceiverFifo)
{

 // while bit 0 of Line Status Register == ‘1’
 // read unwanted char out of fifo receiver buffer
 // return; // no more characters so return
 int garbage;
while ( (*LineStatusReg & 0x01)== 0x01){
  garbage=  * ReceiverFifo;
}

}
