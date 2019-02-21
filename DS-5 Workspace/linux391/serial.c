#include "serial.h"

#define TRUE 1
#define FALSE 0

SerialConf *create_serial_conf(
		volatile void * virtual_base,
		volatile void * ReceiverFifo,
		volatile void * TransmitterFifo,
		volatile void * InterruptEnableReg,
		volatile void * InterruptIdentificationReg,
		volatile void * FifoControlReg,
		volatile void * LineControlReg,
		volatile void * ModemControlReg,
		volatile void * LineStatusReg,
		volatile void * ModemStatusReg,
		volatile void * ScratchReg,
		volatile void * DivisorLatchLSB,
		volatile void * DivisorLatchMSB){
	SerialConf * sc = malloc(sizeof(SerialConf));
	sc->ReceiverFifo               = virtual_base + (unsigned int)(ReceiverFifo - 0xFF200000);
	sc->TransmitterFifo            = virtual_base + (unsigned int)(TransmitterFifo - 0xFF200000);
	sc->InterruptEnableReg         = virtual_base + (unsigned int)(InterruptEnableReg - 0xFF200000);
	sc->InterruptIdentificationReg = virtual_base + (unsigned int)(InterruptIdentificationReg - 0xFF200000);
	sc->FifoControlReg             = virtual_base + (unsigned int)(FifoControlReg - 0xFF200000);
	sc->LineControlReg             = virtual_base + (unsigned int)(LineControlReg - 0xFF200000);
	sc->ModemControlReg            = virtual_base + (unsigned int)(ModemControlReg - 0xFF200000);
	sc->LineStatusReg              = virtual_base + (unsigned int)(LineStatusReg - 0xFF200000);
	sc->ModemStatusReg             = virtual_base + (unsigned int)(ModemStatusReg - 0xFF200000);
	sc->ScratchReg                 = virtual_base + (unsigned int)(ScratchReg - 0xFF200000);
	sc->DivisorLatchLSB            = virtual_base + (unsigned int)(DivisorLatchLSB - 0xFF200000);
	sc->DivisorLatchMSB            = virtual_base + (unsigned int)(DivisorLatchMSB - 0xFF200000);
	return sc;
}


int putchar_uart(int c, SerialConf *sc) {
	// wait for Transmitter Holding Register bit (5) of line status register to be '1'
	// indicating we can write to the device
	printf("%c", c);
	while ( (*(sc->LineStatusReg) & 0x20)!= 0x20){
		;
	}
	*(sc->TransmitterFifo) = (unsigned char)c;
	// write character to Transmitter fifo register
	return c;
	// return the character we printed
}

void putline(SerialConf *sc, char * str) {
	while (*str) {
		putchar_uart(*str, sc);
		str += 1;
	}
	putchar_uart('\n', sc);
}

int getchar_uart(SerialConf *sc)
	{

	while ( (*(sc->LineStatusReg) & 0x01)!= 0x01){
		;
	}
	// wait for Data Ready bit (0) of line status register to be '1'
	// read new character from ReceiverFiFo register
	return (int) *(sc->ReceiverFifo);
	// return new character
}

// the following function polls the UART to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read from the FIFO
int TestForReceivedData(SerialConf *sc)
{
	// if RS232_LineStatusReg bit 0 is set to 1
	if((*(sc->LineStatusReg) & 0x01)== 0x01)
		return TRUE;
	else
		return FALSE;
	//return TRUE, otherwise return FALSE
}

//
// Remove/flush the UART receiver buffer by removing any unread characters
//
void Flush(SerialConf *sc)
{
	// while bit 0 of Line Status Register == ‘1’
	// read unwanted char out of fifo receiver buffer
	// return; // no more characters so return
	int garbage;
	while ( (*(sc->LineStatusReg) & 0x01)== 0x01){
		garbage = *(sc->ReceiverFifo);
	}
}
