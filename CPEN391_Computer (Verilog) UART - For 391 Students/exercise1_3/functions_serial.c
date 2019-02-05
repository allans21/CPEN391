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
void Init_GPS(void);
void Init_Motors(void);
int putchar_uart (int  , volatile unsigned char *  ,  volatile unsigned char * );
int getchar_uart( volatile unsigned char *  ,  volatile unsigned char *   );
int TestForReceivedData(volatile unsigned char *  );
void Flush( volatile unsigned char *   , volatile unsigned char *  );
void Get_GPS_Data(char *latitude, char * longitude, char * time);
void Run_Motors(int);

int main()
{
	// initialize the serial port;


//	Init_GPS();
//	Flush(GPS_LineStatusReg, GPS_ReceiverFifo);
//	char lon[15];
//	char lat[15];
//	char time[15];
//	while(1){
//	Get_GPS_Data(lat, lon, time);
//	printf("\nlatitude: %s\n", lat);
//	printf("longitude: %s\n", lon);
//	printf("time: %s\n", time);
//	}

	Init_Motors();
	Flush(Bluetooth_LineStatusReg, Bluetooth_ReceiverFifo); //tbh probably unessicry
	while(1){
		Run_Motors(1);
		Run_Motors(2);
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
void Init_GPS(void){
	unsigned char line_control_register = *GPS_LineControlReg;
	//set bit 7 to 1 to gain access to baud rate
	line_control_register = line_control_register | 0x80;
	*GPS_LineControlReg = line_control_register;

	//set baud rate to 9600 in hex = 0x0145
	*GPS_DivisorLatchLSB = 0x45;
	*GPS_DivisorLatchMSB = 0x1;

	//set bit 7 of line control back to 0 and set other bits for 8 bit data (pls be right christian)
	*GPS_LineControlReg = 0x03;

	//reset Fifo in fifo control
	*GPS_FifoControlReg = *GPS_FifoControlReg | 0x06;
	//clear all bits in the fifo control register
	*GPS_FifoControlReg = 0x00;
}

//returns gps data as pointers
//only use after INIT_GPS
void Get_GPS_Data(char *latitude, char * longitude, char * time){
	//make an arry that is too big just incase
	char string[70];
	//spin until the start of the correct gps string
	int correct_field = 0;
	int dolla$ = 0;
	int position = 0;
	int commas = 0;

//	while(commas< 6)
//	{
//		char data = (char)getchar_uart(GPS_LineStatusReg, GPS_ReceiverFifo);
//		if(data == '$'){
//			//string[position] = data;
//			dolla$ = 1;
//			//position++;
//		}
//		if(dolla$){
//			if(data == ',')
//				commas++;
//			string[position] = data;
//			position++;
//		}
//	}
//	printf("String: %s\n", string);
	//make sure its $GPGGA
	while(!correct_field){
		char data = (char)getchar_uart(GPS_LineStatusReg, GPS_ReceiverFifo);
		//see if it is the start of the gps string
		//printf("%d\n", data);
		if(data == '$'){
			string[position] = data;
			dolla$ = 1;
			position++;
		}
		//populate field and check to see if gpgga
		if(dolla$){
			string[position] = data;
			//finish reading the fied and see if it is right
			if(position == 5){
				if(string[4] == 'G'){
					correct_field = 1;
				}
				else{
					dolla$ = 0;
					position = -1;
				}
			}
			position++;
		}
	}
	//now have gpgga field
	//6 commas gives up until longitude quadrant
	while(commas < 6){
		char data = (char)getchar_uart(GPS_LineStatusReg, GPS_ReceiverFifo);
		string[position] = data;
		if(data == ',')
			commas++;
		position++;
	}
	//now string contains all nescisarry data in gpgga format
	position = 0;
	commas = 0;
	int lon = 0;
	int lat = 0;
	int t = 0;
	for(position = 0; commas < 6; position ++){
		//parse time data
		printf("%c", string[position]);
		if(string[position] == ',')
			commas++;
		else{
			if(commas == 1){
				time[t] = string[position];
				t++;
			}
			else if(commas == 2){
				latitude[lat] = string[position];
				lat++;
			}
			else if(commas == 3){
				latitude[lat] = string[position];
			}
			else if(commas == 4){
				longitude[lon] = string[position];
				lon++;
			}
			else if(commas == 5){
				longitude[lon] = string[position];
			}
		}

	}





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

