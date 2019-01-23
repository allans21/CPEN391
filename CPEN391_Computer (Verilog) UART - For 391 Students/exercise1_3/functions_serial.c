#define RS232_ReceiverFifo        ((volatile unsigned char *)(0xFF210200))
#define RS232_TransmitterFifo     ((volatile unsigned char *)(0xFF210200))
#define RS232_InterruptEnableReg  ((volatile unsigned char *)(0xFF210202))
#define RS232_InterruptIdentificationReg ((volatile unsigned char *)(0xFF210204))
#define RS232_FifoControlReg ((volatile unsigned char *)(0xFF210204))
#define RS232_LineControlReg ((volatile unsigned char *)(0xFF210206))
#define RS232_ModemControlReg ((volatile unsigned char *)(0xFF210208))
#define RS232_LineStatusReg ((volatile unsigned char *)(0xFF21020A))
#define RS232_ModemStatusReg ((volatile unsigned char *)(0xFF21020C))
#define RS232_ScratchReg ((volatile unsigned char *)(0xFF21020E))
#define RS232_DivisorLatchLSB ((volatile unsigned char *)(0xFF210200))
#define RS232_DivisorLatchMSB ((volatile unsigned char *)(0xFF210202))

#define GPS_ReceiverFifo        ((volatile unsigned char *)(0xFF210210))
#define GPS_TransmitterFifo     ((volatile unsigned char *)(0xFF210210))
#define GPS_InterruptEnableReg  ((volatile unsigned char *)(0xFF210212))
#define GPS_InterruptIdentificationReg ((volatile unsigned char *)(0xFF210214))
#define GPS_FifoControlReg ((volatile unsigned char *)(0xFF210214))
#define GPS_LineControlReg ((volatile unsigned char *)(0xFF210216))
#define GPS_ModemControlReg ((volatile unsigned char *)(0xFF210218))
#define GPS_LineStatusReg ((volatile unsigned char *)(0xFF21021A))
#define GPS_ModemStatusReg ((volatile unsigned char *)(0xFF21021C))
#define GPS_ScratchReg ((volatile unsigned char *)(0xFF21021E))
#define GPS_DivisorLatchLSB ((volatile unsigned char *)(0xFF210210))
#define GPS_DivisorLatchMSB ((volatile unsigned char *)(0xFF210212))

#define Bluetooth_ReceiverFifo        ((volatile unsigned char *)(0xFF210220))
#define Bluetooth_TransmitterFifo     ((volatile unsigned char *)(0xFF210220))
#define Bluetooth_InterruptEnableReg  ((volatile unsigned char *)(0xFF210222))
#define Bluetooth_InterruptIdentificationReg ((volatile unsigned char *)(0xFF210224))
#define Bluetooth_FifoControlReg ((volatile unsigned char *)(0xFF210224))
#define Bluetooth_LineControlReg ((volatile unsigned char *)(0xFF210226))
#define Bluetooth_ModemControlReg ((volatile unsigned char *)(0xFF210228))
#define Bluetooth_LineStatusReg ((volatile unsigned char *)(0xFF21022A))
#define Bluetooth_ModemStatusReg ((volatile unsigned char *)(0xFF21022C))
#define Bluetooth_ScratchReg ((volatile unsigned char *)(0xFF21022E))
#define Bluetooth_DivisorLatchLSB ((volatile unsigned char *)(0xFF210220))
#define Bluetooth_DivisorLatchMSB ((volatile unsigned char *)(0xFF210222))

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
/**************************************************************************
/* Subroutine to initialise the RS232 Port by writing some data
** to the internal registers.
** Call this function at the start of the program before you attempt
** to read or write to data via the RS232 port
**
** Refer to UART data sheet for details of registers and programming
***************************************************************************/

#include <stdio.h>

void Init_RS232(void);
int putchar_uart (int  , volatile unsigned char *  ,  volatile unsigned char * );
int getchar_uart( volatile unsigned char *  ,  volatile unsigned char *   );
int TestForReceivedData(volatile unsigned char *  );
void Flush( volatile unsigned char *   , volatile unsigned char *  );

int main()
{
	// initialize the serial port;
	// needs to be done according to the chip
	Init_RS232();
	int i;
	int u;
	// to use just change the RS232 PART, same idea for put char and get char for the arguments
	Flush(RS232_LineStatusReg, RS232_ReceiverFifo);

	int ch_send= 8;
	putchar_uart(8, RS232_LineStatusReg ,  RS232_TransmitterFifo );
	// just to wait 
	for(i=0; i<1000; i++){
		u++;
	}
	if(TestForReceivedData(RS232_LineStatusReg) == TRUE)
		printf("sucess sending ");


	if(getchar_uart(RS232_LineStatusReg, RS232_ReceiverFifo) == ch_send)
		printf("success total");

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
 *RS232_DivisorLatchLSB =0x45;
 *RS232_DivisorLatchMSB =0x01;
 // set bit 7 of Line control register back to 0 and
 // program other bits in that reg for 8 bit data,
 // 1 stop bit, no parity etc
 //0000 0011
 *RS232_LineControlReg= 0x03;
 // Reset the Fifo’s in the FiFo Control Reg by setting bits 1 & 2
*RS232_FifoControlReg = *RS232_FifoControlReg | 0x06;
 // Now Clear all bits in the FiFo control registers
*RS232_FifoControlReg = *RS232_FifoControlReg ^  0x06;
}


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

