
#include "serial_ports.h"
#include <stdio.h>
#include <math.h>
// reset the Bluetooth reset and send a char
void BTFactoryReset(void)
{
  // wait for 1 second between command
  // enter these commands in upper case
  // $$$ enter command mode
  // SF,1 factory reset
  // SN,Device1 set device name to â€œDevice1â€�
  // SP,1234 set 4 digit pin to â€œ1234â€�
  // R,1<CR> reboot BT controller
  char c, Message[100] ;
  int i;
  while(1){
    printf("\r\nEnter Message for Bluetooth Controller:") ;
    gets(Message); // get command string from user keyboard
    //BTOutMessage(&Message) ; // write string to BT device
    printf("THE Message IS : %s \n", Message);
    int iterator= 0;
    while (Message[iterator] != '\0') {
      putchar_uart(Message[iterator], Bluetooth_LineStatusReg , Bluetooth_TransmitterFifo) ;
      iterator++;
    }
    // if the command string was NOT "$$$" send \r\n
    if(strcmp(Message, "$$$") != 0) { // $$$ puts BT module into command mode
      putchar_uart('\r', Bluetooth_LineStatusReg , Bluetooth_TransmitterFifo) ;
      putchar_uart('\n',  Bluetooth_LineStatusReg , Bluetooth_TransmitterFifo ) ;
    }
    // now read back acknowledge string from device and display on console,
    // will timeout after no communication for about 2 seconds
    for(i = 0; i < 2000000; i ++) {
      if(TestForReceivedData(Bluetooth_LineStatusReg) == 1) {
        c = getchar_uart(Bluetooth_LineStatusReg ,   Bluetooth_ReceiverFifo) ;
        printf("%c", c);
        i=0 ; // reset timer if we got something back
      }
    }
  }
}

// send a message out to the Bluetooth chip

void BTOutMessage(char ** Message){
  int iterator=0;
  //while(iterator<100 || Message[iterator]!= NULL){
  //	putchar_uart(*Message[iterator],Bluetooth_LineStatusReg , Bluetooth_TransmitterFifo);
  //	iterator++;
  //}
  while(iterator<100 || Message[iterator]!= NULL){
    //	putchar_uart(*Message[iterator],Bluetooth_LineStatusReg , Bluetooth_TransmitterFifo);
    //	iterator++;
    printf("%c", Message[iterator] );
    iterator ++;
  }

}

// instantiate the Bluetooth chip
void Init_BT(void)
{

  // set bit 7 of Line Control Register to 1, to gain access to the baud rate registers
  unsigned char line_control_register;
  line_control_register= *Bluetooth_LineControlReg;
  line_control_register = line_control_register |  0x80;
  *Bluetooth_LineControlReg= line_control_register;
  // set Divisor latch (LSB and MSB) with correct value for required baud rate
  // example 0x0145
  *Bluetooth_DivisorLatchLSB =0x1B;
  *Bluetooth_DivisorLatchMSB =0x00;
  // set bit 7 of Line control register back to 0 and
  // program other bits in that reg for 8 bit data,
  // 1 stop bit, no parity etc
  //0000 0011
  *Bluetooth_LineControlReg= 0x03;
  // Reset the Fifoâ€™s in the FiFo Control Reg by setting bits 1 & 2
  *Bluetooth_FifoControlReg = *Bluetooth_FifoControlReg | 0x06;
  // Now Clear all bits in the FiFo control registers
  *Bluetooth_FifoControlReg = *Bluetooth_FifoControlReg ^  0x06;
}

// send a character via the uart port

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
// get a character via the uart port
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
// instantiate the touchscreen chip

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
* get the point from a released touch
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

//
// Remove/flush the UART receiver buffer by removing any unread characters
//
void Flush( volatile unsigned char *  LineStatusReg , volatile unsigned char *  ReceiverFifo)
{

 // while bit 0 of Line Status Register == â€˜1â€™
 // read unwanted char out of fifo receiver buffer
 // return; // no more characters so return
 int garbage;
while ( (*LineStatusReg & 0x01)== 0x01){
  garbage=  * ReceiverFifo;
}

}
void Init_RS232(void)
{

// set bit 7 of Line Control Register to 1, to gain access to the baud rate registers
unsigned char line_control_register;
line_control_register= *RS232_LineControlReg;
line_control_register = line_control_register |  0x80;
*RS232_LineControlReg= line_control_register;
 // set Divisor latch (LSB and MSB) with correct value for required baud rate
// example 0x0145
// *Bluetooth_DivisorLatchLSB =0x1B;
// *Bluetooth_DivisorLatchMSB =0x00;
// *RS232_DivisorLatchLSB =0x45;
// *RS232_DivisorLatchMSB =0x01;
 *RS232_DivisorLatchLSB = 0x1B;
 *RS232_DivisorLatchMSB = 0x00;
 // set bit 7 of Line control register back to 0 and
 // program other bits in that reg for 8 bit data,
 // 1 stop bit, no parity etc
 //0000 0011
 *RS232_LineControlReg= 0x03;
 // Reset the Fifoâ€™s in the FiFo Control Reg by setting bits 1 & 2
*RS232_FifoControlReg = *RS232_FifoControlReg | 0x06;
 // Now Clear all bits in the FiFo control registers
*RS232_FifoControlReg = *RS232_FifoControlReg ^  0x06;
}
/// motors functions

void Init_Motors(void){
	//hijacking the Buetooth serial because we are not using that for now
	unsigned char line_control_register = *Bluetooth_LineControlReg;
	line_control_register = line_control_register | 0x80;
	*Bluetooth_LineControlReg = line_control_register;

	*Bluetooth_DivisorLatchLSB = 0x45;
	*Bluetooth_DivisorLatchMSB = 0x1;

	*Bluetooth_LineControlReg = 0x03;

	*Bluetooth_FifoControlReg = *Bluetooth_FifoControlReg | 0x06;
	*Bluetooth_FifoControlReg = 0x00;

}
void Run_Motors(int motor){
	if(motor == 1)
		putchar_uart(97,  Bluetooth_LineStatusReg ,  Bluetooth_TransmitterFifo);
	else if(motor == 2)
		putchar_uart(98,  Bluetooth_LineStatusReg ,  Bluetooth_TransmitterFifo);




}
