
#include "serial_ports.h"
#include "serial.h"
#include <stdio.h>
#include <math.h>


SerialConf* Init_BT(void *virtual_base)
{
	SerialConf *sc = create_serial_conf(virtual_base,
		Bluetooth_ReceiverFifo, Bluetooth_TransmitterFifo,
		Bluetooth_InterruptEnableReg, Bluetooth_InterruptIdentificationReg,
		Bluetooth_FifoControlReg, Bluetooth_LineControlReg, Bluetooth_ModemControlReg,
		Bluetooth_LineStatusReg, Bluetooth_ModemStatusReg, Bluetooth_ScratchReg,
		Bluetooth_DivisorLatchLSB, Bluetooth_DivisorLatchMSB);

	// set bit 7 of Line Control Register to 1, to gain access to the baud rate registers
	unsigned char line_control_register;
	line_control_register = *(sc->LineControlReg);
	line_control_register = line_control_register |  0x80;
	*(sc->LineControlReg) = line_control_register;
	*(sc->DivisorLatchLSB) = 0x1B;
	*(sc->DivisorLatchMSB) = 0x00;
	// set bit 7 of Line control register back to 0 and
	// program other bits in that reg for 8 bit data,
	// 1 stop bit, no parity etc
	//0000 0011
	*(sc->LineControlReg) = 0x03;
	// Reset the Fifo’s in the FiFo Control Reg by setting bits 1 & 2
	*(sc->FifoControlReg) = *(sc->FifoControlReg) | 0x06;
	// Now Clear all bits in the FiFo control registers
	*(sc->FifoControlReg) = *(sc->FifoControlReg) ^  0x06;
	return sc;
}


