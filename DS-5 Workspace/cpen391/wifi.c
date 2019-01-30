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
  garbage =  *ReceiverFifo;
}

}


// END COPY PASTE



#include <stdio.h>

int exec_lua(char*, char*);

int main() {
	Init_RS232();
	Flush(RS232_LineStatusReg, RS232_ReceiverFifo);
	char buf[1024] = "";

	printf("Sending to dangle dongle\n");
	int res = exec_lua("dofile(\"send_text_message.lua\")", buf);
	printf("%s\n", buf);

	char c;
	int got=0;
	char *expected = "connected";
	while (got < 9) {
		while(TestForReceivedData(RS232_LineStatusReg) != 1) {;}
		c = (char) getchar_uart(RS232_LineStatusReg, RS232_ReceiverFifo);
		printf("%c", c);
		if (c == expected[got]) {
			got += 1;
		}
	}
	printf("\n");

	res = exec_lua("send_sms(\"(204) 818-9230\",\"(778) 866-2529\",\"WiFi Chip ON!\")", buf);
	printf("%s\n", buf);

	while (1) {
		if(TestForReceivedData(RS232_LineStatusReg) == 1) {
			c = (char) getchar_uart(RS232_LineStatusReg, RS232_ReceiverFifo);
			printf("%c", c);
		}
	}
}

int exec_lua(char * str, char * res) {
	int i;
	int bytes_received = 0;
	printf("executing %s as a Lua command\n", str);
	while (*str) {
		for(i = 0; i < 1000; i++) {;}
		putchar_uart(*str, RS232_LineStatusReg, RS232_TransmitterFifo);
		str += 1;
		for(i = 0; i < 1000; i++) {;}
		if(TestForReceivedData(RS232_LineStatusReg) == 1) {
			res[bytes_received++] = (char) getchar_uart(RS232_LineStatusReg, RS232_ReceiverFifo);
		}
	}
	putchar_uart('\n', RS232_LineStatusReg , RS232_ReceiverFifo);
    for(i = 0; i < 200000; i++) {
        if(TestForReceivedData(RS232_LineStatusReg) == 1) {
        	res[bytes_received++] = (char) getchar_uart(RS232_LineStatusReg, RS232_ReceiverFifo);
            i = 0 ; // reset timer if we got something back
        }
    }
	printf("Got %u bytes\n", bytes_received);
	res[bytes_received] = '\0';
	return bytes_received;
}
