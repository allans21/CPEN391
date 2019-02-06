#include <stdio.h>
#include "wifi.h"

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


/*
 * This is similar to the other initialization EXCEPT it uses the SerialConf struct
 * and create_serial_conf method from serial.c
 * */
SerialConf* Init_WiFi(void *virtual_base)
{
	SerialConf *sc = create_serial_conf(virtual_base,
		RS232_ReceiverFifo, RS232_TransmitterFifo,
		RS232_InterruptEnableReg, RS232_InterruptIdentificationReg,
		RS232_FifoControlReg, RS232_LineControlReg, RS232_ModemControlReg,
		RS232_LineStatusReg, RS232_ModemStatusReg, RS232_ScratchReg,
		RS232_DivisorLatchLSB, RS232_DivisorLatchMSB);

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
	// Reset the Fifo�s in the FiFo Control Reg by setting bits 1 & 2
	*(sc->FifoControlReg) = *(sc->FifoControlReg) | 0x06;
	// Now Clear all bits in the FiFo control registers
	*(sc->FifoControlReg) = *(sc->FifoControlReg) ^  0x06;
	Flush(sc->LineStatusReg, sc->ReceiverFifo);
	char buf[10240] = "";
	exec_lua(sc, "dofile(\"send_text_message.lua\")", buf);

	char c;
	int got=0;
	char *expected = "connected";
	while (got < 9) {
		while(TestForReceivedData(sc->LineStatusReg) != 1) {;}
		c = (char) getchar_uart(sc->LineStatusReg, sc->ReceiverFifo);
		printf("%c", c);
		if (c == expected[got]) {
			got += 1;
		}
	}
	printf("\n");

	return sc;
}



//int _main() {
//	Init_WiFi();
//	char buf[1024] = "";
//
//	printf("Sending to dangle dongle\n");
//	int res = exec_lua("dofile(\"send_text_message.lua\")", buf);
//	printf("%s\n", buf);
//
//	char c;
//	int got=0;
//	char *expected = "connected";
//	while (got < 9) {
//		while(TestForReceivedData(RS232_LineStatusReg) != 1) {;}
//		c = (char) getchar_uart(RS232_LineStatusReg, RS232_ReceiverFifo);
//		printf("%c", c);
//		if (c == expected[got]) {
//			got += 1;
//		}
//	}
//	printf("\n");
//
//	res = exec_lua("send_sms(\"(204) 818-9230\",\"(778) 866-2529\",\"WiFi Chip ON!\")", buf);
//	printf("%s\n", buf);
//
//	while (1) {
//		if(TestForReceivedData(RS232_LineStatusReg) == 1) {
//			c = (char) getchar_uart(RS232_LineStatusReg, RS232_ReceiverFifo);
//			printf("%c", c);
//		}
//	}
//}

int exec_lua(SerialConf *sc, char * str, char * res) {
	int bytes_received = 0;
//	printf("executing %s as a Lua command\n", str);
	while (*str) {
		putchar_uart(*str, sc->LineStatusReg, sc->TransmitterFifo);

//		while(TestForReceivedData(sc->LineStatusReg) == 1) {
//			res[bytes_received++] = (char) getchar_uart(sc->LineStatusReg, sc->ReceiverFifo);
//		}
		str += 1;
	}
	putchar_uart('\n', sc->LineStatusReg , sc->TransmitterFifo);
//	while(TestForReceivedData(sc->LineStatusReg) == 1) {
//		res[bytes_received++] = (char) getchar_uart(sc->LineStatusReg, sc->ReceiverFifo);
//	}
//	while(TestForReceivedData(sc->LineStatusReg) == 1) {
//		res[bytes_received++] = (char) getchar_uart(sc->LineStatusReg, sc->ReceiverFifo);
//	}
//	printf("Got %u bytes\n", bytes_received);
	res[bytes_received] = '\0';
	return bytes_received;
}

void wait_for(SerialConf *sc, char *to_wait) {
	int i = 0;
	while (to_wait[i]) {
		if (TestForReceivedData(sc->LineStatusReg) == 1) {
			char c = (char) getchar_uart(sc->LineStatusReg, sc->ReceiverFifo);
			if (to_wait[i] == c) {
				i++;
			} else {
				i = 0;
			}
		}
	}
}