SerialConf * Init_Touch(void * virtual_base)
{
  //Program serial port to communicate with touchscreen
  // send touchscreen controller an "enable touch" command

	SerialConf *sc = create_serial_conf(virtual_base,
		TouchScreen_ReceiverFifo, TouchScreen_TransmitterFifo,
		TouchScreen_InterruptEnableReg, TouchScreen_InterruptIdentificationReg,
		TouchScreen_FifoControlReg, TouchScreen_LineControlReg, TouchScreen_ModemControlReg,
		TouchScreen_LineStatusReg, TouchScreen_ModemStatusReg, TouchScreen_ScratchReg,
		TouchScreen_DivisorLatchLSB, TouchScreen_DivisorLatchMSB);

	// set bit 7 of Line Control Register to 1, to gain access to the baud rate registers
	unsigned char line_control_register;
	line_control_register = *(sc->LineControlReg);
	line_control_register = line_control_register |  0x80;
	*(sc->LineControlReg) = line_control_register;
	*(sc->DivisorLatchLSB) = 0x1B;
	*(sc->DivisorLatchMSB) = 0x00;
	// set bit 7 of Line control register back to 0 and
	// program other bits in that reg for 8 bit data,
	// 1 stop bit, no parity etc
	//0000 0011
	*(sc->LineControlReg) = 0x03;
	// Reset the Fifo’s in the FiFo Control Reg by setting bits 1 & 2
	*(sc->FifoControlReg) = *(sc->FifoControlReg) | 0x06;
	// Now Clear all bits in the FiFo control registers
	*(sc->FifoControlReg) = *(sc->FifoControlReg) ^  0x06;

	putchar_uart(0x55, sc);
	putchar_uart(0x01, sc);
    putchar_uart(0x12, sc);
	return sc;
/*
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
*/
}
/*****************************************************************************
** test if screen touched
*****************************************************************************/
int ScreenTouched( SerialConf *sc )
{
  // return TRUE if any data received from serial pory connected to
  // touchscreen or FALSE otherwise
  if(TestForReceivedData(sc)){

    char test = getchar_uart(sc);
    //Flush( TouchScreen_LineStatusReg ,  TouchScreen_ReceiverFifo);
      return ((test>>7)%2 == 1 && (test%2) == 1); //pen down

  }
  return FALSE;
  

}
int ScreenReleased(SerialConf *sc)
{
  // return TRUE if any data received from serial pory connected to
  // touchscreen or FALSE otherwise
  char test = getchar_uart(sc);
  return ((test>>7)%2 == 1 && (test%2) == 0); //pen down
}
/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void WaitForReleased(SerialConf *sc)
{
  while(!ScreenReleased(sc));
}
/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void WaitForTouch(SerialConf *sc)
{
  while(!ScreenTouched(sc));
}
/*****************************************************************************
* This function waits for a touch screen press event and returns X,Y coord
*****************************************************************************/
Point GetPress(SerialConf *sc)
{
  Point p1;
  // wait for a pen down command then return the X,Y coord of the point
  // calibrated correctly so that it maps to a pixel on screen
  WaitForTouch(sc);
  char x1 = getchar_uart(sc);
  char x2 = getchar_uart(sc);
  char y1 = getchar_uart(sc);
  char y2 =  getchar_uart(sc);
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
Point GetRelease(SerialConf *sc)
{
  WaitForReleased(sc);
  Point p1;
  // wait for a pen up command then return the X,Y coord of the point
  // calibrated correctly so that it maps to a pixel on screen
  char x1 = getchar_uart(sc);
  char x2 = getchar_uart(sc);
  char y1 = getchar_uart(sc);
  char y2 =  getchar_uart(sc);
  float x = x1|(x2<<7);
  x = x * 800/4096;
  float y = y1| ( y2<<7 );
  y = y * 480/4096;
  p1.x=x;
  p1.y=y;
  return p1;
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
 // Reset the FifoÃ¢â‚¬â„¢s in the FiFo Control Reg by setting bits 1 & 2
*RS232_FifoControlReg = *RS232_FifoControlReg | 0x06;
 // Now Clear all bits in the FiFo control registers
*RS232_FifoControlReg = *RS232_FifoControlReg ^  0x06;
}
/// motors functions

SerialConf * Init_Motors(void * virtual_base){
	SerialConf *sc = create_serial_conf(virtual_base,
		Bluetooth_ReceiverFifo, Bluetooth_TransmitterFifo,
		Bluetooth_InterruptEnableReg, Bluetooth_InterruptIdentificationReg,
		Bluetooth_FifoControlReg, Bluetooth_LineControlReg, Bluetooth_ModemControlReg,
		Bluetooth_LineStatusReg, Bluetooth_ModemStatusReg, Bluetooth_ScratchReg,
		Bluetooth_DivisorLatchLSB, Bluetooth_DivisorLatchMSB);

	// set bit 7 of Line Control Register to 1, to gain access to the baud rate registers
	unsigned char line_control_register;
	line_control_register = *(sc->LineControlReg);
	line_control_register = line_control_register |  0x80;
	*(sc->LineControlReg) = line_control_register;
	*(sc->DivisorLatchLSB) = 0x45;
	*(sc->DivisorLatchMSB) = 0x01;
	// set bit 7 of Line control register back to 0 and
	// program other bits in that reg for 8 bit data,
	// 1 stop bit, no parity etc
	//0000 0011
	*(sc->LineControlReg) = 0x03;
	// Reset the Fifo’s in the FiFo Control Reg by setting bits 1 & 2
	*(sc->FifoControlReg) = *(sc->FifoControlReg) | 0x06;
	// Now Clear all bits in the FiFo control registers
	*(sc->FifoControlReg) = 0;

	return sc;

/*
	// TODO MEMORY MAP THIS
	//hijacking the Buetooth serial because we are not using that for now
	unsigned char line_control_register = *Bluetooth_LineControlReg;
	line_control_register = line_control_register | 0x80;
	*Bluetooth_LineControlReg = line_control_register;

	*Bluetooth_DivisorLatchLSB = 0x45;
	*Bluetooth_DivisorLatchMSB = 0x1;

	*Bluetooth_LineControlReg = 0x03;

	*Bluetooth_FifoControlReg = *Bluetooth_FifoControlReg | 0x06;
	*Bluetooth_FifoControlReg = 0x00;
*/
}

void Run_Motors(int motor, SerialConf *sc){
	if(motor == 1)
		putchar_uart(97,  sc);
	else if(motor == 2)
		putchar_uart(98, sc);
}

