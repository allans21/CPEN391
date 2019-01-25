#include <stdio.h>

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
#define TRUE 1
#define FALSE 0

void Init_BT(void);
int putchar_uart (int  , volatile unsigned char *  ,  volatile unsigned char * );
int getchar_uart( volatile unsigned char *  ,  volatile unsigned char *   );
int TestForReceivedData(volatile unsigned char *  );
void Flush( volatile unsigned char *   , volatile unsigned char *  );
void BTFactoryReset(void);
void BTOutMessage(char ** Message);

int main()
{

    int iterator, i;
	Init_BT();
	BTFactoryReset();

}

void BTFactoryReset(void)
{
// wait for 1 second between command
// enter these commands in upper case
// $$$ enter command mode
// SF,1 factory reset
// SN,Device1 set device name to “Device1”
// SP,1234 set 4 digit pin to “1234”
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
 // Reset the Fifo’s in the FiFo Control Reg by setting bits 1 & 2
*Bluetooth_FifoControlReg = *Bluetooth_FifoControlReg | 0x06;
 // Now Clear all bits in the FiFo control registers
*Bluetooth_FifoControlReg = *Bluetooth_FifoControlReg ^  0x06;
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
